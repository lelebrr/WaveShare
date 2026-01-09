#include "voice_command.h"

LocalVoiceCommand::LocalVoiceCommand()
    : _isListening(false), _confidenceThreshold(0.85f) {}

bool LocalVoiceCommand::loadModel(const char *path) {
  // Carregar modelo KWS (TinyML)
  return true;
}

VoiceCommand LocalVoiceCommand::processAudioChunk(const int16_t *audioData,
                                                  size_t samples) {
  // Implementação de Keyword Spotting (KWS)
  // Se detectou "Ei Dragão", entra em modo escuta.

  // Simulação: Se volume alto e padrão reconhecido
  // Aqui usaria TFLite Micro com modelo `voice_kws.tflite`

  // Placeholder logic:
  // Retornar CMD_NONE exceto se detectado
  return CMD_NONE;
}
