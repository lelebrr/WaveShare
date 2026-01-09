#include "neura9_inference.h"
#include "neura9_model_data.h"

// TFLite Micro includes (depende da lib instalada)
// Usando apenas os headers essenciais que costumam estar presentes em porting
// para ESP32
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/schema/schema_generated.h>
// #include <tensorflow/lite/version.h> // Removido pois causa erro em algumas
// libs

// Globais para evitar realocação
// Tip 10: Arena fixa, nunca alocar dinamicamente durante runtime
// Tip 6: Cache na PSRAM

NEURA9Inference neura9;

NEURA9Inference::NEURA9Inference() {
  _lastResult = {RESULT_UNKNOWN, 0.0f, 0};
  _lastFeatures.reset();
}

NEURA9Inference::~NEURA9Inference() {
  if (tensor_arena)
    free(tensor_arena);
}

void NEURA9Inference::setLiteMode(bool enabled) { _liteMode = enabled; }

bool NEURA9Inference::begin() {
  if (_initialized)
    return true;

  Serial.println("[NEURA9] Inicializando TFLite Micro na PSRAM...");

  // 1. Alocar Arena na PSRAM (Tip 6)
  tensor_arena = (uint8_t *)heap_caps_malloc(
      NEURA9_ARENA_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!tensor_arena) {
    Serial.println("[NEURA9] FALHA: Sem memória PSRAM para Arena!");
    return false;
  }

  // 2. Setup Error Reporter
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // 3. Map Model
  // Tip 22: Load NEURA9 model from flash to PSRAM blocks
  // Copying model to PSRAM avoids Flash XIP contention and potentially speeds
  // up inference
  static uint8_t *model_psram_copy = nullptr;
  if (!model_psram_copy) {
    if (g_neura9_model_data_len > 0) {
      model_psram_copy = (uint8_t *)heap_caps_malloc(g_neura9_model_data_len,
                                                     MALLOC_CAP_SPIRAM);
      if (model_psram_copy) {
        memcpy(model_psram_copy, g_neura9_model_data, g_neura9_model_data_len);
        Serial.println("[NEURA9] Modelo copiado para PSRAM (Performance++)");
      } else {
        Serial.println(
            "[NEURA9] Falha ao alocar modelo na PSRAM, usando Flash (XIP)");
      }
    }
  }

  // Use PSRAM copy if available, otherwise Flash
  const void *model_data_ptr =
      model_psram_copy ? model_psram_copy : g_neura9_model_data;
  model = tflite::GetModel(model_data_ptr);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.printf("[NEURA9] Erro: Versão do Schema %d != %d\n",
                  model->version(), TFLITE_SCHEMA_VERSION);
    return false;
  }

  // 4. Resolver Ops
  static tflite::MicroMutableOpResolver<5> resolver;
  resolver.AddFullyConnected();
  resolver.AddSoftmax();
  resolver.AddRelu();
  resolver.AddQuantize();
  resolver.AddDequantize();

  // 5. Interpreter
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, NEURA9_ARENA_SIZE, error_reporter);
  interpreter = &static_interpreter;

  // 6. Allocate Tensors
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    Serial.println("[NEURA9] Erro ao alocar tensores!");
    return false;
  }

  // 7. Get Pointers
  input = interpreter->input(0);
  output = interpreter->output(0);

  _initialized = true;
  Serial.println("[NEURA9] Pronto! Engine TFLite rodando na PSRAM.");
  return true;
}

