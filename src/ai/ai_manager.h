#pragma once
#include <Arduino.h>

class AiManager {
public:
  AiManager();
  bool begin();

  // Transcreve audio para texto (Whisper)
  String transcribeAudio(const int16_t *audioData, size_t size);

  // Obtem resposta do ChatGPT
  String getChatCompletion(String userText);

  // Sintetiza fala (TTS)
  bool synthesizeSpeech(String text, String voice);

  // Tip 27: OTA Update
  bool updateModelFromWeb(String url);

  // Processa interacao de voz
  void processVoiceInteraction();

  // Tip 11: Task loop
  void loop();

#include "detectors/tiny_classifiers.h"

  // Expose Tools for UI/Web
  bool checkPmkidWeakness(String ssid, String oui, uint8_t *data) {
    return TinyClassifiers::isPmkidWeak(ssid, oui, data);
  }

  int checkHandshakeQuality(int half, int anonces, int snr, int gap) {
    return TinyClassifiers::predictHandshakeStrength(half, anonces, snr, gap);
  }

private:
  String _apiKey;
  TaskHandle_t _aiTaskHandle;
};

extern AiManager aiManager;
