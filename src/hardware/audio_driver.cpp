/**
 * @file audio_driver.cpp
 * @brief Implementação do driver de áudio usando ES8311 + I2S
 *
 * Usa o codec ES8311 do Waveshare ESP32-S3-Touch-AMOLED-1.8
 * para reprodução de áudio via speaker interno e captura via microfone.
 */

#include "audio_driver.h"
#include "es8311.h"
#include <Wire.h>
#include <driver/i2s.h>
#include <math.h>

// Instância global
AudioDriver audioDriver;

// Configurações de áudio
#define AUDIO_SAMPLE_RATE 16000
#define AUDIO_BIT_DEPTH 16
#define I2S_PORT I2S_NUM_0

// === Melodias predefinidas ===
static const MelodyNote melody_boot[] = {{NOTE_C4, 100},  {NOTE_E4, 100},
                                         {NOTE_G4, 100},  {NOTE_C5, 200},
                                         {NOTE_REST, 50}, {NOTE_E5, 300}};

static const MelodyNote melody_success[] = {
    {NOTE_C5, 100}, {NOTE_E5, 100}, {NOTE_G4, 200}};

static const MelodyNote melody_error[] = {
    {NOTE_E4, 200}, {NOTE_REST, 50}, {NOTE_C4, 300}};

static const MelodyNote melody_alert[] = {{NOTE_A4, 100},
                                          {NOTE_REST, 50},
                                          {NOTE_A4, 100},
                                          {NOTE_REST, 50},
                                          {NOTE_A4, 100}};

static const MelodyNote melody_scan_start[] = {{NOTE_C4, 50}, {NOTE_G4, 100}};
static const MelodyNote melody_scan_found[] = {{NOTE_E5, 50}, {NOTE_G4, 50}};
static const MelodyNote melody_attack_start[] = {
    {NOTE_C4, 100}, {NOTE_C4, 100}, {NOTE_G4, 200}};
static const MelodyNote melody_attack_complete[] = {
    {NOTE_G4, 100}, {NOTE_C5, 100}, {NOTE_E5, 100}, {NOTE_G4, 300}};
static const MelodyNote melody_handshake[] = {{NOTE_C5, 100}, {NOTE_E5, 100},
                                              {NOTE_G4, 100}, {NOTE_REST, 50},
                                              {NOTE_C5, 100}, {NOTE_E5, 200}};
static const MelodyNote melody_click[] = {{NOTE_C5, 20}};
static const MelodyNote melody_beep[] = {{NOTE_A4, 100}};

// Buffer de áudio
#define TONE_BUFFER_SIZE 1024
static int16_t toneBuffer[TONE_BUFFER_SIZE];
static bool i2s_installed = false;

// === Implementação ===

AudioDriver::AudioDriver()
    : _initialized(false), _muted(false), _playing(false), _recording(false),
      _volume(80), _currentMelody(nullptr), _melodyLength(0), _melodyIndex(0),
      _noteStartTime(0), _currentNoteDuration(0), _es8311Handle(nullptr) {}

bool AudioDriver::begin() {
  Serial.println("[AUDIO] Inicializando ES8311 + I2S...");

  // 1. Ativar Power Amplifier
  pinMode(AUDIO_PA, OUTPUT);
  digitalWrite(AUDIO_PA, HIGH);
  Serial.printf("[AUDIO] PA ativado (GPIO %d)\n", AUDIO_PA);

  // 2. Configurar I2S
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
      .sample_rate = AUDIO_SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = 256,
      .use_apll = false,
      .tx_desc_auto_clear = true,
      .fixed_mclk = 0};

  i2s_pin_config_t pin_config = {.mck_io_num = I2S_MCK_IO,
                                 .bck_io_num = I2S_BCK_IO,
                                 .ws_io_num = I2S_WS_IO,
                                 .data_out_num = I2S_DO_IO,
                                 .data_in_num = I2S_DI_IO};

  esp_err_t ret = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (ret != ESP_OK) {
    Serial.printf("[AUDIO] ERRO: Falha ao instalar driver I2S (0x%x)\n", ret);
    return false;
  }

  ret = i2s_set_pin(I2S_PORT, &pin_config);
  if (ret != ESP_OK) {
    Serial.printf("[AUDIO] ERRO: Falha ao configurar pinos I2S (0x%x)\n", ret);
    i2s_driver_uninstall(I2S_PORT);
    return false;
  }

  i2s_installed = true;
  Serial.println("[AUDIO] I2S inicializado");

  // 3. Inicializar ES8311
  _es8311Handle = es8311_create(0, ES8311_ADDR);
  if (!_es8311Handle) {
    Serial.println("[AUDIO] ERRO: Falha ao criar handle ES8311!");
    return false;
  }

  es8311_clock_config_t es_clk = {.mclk_inverted = false,
                                  .sclk_inverted = false,
                                  .mclk_from_mclk_pin = true,
                                  .mclk_frequency = AUDIO_SAMPLE_RATE * 256,
                                  .sample_frequency = AUDIO_SAMPLE_RATE};

  ret = es8311_init((es8311_handle_t)_es8311Handle, &es_clk,
                    ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
  if (ret != ESP_OK) {
    Serial.printf("[AUDIO] ERRO: Falha ao inicializar ES8311 (0x%x)\n", ret);
    return false;
  }

  es8311_voice_volume_set((es8311_handle_t)_es8311Handle, _volume, NULL);

  _initialized = true;
  Serial.printf("[AUDIO] ES8311 inicializado! Volume: %d%%\n", _volume);

  return true;
}

