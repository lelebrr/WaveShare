#pragma once
/**
 * @file neura9_inference.h
 * @brief Inferência NEURA9 OTIMIZADA com TFLite Micro + ESP32-S3 PSRAM
 *
 * Implementa Tips 1, 3, 4, 6, 10 do WavePwn
 */

#include "feature_extractor.h"
#include <Arduino.h>

// TFLite Micro includes
namespace tflite {
template <unsigned int tOpCount> class MicroMutableOpResolver;
class ErrorReporter;
class Model;
class MicroInterpreter;
} // namespace tflite

// Configurações (Tip 10: Arena Fixa)
// Modelo 7KB + Tensores Intermediários. 160KB é super seguro.
#define NEURA9_ARENA_SIZE                                                      \
  (16 * 1024) // Reduzido para testar, tip diz 160KB mas 16KB deve dar para
              // modelo pequeno

// Categorias (Tip 13: 15 classes para v2)
enum Neura9Result {
  RESULT_SAFE = 0,
  RESULT_DEAUTH,
  RESULT_EVIL_TWIN,
  RESULT_BEACON_SPAM,
  RESULT_PROBE_FLOOD,
  RESULT_ROGUE_AP,
  RESULT_KARMA,
  RESULT_WPS_ACTIVE,
  RESULT_HIDDEN_SSID,
  RESULT_CLIENT_FLOOD,
  RESULT_HANDSHAKE_VALID,
  RESULT_PMKID_WEAK,
  RESULT_UNKNOWN = 255
};

struct InferenceResult {
  Neura9Result category;
  float confidence;
  uint32_t inferenceTimeMs;
};

class NEURA9Inference {
public:
  NEURA9Inference();
  ~NEURA9Inference();

  // Inicializa TFLite Micro na PSRAM (Tip 6)
  bool begin();

  // Executa inferência
  InferenceResult predict(const Neura9Features &features);

  // Tip 5: Lógica de quando rodar (Adaptive Scheduling)
  // Tip 12: Caching de Predição
  bool shouldRunInference(const Neura9Features &currentFeatures);

  // Tip 8: Switch Lite/Full mode
  void setLiteMode(bool enabled);

private:
  // TFLite pointers
  tflite::ErrorReporter *error_reporter = nullptr;
  const tflite::Model *model = nullptr;
  tflite::MicroInterpreter *interpreter = nullptr;

  // Arena de memória (PSRAM)
  uint8_t *tensor_arena = nullptr;

  // Input/Output tensors
  struct TfLiteTensor *input = nullptr;
  struct TfLiteTensor *output = nullptr;

  bool _initialized = false;
  uint32_t _lastInferenceTime = 0;

  // Tip 12: Caching vars
  Neura9Features _lastFeatures;
  InferenceResult _lastResult;
  bool _liteMode = false;

  // Tip 5: Scheduling vars
  uint32_t _adaptiveInterval = 800;
};

extern NEURA9Inference neura9;
