/**
 * @file keyword_detector.cpp
 * @brief Implementação do detector de wake word
 *
 * Usa detecção baseada em energia para identificar o wake word "Hey Dragon"
 * sem necessidade de modelos de ML pesados.
 */

#include "keyword_detector.h"
#include <math.h>

// Instância global
KeywordDetector keywordDetector;

// Constantes de calibração
static const float DEFAULT_SENSITIVITY = 0.7f;
static const float DEFAULT_NOISE_FLOOR = 100.0f;
static const float NOISE_FLOOR_ALPHA = 0.05f;
static const float VAD_MULTIPLIER = 3.0f;

// Padrões esperados para "Hey Dragon" (2 sílabas + 2 sílabas)
static const int MIN_PEAKS_FOR_WAKE = 2;  // Mínimo 2 picos de energia
static const int MAX_PEAKS_FOR_WAKE = 5;  // Máximo 5 picos
static const float PEAK_RATIO_MIN = 1.5f; // Pico deve ser 1.5x a média

KeywordDetector::KeywordDetector()
    : _state(KWD_IDLE), _sensitivity(DEFAULT_SENSITIVITY),
      _noiseFloor(DEFAULT_NOISE_FLOOR), _noiseFloorAlpha(NOISE_FLOOR_ALPHA),
      _vadThreshold(DEFAULT_NOISE_FLOOR * VAD_MULTIPLIER),
      _energyHistoryIndex(0), _energyHistoryCount(0), _voiceActive(false),
      _voiceStartTime(0), _lastVoiceTime(0), _currentEnergy(0), _peakEnergy(0),
      _frameSinceVoiceStart(0), _silenceFrames(0) {

  memset(_energyHistory, 0, sizeof(_energyHistory));
  _lastResult = {false, 0.0f, 0};
}

bool KeywordDetector::begin() {
  Serial.println("[KWD] Inicializando detector de wake word...");
  Serial.printf("[KWD] Sensibilidade: %.2f\n", _sensitivity);
  Serial.printf("[KWD] Noise floor inicial: %.2f\n", _noiseFloor);

  reset();

  Serial.println("[KWD] Detector pronto! Wake word: 'Hey Dragon'");
  return true;
}

void KeywordDetector::reset() {
  _state = KWD_IDLE;
  _voiceActive = false;
  _voiceStartTime = 0;
  _lastVoiceTime = 0;
  _frameSinceVoiceStart = 0;
  _silenceFrames = 0;
  _peakEnergy = 0;
  _energyHistoryIndex = 0;
  _energyHistoryCount = 0;
  memset(_energyHistory, 0, sizeof(_energyHistory));
}

bool KeywordDetector::processFrame(const int16_t *samples, size_t count) {
  if (!samples || count == 0)
    return false;

  // Calcula energia do frame
  _currentEnergy = calculateEnergy(samples, count);

  // Atualiza noise floor durante silêncio
  if (!_voiceActive && _currentEnergy < _noiseFloor * 1.5f) {
    updateNoiseFloor(_currentEnergy);
  }

  // Detecta atividade de voz
  bool wasVoiceActive = _voiceActive;
  _voiceActive = detectVoiceActivity(_currentEnergy);

  uint32_t now = millis();

  switch (_state) {
  case KWD_IDLE:
    if (_voiceActive && !wasVoiceActive) {
      // Início de voz detectado
      _state = KWD_DETECTING;
      _voiceStartTime = now;
      _frameSinceVoiceStart = 0;
      _peakEnergy = _currentEnergy;
      _silenceFrames = 0;
      Serial.println("[KWD] Voz detectada, analisando...");
    }
    break;

  case KWD_DETECTING:
    _frameSinceVoiceStart++;

    // Armazena energia no histórico
    _energyHistory[_energyHistoryIndex] = _currentEnergy;
    _energyHistoryIndex = (_energyHistoryIndex + 1) % KWD_ENERGY_HISTORY_SIZE;
    if (_energyHistoryCount < KWD_ENERGY_HISTORY_SIZE)
      _energyHistoryCount++;

    // Atualiza pico
    if (_currentEnergy > _peakEnergy) {
      _peakEnergy = _currentEnergy;
    }

    if (_voiceActive) {
      _lastVoiceTime = now;
      _silenceFrames = 0;
    } else {
      _silenceFrames++;
    }

    // Verifica timeout de silêncio (fim da fala)
    if (now - _lastVoiceTime > KWD_SILENCE_TIMEOUT_MS || _silenceFrames > 10) {
      uint32_t duration = now - _voiceStartTime;

      // Analisa se corresponde ao wake word
      if (duration >= KWD_WAKE_WORD_MIN_DURATION_MS &&
          duration <= KWD_WAKE_WORD_MAX_DURATION_MS) {

        if (analyzePattern()) {
          _lastResult.detected = true;
          _lastResult.duration_ms = duration;
          _lastResult.confidence = calculatePeakRatio() / 3.0f; // Normaliza

          _state = KWD_WAKE_DETECTED;
          Serial.printf("[KWD] ✓ WAKE WORD DETECTADO! (%.0fms, conf: %.2f)\n",
                        (float)duration, _lastResult.confidence);
          return true;
        }
      }

      // Não foi wake word, volta ao idle
      Serial.printf("[KWD] Descartado (duração: %dms)\n", duration);
      reset();
    }

    // Timeout máximo
    if (now - _voiceStartTime > KWD_WAKE_WORD_MAX_DURATION_MS + 500) {
      Serial.println("[KWD] Timeout, resetando...");
      reset();
    }
    break;

  case KWD_WAKE_DETECTED:
    // Aguarda ser processado externamente
    break;

  case KWD_COMMAND_READY:
    // Pronto para comando, gerenciado externamente
    break;
  }

  return false;
}