bool NEURA9Inference::shouldRunInference(
    const Neura9Features &currentFeatures) {
  if (!_initialized)
    return false;

  uint32_t now = millis();

  // Tip 5: Scheduling Inteligente
  // Se detectou algo suspeito recentemente, aumenta frequência (150ms).
  // Se está tudo safe, relaxa (800ms).
  if (_lastResult.category != RESULT_SAFE &&
      _lastResult.category != RESULT_UNKNOWN) {
    _adaptiveInterval = 150;
  } else {
    _adaptiveInterval = 800; // Idle
  }

  if (now - _lastInferenceTime < _adaptiveInterval) {
    return false;
  }

  // Tip 12: Caching de Predição - Delta Check
  // Se as features mudaram muito pouco, não roda inferência pesada, retorna
  // cache.
  float delta = 0.0f;
  delta += abs(currentFeatures.rssi_norm - _lastFeatures.rssi_norm);
  delta += abs(currentFeatures.beacon_rate - _lastFeatures.beacon_rate);
  delta += abs(currentFeatures.deauth_rate - _lastFeatures.deauth_rate);

  // Se mudou menos de 5% no total das features chave
  if (delta < 0.05f &&
      (now - _lastInferenceTime < 2000)) { // Força a cada 2s mesmo sem mudança
    return false;                          // Usa cache
  }

  return true;
}

InferenceResult NEURA9Inference::predict(const Neura9Features &features) {
  // Check if we should actually run (Tip 12 logic inside)
  if (!shouldRunInference(features)) {
    // Return cached result (Tip 12)
    return _lastResult;
  }

  uint32_t start = millis();

  // Preencher Input Tensor
  if (input->type == kTfLiteFloat32) {
    float *input_data = input->data.f;
    // Tip 2: 56 Features Mapping
    // Mapeia features da struct para o tensor plano
    input_data[0] = features.rssi_norm;
    input_data[1] = features.snr_norm;
    input_data[2] = features.beacon_rate;
    input_data[3] = features.probereq_rate;
    input_data[4] = features.deauth_rate;
    input_data[5] = features.assocreq_rate;
    input_data[6] = features.eapol_rate;
    input_data[7] = features.management_frame_ratio;

    // ... mapeamento das outras features (simplificado para loop se fosse
    // array) Para 56 features, idealmente teríamos um loop ou memcpy se struct
    // alinhada

  } else if (input->type == kTfLiteInt8) {
    int8_t *input_data = input->data.int8;

    // Helper lambda for quantization: value -> (value / scale) + zero_point
    auto quantize = [&](float val) -> int8_t {
      float q_val = (val / input->params.scale) + input->params.zero_point;
      return (int8_t)max(-128.0f, min(127.0f, q_val));
    };

    // Tip 2: 56 Features Mapping (Quantized)
    input_data[0] = quantize(features.rssi_norm);
    input_data[1] = quantize(features.snr_norm);
    input_data[2] = quantize(features.beacon_rate);
    input_data[3] = quantize(features.probereq_rate);
    input_data[4] = quantize(features.deauth_rate);
    input_data[5] = quantize(features.assocreq_rate);
    input_data[6] = quantize(features.eapol_rate);
    input_data[7] = quantize(features.management_frame_ratio);

    // Fill remaining features with 0 (or appropriate default) if not used yet
    // Assuming 8 features used for now out of 56
    for (int i = 8; i < input->dims->data[1]; i++) {
      input_data[i] = input->params.zero_point;
    }
  }

  // Rodar Inferência
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    Serial.println("[NEURA9] Invoke falhou!");
    return {RESULT_UNKNOWN, 0.0f, 0};
  }

  // Processar Output
  float max_score = 0;
  int max_index = -1;

  if (output->type == kTfLiteFloat32) {
    // ... float parsing
  } else if (output->type == kTfLiteInt8) { // Tip 1: INT8 Output
    int8_t *output_data = output->data.int8;
    for (int i = 0; i < output->dims->data[1]; i++) {
      // Dequantize: (value - zero_point) * scale
      float val =
          (output_data[i] - output->params.zero_point) * output->params.scale;
      if (val > max_score) {
        max_score = val;
        max_index = i;
      }
    }
  }

  _lastInferenceTime = millis();
  _lastFeatures = features; // Update cache

  _lastResult = {(Neura9Result)max_index, (float)max_score,
                 _lastInferenceTime - start};

  return _lastResult;
}