void AudioDriver::end() {
  if (!_initialized)
    return;

  stop();
  enablePA(false);

  if (_es8311Handle) {
    es8311_delete((es8311_handle_t)_es8311Handle);
    _es8311Handle = nullptr;
  }

  if (i2s_installed) {
    i2s_driver_uninstall(I2S_PORT);
    i2s_installed = false;
  }

  _initialized = false;
  Serial.println("[AUDIO] Driver encerrado");
}

void AudioDriver::enablePA(bool enable) {
  digitalWrite(AUDIO_PA, enable ? HIGH : LOW);
}

void AudioDriver::playSound(SoundType type) {
  if (!_initialized || _muted)
    return;

  switch (type) {
  case SOUND_BOOT:
    playMelody(melody_boot, sizeof(melody_boot) / sizeof(MelodyNote));
    break;
  case SOUND_SUCCESS:
    playMelody(melody_success, sizeof(melody_success) / sizeof(MelodyNote));
    break;
  case SOUND_ERROR:
    playMelody(melody_error, sizeof(melody_error) / sizeof(MelodyNote));
    break;
  case SOUND_ALERT:
    playMelody(melody_alert, sizeof(melody_alert) / sizeof(MelodyNote));
    break;
  case SOUND_SCAN_START:
    playMelody(melody_scan_start,
               sizeof(melody_scan_start) / sizeof(MelodyNote));
    break;
  case SOUND_SCAN_FOUND:
    playMelody(melody_scan_found,
               sizeof(melody_scan_found) / sizeof(MelodyNote));
    break;
  case SOUND_ATTACK_START:
    playMelody(melody_attack_start,
               sizeof(melody_attack_start) / sizeof(MelodyNote));
    break;
  case SOUND_ATTACK_COMPLETE:
    playMelody(melody_attack_complete,
               sizeof(melody_attack_complete) / sizeof(MelodyNote));
    break;
  case SOUND_HANDSHAKE:
    playMelody(melody_handshake, sizeof(melody_handshake) / sizeof(MelodyNote));
    break;
  case SOUND_CLICK:
    playMelody(melody_click, sizeof(melody_click) / sizeof(MelodyNote));
    break;
  case SOUND_BEEP:
    playMelody(melody_beep, sizeof(melody_beep) / sizeof(MelodyNote));
    break;
  }
}

void AudioDriver::generateToneBuffer(int frequency, int duration) {
  if (frequency == 0) {
    memset(toneBuffer, 0, sizeof(toneBuffer));
    return;
  }

  float samplesPerCycle = (float)AUDIO_SAMPLE_RATE / frequency;
  int numSamples = min((int)(AUDIO_SAMPLE_RATE * duration / 1000),
                       (int)(TONE_BUFFER_SIZE / 2));
  int16_t amplitude = (int16_t)(32767 * _volume / 100);

  for (int i = 0; i < numSamples; i++) {
    float phase = (float)i / samplesPerCycle;
    int16_t sample = (int16_t)(amplitude * sin(2.0 * PI * phase));
    toneBuffer[i * 2] = sample;     // Left
    toneBuffer[i * 2 + 1] = sample; // Right
  }
}