float KeywordDetector::calculateEnergy(const int16_t *samples, size_t count) {
  if (count == 0)
    return 0;

  // RMS (Root Mean Square) energy
  float sum = 0;
  for (size_t i = 0; i < count; i++) {
    float sample = (float)samples[i];
    sum += sample * sample;
  }
  return sqrtf(sum / count);
}

void KeywordDetector::updateNoiseFloor(float energy) {
  // Média móvel exponencial para adaptar ao ruído ambiente
  _noiseFloor =
      _noiseFloor * (1.0f - _noiseFloorAlpha) + energy * _noiseFloorAlpha;

  // Atualiza threshold de VAD
  _vadThreshold = _noiseFloor * VAD_MULTIPLIER * (2.0f - _sensitivity);
}

bool KeywordDetector::detectVoiceActivity(float energy) {
  return energy > _vadThreshold;
}

bool KeywordDetector::analyzePattern() {
  if (_energyHistoryCount < 4)
    return false;

  // Conta picos de energia
  int peaks = countEnergyPeaks();

  // Calcula razão pico/média
  float peakRatio = calculatePeakRatio();

  // Debug
  Serial.printf("[KWD] Análise: %d picos, razão: %.2f\n", peaks, peakRatio);

  // Critérios para "Hey Dragon":
  // - 2 a 5 picos (duas palavras com sílabas)
  // - Razão pico/média > 1.5 (voz clara)
  // - Ajustado pela sensibilidade

  float minRatio = PEAK_RATIO_MIN * (2.0f - _sensitivity);

  return (peaks >= MIN_PEAKS_FOR_WAKE && peaks <= MAX_PEAKS_FOR_WAKE &&
          peakRatio >= minRatio);
}

int KeywordDetector::countEnergyPeaks() {
  if (_energyHistoryCount < 3)
    return 0;

  int peaks = 0;
  float avgEnergy = 0;

  // Calcula média
  for (int i = 0; i < _energyHistoryCount; i++) {
    avgEnergy += _energyHistory[i];
  }
  avgEnergy /= _energyHistoryCount;

  // Conta picos (frames acima de 1.5x a média, com derivada positiva->negativa)
  for (int i = 1; i < _energyHistoryCount - 1; i++) {
    float prev = _energyHistory[i - 1];
    float curr = _energyHistory[i];
    float next = _energyHistory[i + 1];

    if (curr > avgEnergy * 1.3f && curr > prev && curr > next) {
      peaks++;
    }
  }

  return peaks;
}

float KeywordDetector::calculatePeakRatio() {
  if (_energyHistoryCount < 2)
    return 1.0f;

  float sum = 0;
  float maxEnergy = 0;

  for (int i = 0; i < _energyHistoryCount; i++) {
    sum += _energyHistory[i];
    if (_energyHistory[i] > maxEnergy) {
      maxEnergy = _energyHistory[i];
    }
  }

  float avg = sum / _energyHistoryCount;
  if (avg < 1.0f)
    avg = 1.0f;

  return maxEnergy / avg;
}

void KeywordDetector::setSensitivity(float sensitivity) {
  _sensitivity = constrain(sensitivity, 0.0f, 1.0f);
  Serial.printf("[KWD] Sensibilidade: %.2f\n", _sensitivity);
}

void KeywordDetector::setNoiseFloor(float floor) {
  _noiseFloor = max(floor, 10.0f);
  _vadThreshold = _noiseFloor * VAD_MULTIPLIER * (2.0f - _sensitivity);
}
