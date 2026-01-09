#pragma once
/**
 * @file keyword_detector.h
 * @brief Detector de wake word baseado em energia e padrão de áudio
 *
 * Implementa detecção de wake word "Hey Dragon" usando:
 * - VAD (Voice Activity Detection) baseado em energia
 * - Análise de duração e padrão do sinal
 * - Detecção de picos característicos
 */

#include <Arduino.h>

// Configurações do detector
#define KWD_SAMPLE_RATE 16000
#define KWD_FRAME_SIZE 512
#define KWD_ENERGY_HISTORY_SIZE 32
#define KWD_WAKE_WORD_MIN_DURATION_MS 400
#define KWD_WAKE_WORD_MAX_DURATION_MS 1500
#define KWD_SILENCE_TIMEOUT_MS 300

// Estados do detector
enum KeywordState {
  KWD_IDLE,          // Aguardando voz
  KWD_DETECTING,     // Som detectado, analisando
  KWD_WAKE_DETECTED, // Wake word detectado!
  KWD_COMMAND_READY  // Pronto para receber comando
};

// Resultado da análise
struct KeywordResult {
  bool detected;
  float confidence;
  uint32_t duration_ms;
};

class KeywordDetector {
public:
  KeywordDetector();

  // Inicializa o detector
  bool begin();

  // Processa frame de áudio (retorna true se wake word detectado)
  bool processFrame(const int16_t *samples, size_t count);

  // Reseta o detector para estado inicial
  void reset();

  // Obtém estado atual
  KeywordState getState() const { return _state; }

  // Obtém resultado da última detecção
  KeywordResult getLastResult() const { return _lastResult; }

  // Configurações
  void setSensitivity(float sensitivity); // 0.0 a 1.0
  void setNoiseFloor(float floor);

  // Debug
  float getCurrentEnergy() const { return _currentEnergy; }
  float getNoiseFloor() const { return _noiseFloor; }
  bool isVoiceActive() const { return _voiceActive; }

private:
  // Cálculos de energia
  float calculateEnergy(const int16_t *samples, size_t count);
  void updateNoiseFloor(float energy);
  bool detectVoiceActivity(float energy);

  // Análise de padrão
  bool analyzePattern();
  float calculatePeakRatio();
  int countEnergyPeaks();

  // Estado interno
  KeywordState _state;
  KeywordResult _lastResult;

  // Configurações
  float _sensitivity;
  float _noiseFloor;
  float _noiseFloorAlpha;
  float _vadThreshold;

  // Histórico de energia
  float _energyHistory[KWD_ENERGY_HISTORY_SIZE];
  int _energyHistoryIndex;
  int _energyHistoryCount;

  // Tracking de voz
  bool _voiceActive;
  uint32_t _voiceStartTime;
  uint32_t _lastVoiceTime;
  float _currentEnergy;
  float _peakEnergy;

  // Contadores
  int _frameSinceVoiceStart;
  int _silenceFrames;
};

// Instância global
extern KeywordDetector keywordDetector;