void AudioDriver::playTone(int frequency, int duration) {
  if (!_initialized || _muted || !i2s_installed)
    return;

  generateToneBuffer(frequency, duration);

  int numSamples = min((int)(AUDIO_SAMPLE_RATE * duration / 1000),
                       (int)(TONE_BUFFER_SIZE / 2));
  size_t bytesToWrite = numSamples * 2 * sizeof(int16_t);
  size_t bytesWritten = 0;

  i2s_write(I2S_PORT, toneBuffer, bytesToWrite, &bytesWritten, portMAX_DELAY);
}

void AudioDriver::playRaw(const int16_t *samples, size_t count) {
  if (!_initialized || _muted || !i2s_installed || !samples || count == 0)
    return;

  size_t bytesToWrite = count * sizeof(int16_t);
  size_t bytesWritten = 0;

  // Em modo stereo (I2S_CHANNEL_FMT_RIGHT_LEFT), precisamos duplicar se o input
  // for mono Mas aqui assumimos que playRaw já recebe o formato correto ou que
  // o I2S lida com isso. Se o buffer for mono e I2S stereo, teríamos que
  // expandir. O Talkie gera 8khz. Se I2S está em 16khz, precisamos de
  // upsampling. Assumindo que quem chama playRaw já tratou isso
  // (VoiceAssistant).

  i2s_write(I2S_PORT, samples, bytesToWrite, &bytesWritten, portMAX_DELAY);
}

void AudioDriver::playMelody(const MelodyNote *melody, int length) {
  if (!_initialized || _muted)
    return;

  _currentMelody = melody;
  _melodyLength = length;
  _melodyIndex = 0;
  _playing = true;

  playNextNote();
}

void AudioDriver::playNextNote() {
  if (_melodyIndex >= _melodyLength) {
    stop();
    return;
  }

  const MelodyNote &note = _currentMelody[_melodyIndex];
  playTone(note.frequency, note.duration);

  _noteStartTime = millis();
  _currentNoteDuration = note.duration;
  _melodyIndex++;
}

void AudioDriver::update() {
  if (!_playing || _currentMelody == nullptr)
    return;

  unsigned long now = millis();
  if (now - _noteStartTime >= (unsigned long)_currentNoteDuration) {
    playNextNote();
  }
}

void AudioDriver::stop() {
  _playing = false;
  _currentMelody = nullptr;
  _melodyIndex = 0;
}

void AudioDriver::setVolume(int volume) {
  _volume = constrain(volume, 0, 100);
  if (_initialized && _es8311Handle) {
    es8311_voice_volume_set((es8311_handle_t)_es8311Handle, _volume, NULL);
  }
}

void AudioDriver::setMuted(bool muted) {
  _muted = muted;
  if (_initialized && _es8311Handle) {
    es8311_voice_mute((es8311_handle_t)_es8311Handle, muted);
  }
  enablePA(!muted);
}

bool AudioDriver::startRecording() {
  if (!_initialized)
    return false;
  _recording = true;
  Serial.println("[AUDIO] Gravação iniciada - Microfone ativo");
  return true;
}

void AudioDriver::stopRecording() {
  _recording = false;
  Serial.println("[AUDIO] Gravação parada");
}

size_t AudioDriver::readAudioSamples(int16_t *buffer, size_t maxSamples) {
  if (!_initialized || !_recording || !buffer || maxSamples == 0) {
    return 0;
  }

  size_t bytesToRead = maxSamples * sizeof(int16_t);
  size_t bytesRead = 0;

  esp_err_t ret = i2s_read(I2S_PORT, buffer, bytesToRead, &bytesRead,
                           10 / portTICK_PERIOD_MS);

  if (ret != ESP_OK) {
    return 0;
  }

  return bytesRead / sizeof(int16_t);
}

bool AudioDriver::isAudioAvailable() const {
  return _initialized && _recording && i2s_installed;
}

void AudioDriver::setMicGain(int gain) {
  if (!_initialized || !_es8311Handle)
    return;

  // ES8311 mic gain: 0-100 mapeado para configuração do codec
  int micGain = constrain(gain, 0, 100);

  // Configura ganho do ADC do microfone
  // ES8311 suporta ganhos de 0dB a +24dB
  float dbGain = (float)micGain / 100.0f * 24.0f;

  Serial.printf("[AUDIO] Ganho do microfone: %d%% (%.1f dB)\n", micGain,
                dbGain);

  // Nota: A API do ES8311 pode variar - ajustar conforme necessário
  // es8311_microphone_gain_set((es8311_handle_t)_es8311Handle, dbGain);
}
