#include "ai_manager.h"
#include "../core/config.h"
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

AiManager::AiManager() : _aiTaskHandle(NULL), _apiKey(OPENAI_API_KEY) {}

bool AiManager::begin() {
  Serial.println("[AI] Initializing AI Manager...");
  
  // Load API key from config
  if (String(_apiKey).startsWith("sk-YOUR")) {
    Serial.println("[AI] WARNING: OpenAI API key not configured! TTS/STT will not work.");
    Serial.println("[AI] Edit OPENAI_API_KEY in src/core/config.h");
  } else {
    Serial.println("[AI] OpenAI API key configured.");
  }
  
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

String AiManager::transcribeAudio(const int16_t *audioData, size_t sampleCount) {
  // Whisper API requires multipart/form-data with WAV file
  // Audio format: 16kHz, 16-bit, mono
  
  if (sampleCount == 0 || !audioData) return "";
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[AI] STT: No WiFi connection");
    return "";
  }
  
  Serial.printf("[AI] Transcribing %d samples via Whisper API...\n", sampleCount);
  
  // Build WAV header
  size_t audioBytes = sampleCount * 2; // 16-bit samples
  size_t wavSize = 44 + audioBytes;    // 44-byte header + data
  
  // WAV header structure
  uint8_t wavHeader[44] = {
    'R', 'I', 'F', 'F',                 // ChunkID
    (uint8_t)(wavSize - 8), (uint8_t)((wavSize - 8) >> 8), 
    (uint8_t)((wavSize - 8) >> 16), (uint8_t)((wavSize - 8) >> 24), // ChunkSize
    'W', 'A', 'V', 'E',                 // Format
    'f', 'm', 't', ' ',                 // Subchunk1ID
    16, 0, 0, 0,                        // Subchunk1Size (16 for PCM)
    1, 0,                               // AudioFormat (1 = PCM)
    1, 0,                               // NumChannels (1 = mono)
    0x80, 0x3E, 0, 0,                   // SampleRate (16000)
    0x00, 0x7D, 0, 0,                   // ByteRate (16000 * 1 * 2 = 32000)
    2, 0,                               // BlockAlign (NumChannels * BitsPerSample/8)
    16, 0,                              // BitsPerSample
    'd', 'a', 't', 'a',                 // Subchunk2ID
    (uint8_t)audioBytes, (uint8_t)(audioBytes >> 8),
    (uint8_t)(audioBytes >> 16), (uint8_t)(audioBytes >> 24) // Subchunk2Size
  };
  
  // Multipart boundary
  String boundary = "----ESP32WavePwnBoundary" + String(millis());
  
  // Build multipart body
  String bodyStart = "--" + boundary + "\r\n";
  bodyStart += "Content-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\n";
  bodyStart += "Content-Type: audio/wav\r\n\r\n";
  
  String bodyModel = "\r\n--" + boundary + "\r\n";
  bodyModel += "Content-Disposition: form-data; name=\"model\"\r\n\r\n";
  bodyModel += "whisper-1";
  
  String bodyLanguage = "\r\n--" + boundary + "\r\n";
  bodyLanguage += "Content-Disposition: form-data; name=\"language\"\r\n\r\n";
  bodyLanguage += "pt"; // Portuguese
  
  String bodyEnd = "\r\n--" + boundary + "--\r\n";
  
  // Calculate total content length
  size_t contentLength = bodyStart.length() + 44 + audioBytes + 
                         bodyModel.length() + bodyLanguage.length() + bodyEnd.length();
  
  WiFiClientSecure *client = new WiFiClientSecure();
  client->setInsecure();
  
  HTTPClient http;
  http.begin(*client, "https://api.openai.com/v1/audio/transcriptions");
  http.addHeader("Authorization", "Bearer " + _apiKey);
  http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
  http.addHeader("Content-Length", String(contentLength));
  
  // Build complete payload in chunks
  // Note: For large audio, this should be streamed, but ESP32 has limited RAM
  uint8_t* payload = (uint8_t*)ps_malloc(contentLength);
  if (!payload) {
    Serial.println("[AI] STT: Failed to allocate payload buffer");
    delete client;
    return "";
  }
  
  size_t offset = 0;
  
  // Copy body start
  memcpy(payload + offset, bodyStart.c_str(), bodyStart.length());
  offset += bodyStart.length();
  
  // Copy WAV header
  memcpy(payload + offset, wavHeader, 44);
  offset += 44;
  
  // Copy audio data
  memcpy(payload + offset, audioData, audioBytes);
  offset += audioBytes;
  
  // Copy model field
  memcpy(payload + offset, bodyModel.c_str(), bodyModel.length());
  offset += bodyModel.length();
  
  // Copy language field
  memcpy(payload + offset, bodyLanguage.c_str(), bodyLanguage.length());
  offset += bodyLanguage.length();
  
  // Copy body end
  memcpy(payload + offset, bodyEnd.c_str(), bodyEnd.length());
  
  // Send request
  int httpCode = http.POST(payload, contentLength);
  free(payload);
  
  String result = "";
  
  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.printf("[AI] Whisper response: %s\n", response.c_str());
    
    // Parse JSON response
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, response) == DeserializationError::Ok) {
      result = doc["text"].as<String>();
      Serial.printf("[AI] Transcription: %s\n", result.c_str());
    }
  } else {
    Serial.printf("[AI] Whisper error: %d - %s\n", httpCode, http.getString().c_str());
  }
  
  http.end();
  delete client;
  
  return result;
}

// Tip 27: OTA de modelo IA
bool AiManager::updateModelFromWeb(String url) {
  // Simulação: Baixa arquivo .tflite, verifica checksum e substitui na FS
  // Como usamos tabela de ponteiros, na próxima carga (begin) ele pegaria o
  // novo Serial.println("[AI] Updating model from " + url);
  return true;
}
