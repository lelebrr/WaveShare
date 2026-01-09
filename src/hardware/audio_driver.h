#pragma once
/**
 * @file audio_driver.h
 * @brief Driver de áudio I2S para WavePwn usando ES8311 codec
 *
 * O Waveshare ESP32-S3-Touch-AMOLED-1.8 possui:
 * - ES8311: Codec de áudio I2S (speaker + microfone)
 * - Speaker interno
 * - Microfone interno
 * - Amplificador PA (Power Amplifier) no GPIO 46
 */

#include "../core/pin_definitions.h"
#include <Arduino.h>

// Frequências de amostragem suportadas
#define AUDIO_SAMPLE_RATE_8K 8000
#define AUDIO_SAMPLE_RATE_16K 16000
#define AUDIO_SAMPLE_RATE_22K 22050
#define AUDIO_SAMPLE_RATE_44K 44100

// Notas musicais (Hz)
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_REST 0

// Tipos de som predefinidos
enum SoundType {
  SOUND_BOOT,
  SOUND_SUCCESS,
  SOUND_ERROR,
  SOUND_ALERT,
  SOUND_SCAN_START,
  SOUND_SCAN_FOUND,
  SOUND_ATTACK_START,
  SOUND_ATTACK_COMPLETE,
  SOUND_HANDSHAKE,
  SOUND_CLICK,
  SOUND_BEEP
};

// Estrutura de nota
struct MelodyNote {
  int frequency;
  int duration; // ms
};

class AudioDriver {
public:
  AudioDriver();

  // Inicializa o driver (ES8311 + I2S)
  bool begin();

  // Para o áudio e libera recursos
  void end();

  // Toca um som predefinido
  void playSound(SoundType type);

  // Toca uma nota única (frequência em Hz, duração em ms)
  void playTone(int frequency, int duration);

  // Toca stream raw (mono 16-bit PCM)
  void playRaw(const int16_t *samples, size_t count);

  // Toca melodia customizada
  void playMelody(const MelodyNote *melody, int length);

  // Para qualquer som em reprodução
  void stop();

  // Controle de volume (0-100)
  void setVolume(int volume);
  int getVolume() const { return _volume; }

  // Ativa/desativa sons
  void setMuted(bool muted);
  bool isMuted() const { return _muted; }

  // Verifica se está tocando
  bool isPlaying() const { return _playing; }

  // Verifica se inicializado
  bool isInitialized() const { return _initialized; }

  // Atualiza (para melodias assíncronas)
  void update();

  // Controle do microfone
  bool startRecording();
  void stopRecording();
  bool isRecording() const { return _recording; }

  // Captura de áudio para voice assistant
  size_t readAudioSamples(int16_t *buffer, size_t maxSamples);
  bool isAudioAvailable() const;

  // Configuração de ganho do microfone
  void setMicGain(int gain); // 0-100

  // Ativa/desativa Power Amplifier
  void enablePA(bool enable);

private:
  void playNextNote();
  void generateToneBuffer(int frequency, int duration);

  bool _initialized;
  bool _muted;
  bool _playing;
  bool _recording;
  int _volume;

  // Para melodias assíncronas
  const MelodyNote *_currentMelody;
  int _melodyLength;
  int _melodyIndex;
  unsigned long _noteStartTime;
  int _currentNoteDuration;

  // Handle do ES8311
  void *_es8311Handle;
};

// Instância global
extern AudioDriver audioDriver;
