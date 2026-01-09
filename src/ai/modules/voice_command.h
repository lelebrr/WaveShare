#pragma once
#include <Arduino.h>

enum VoiceCommand {
  CMD_NONE,
  CMD_WAKE_WORD, // "Ei Dragão"
  CMD_ATTACK_AUTO,
  CMD_STOP_ALL,
  CMD_STATUS,
  CMD_TOGGLE_WIFI
};

class LocalVoiceCommand {
public:
  LocalVoiceCommand();

  // Processa chunk de áudio e busca comandos (RNN-T ou Keyword Spotting Lite)
  VoiceCommand processAudioChunk(const int16_t *audioData, size_t samples);

  // Configura modelo (Tip 1: Quantized keyword spotting)
  bool loadModel(const char *path);

private:
  bool _isListening;
  float _confidenceThreshold;
  // ... buffers para janelamento de audio
};
