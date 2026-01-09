#include "ai_manager.h"
#include "../core/globals.h"
#include "../hardware/audio_driver.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// NEURA9 Integration
#include "feature_extractor.h"
#include "neura9_inference.h"

// New Detectors (Tips 13-25)
#include "detectors/anomaly_detector.h"
#include "detectors/tiny_classifiers.h"
#include "detectors/voice_command.h"

#include "data_collector.h"

AiManager aiManager;
FeatureExtractor featureExtractor;
AnomalyDetector anomalyDetector;
VoiceCommandAI voiceAI;
DataCollector dataCollector;

// Tip 11: Task wrapper
void aiTaskFunction(void *pvParameters) {
  while (true) {
    aiManager.loop();
    // Yield to watchdog
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

AiManager::AiManager() : _aiTaskHandle(NULL) {}

bool AiManager::begin() {
  // Tip 6: Inicializa TFLite na PSRAM
  if (!neura9.begin()) {
    Serial.println("[AI] FALHA ao iniciar NEURA9 TFLite!");
  }

  // Tip 11: Inicia Task na Core 0 (para não competir com Loop/UI na Core 1)
  xTaskCreatePinnedToCore(
      aiTaskFunction, "NEURA9_AI_Task",
      8192, // Stack size (8KB deve dar, arena está na PSRAM)
      NULL,
      1, // Priority (Baixa)
      &_aiTaskHandle,
      0 // Core 0
  );

  return true;
}

void AiManager::loop() {
  // Tip 7: Desativar se bateria < 20% e não carregando
  if (g_state.battery_percent < 20 && !g_state.is_charging &&
      !g_state.auto_attack_enabled) {
    vTaskDelay(5000 / portTICK_PERIOD_MS); // Hiberna IA
    return;
  }

  // Extrai features
  const Neura9Features &features = featureExtractor.getFeatures();

  // Tip 12 & 5: predict() agora gerencia internamente se deve rodar ou usar
  // cache
  InferenceResult res = neura9.predict(features);

  if (res.category != RESULT_UNKNOWN && res.confidence > 0.7f) {
    // Ação baseada na inferência
    // Serial.printf("[AI] Detected: %d (Conf: %.2f) Time: %d ms\n",
    // res.category, res.confidence, res.inferenceTimeMs);
  }

  // Tip 18 & 26: Anomaly Detection + Auto Data Collection
  // Se detectar anomalia, salva os dados para treino futuro (Tip 26)
  if (anomalyDetector.isAnomalous(features.rssi_norm * 100,
                                  features.probereq_rate * 10,
                                  features.deauth_rate)) {
    dataCollector.logFeatures(features, res.category);
  }

  // Tip 25: Micro Sleep (Desliga tela se o usuario dormiu)
  // Assumindo acesso a variaveis globais de touch/IMU (simulado)
  // static uint32_t lastTouch = 0; // Placeholder
  // if (ContextAwareAI::detectMicroSleep(0.01f, lastTouch)) {
  //     // turnOffScreen();
  // }

  // Feature window reset depende da lógica de coleta.
  // Se rodou inferência nova, resetamos.
  if (res.inferenceTimeMs > 0) { // Significa que rodou de verdade (não cache)
    featureExtractor.resetWindow();
  }

  // Pequeno delay para não travar task loop, mas o timing real é controlado lá
  // dentro
  vTaskDelay(50 / portTICK_PERIOD_MS);
}

// ... Rest of existing methods (Cloud AI) ...

static String _apiPost(const char *url, const char *payload,
                       const char *apiKey) {
  if (WiFi.status() != WL_CONNECTED)
    return "";

  WiFiClientSecure *client = new WiFiClientSecure;
  client->setInsecure(); // Simplify interaction without cert bundles

  HTTPClient http;
  http.begin(*client, url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", String("Bearer ") + apiKey);

  int httpCode = http.POST(payload);
  String response = "";

  if (httpCode == HTTP_CODE_OK) {
    response = http.getString();
  } else {
    Serial.printf("[AI] Error %d: %s\n", httpCode, http.getString().c_str());
  }

  http.end();
  delete client;
  return response;
}

String AiManager::getChatCompletion(String userText) {
  if (strlen(g_state.openai_api_key) < 5) {
    Serial.println("[AI] API Key not set");
    return "Please configure API Key";
  }

  DynamicJsonDocument doc(2048);
  doc["model"] = g_state.openai_model;

  JsonArray messages = doc.createNestedArray("messages");

  JsonObject sysMsg = messages.createNestedObject();
  sysMsg["role"] = "system";
  sysMsg["content"] = "You are a cyber-dragon named NEURA9 inside a hacking "
                      "watch. Be brief, cool, and edgy.";

  JsonObject msg = messages.createNestedObject();
  msg["role"] = "user";
  msg["content"] = userText;

  String payload;
  serializeJson(doc, payload);

  String response = _apiPost("https://api.openai.com/v1/chat/completions",
                             payload.c_str(), g_state.openai_api_key);

  if (response.length() > 0) {
    DynamicJsonDocument resDoc(4096);
    DeserializationError error = deserializeJson(resDoc, response);
    if (!error) {
      const char *content = resDoc["choices"][0]["message"]["content"];
      if (content)
        return String(content);
    } else {
      Serial.println("[AI] JSON Parse Error");
    }
  }
  return "";
}

bool AiManager::synthesizeSpeech(String text, String voice) {
  if (WiFi.status() != WL_CONNECTED)
    return false;

  WiFiClientSecure *client = new WiFiClientSecure;
  client->setInsecure();

  HTTPClient http;
  http.begin(*client, "https://api.openai.com/v1/audio/speech");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization",
                 String("Bearer ") + String(g_state.openai_api_key));

  DynamicJsonDocument doc(1024);
  doc["model"] = "tts-1"; // Use tts-1 for speed
  doc["input"] = text;
  doc["voice"] = voice;
  doc["response_format"] = "pcm"; // 16-bit PCM

  String payload;
  serializeJson(doc, payload);

  int httpCode = http.POST(payload);
  if (httpCode == HTTP_CODE_OK) {
    int len = http.getSize();
    uint8_t buff[512];
    WiFiClient *stream = http.getStreamPtr();

    while (http.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        int c = stream->readBytes(
            buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        if (c > 0) {
          audioDriver.playRaw((int16_t *)buff, c / 2);
          if (len > 0)
            len -= c;
        }
      }
      delay(1);
    }
  } else {
    Serial.printf("[AI] TTS Error %d\n", httpCode);
    return false;
  }

  http.end();
  delete client;
  return true;
}

String AiManager::transcribeAudio(const int16_t *audioData, size_t size) {
  return "";
}

// Tip 27: OTA de modelo IA
bool AiManager::updateModelFromWeb(String url) {
  // Simulação: Baixa arquivo .tflite, verifica checksum e substitui na FS
  // Como usamos tabela de ponteiros, na próxima carga (begin) ele pegaria o
  // novo Serial.println("[AI] Updating model from " + url);
  return true;
}
