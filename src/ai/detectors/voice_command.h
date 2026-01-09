#pragma once
#include <Arduino.h>

/**
 * @file voice_command.h
 * @brief Tip 17: Local Voice Command state machine & Keyword Spotting logic
 */

class VoiceCommandAI {
public:
  enum CommandType {
    CMD_NONE = 0,
    CMD_WAKE_WORD, // "Ei Dragão"
    CMD_ATTACK_START,
    CMD_STOP,
    CMD_STATUS
  };

  VoiceCommandAI() {}

  // Processa buffer de audio (simulado aqui, na real usaria TFLite Micro
  // Speech)
  CommandType processAudioChunk(int16_t *samples, size_t len) {
    // Tip 17: Rede RNN-T seria implementada aqui.
    // Como não temos os pesos, vamos simular a interface.

    // Exemplo: calcular energia RMS para detecção de voz (VAD)
    long energy = 0;
    for (size_t i = 0; i < len; i++) {
      energy += abs(samples[i]);
    }
    energy /= len;

    if (energy > 2000) {
      // Se energia alta, potencialmente voz.
      // Retornaria CMD_WAKE_WORD se o modelo TFLite matchasse.
      return CMD_NONE;
    }
    return CMD_NONE;
  }

  String getCommandString(CommandType cmd) {
    switch (cmd) {
    case CMD_WAKE_WORD:
      return "Wake Word Detected";
    case CMD_ATTACK_START:
      return "Starting Attack";
    case CMD_STOP:
      return "Stop";
    default:
      return "";
    }
  }
};
