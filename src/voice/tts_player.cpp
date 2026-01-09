/**
 * @file tts_player.cpp
 * @brief Implementação do player TTS com samples WAV
 */

#include "tts_player.h"
#include "../hardware/audio_driver.h"
#include <SD_MMC.h>
#include <driver/i2s.h>

// Instância global
TTSPlayer ttsPlayer;

// Mapeamento de samples para nomes de arquivo
static const char *sampleFilenames[] = {
    "hello.wav",          // TTS_SAMPLE_HELLO
    "goodbye.wav",        // TTS_SAMPLE_GOODBYE
    "listening.wav",      // TTS_SAMPLE_LISTENING
    "scanning.wav",       // TTS_SAMPLE_SCANNING
    "scan_done.wav",      // TTS_SAMPLE_SCAN_DONE
    "attack_start.wav",   // TTS_SAMPLE_ATTACK_START
    "attack_stop.wav",    // TTS_SAMPLE_ATTACK_STOP
    "handshake.wav",      // TTS_SAMPLE_HANDSHAKE
    "alert.wav",          // TTS_SAMPLE_ALERT
    "status.wav",         // TTS_SAMPLE_STATUS
    "ok.wav",             // TTS_SAMPLE_OK
    "error.wav",          // TTS_SAMPLE_ERROR
    "not_understood.wav", // TTS_SAMPLE_NOT_UNDERSTOOD
    "ble_start.wav",      // TTS_SAMPLE_BLE_START
    "wake.wav"            // TTS_SAMPLE_WAKE
};

// Melodias de fallback para cada sample
static const MelodyNote fallback_hello[] = {
    {NOTE_C5, 100}, {NOTE_E5, 100}, {NOTE_G4, 150}};
static const MelodyNote fallback_goodbye[] = {
    {NOTE_G4, 150}, {NOTE_E4, 150}, {NOTE_C4, 200}};
static const MelodyNote fallback_listening[] = {{NOTE_E5, 100}, {NOTE_G4, 100}};
static const MelodyNote fallback_scanning[] = {{NOTE_D5, 100}, {NOTE_E5, 100}};
static const MelodyNote fallback_success[] = {
    {NOTE_C5, 100}, {NOTE_E5, 100}, {NOTE_G4, 200}};
static const MelodyNote fallback_error[] = {{NOTE_E4, 200}, {NOTE_C4, 200}};
static const MelodyNote fallback_alert[] = {
    {NOTE_A4, 100}, {NOTE_A4, 100}, {NOTE_A4, 100}};
static const MelodyNote fallback_beep[] = {{NOTE_C5, 100}};

TTSPlayer::TTSPlayer()
    : _initialized(false), _playing(false), _fallbackMode(true),
      _samplesAvailable(false), _bytesRemaining(0) {}

bool TTSPlayer::begin() {
  Serial.println("[TTS] Inicializando player TTS...");

  // Verifica se SD está montado
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("[TTS] SD card não disponível, usando modo fallback");
    _fallbackMode = true;
    _samplesAvailable = false;
  } else {
    // Verifica se samples existem
    _samplesAvailable = checkSamplesAvailable();
    _fallbackMode = !_samplesAvailable;

    if (_samplesAvailable) {
      Serial.println("[TTS] Samples de voz encontrados!");
      listAvailableSamples();
    } else {
      Serial.println("[TTS] Samples não encontrados, usando modo fallback");
    }
  }

  _initialized = true;
  Serial.printf("[TTS] Player inicializado (modo: %s)\n",
                _fallbackMode ? "fallback/tons" : "samples WAV");

  return true;
}

bool TTSPlayer::play(TTSSample sample) {
  if (!_initialized)
    return false;

  if (sample >= TTS_SAMPLE_COUNT)
    return false;

  stop(); // Para reprodução anterior

  if (_fallbackMode || !_samplesAvailable) {
    playFallbackTone(sample);
    return true;
  }

  // Tenta reproduzir arquivo WAV
  String path = String(TTS_SAMPLES_DIR) + "/" + getSampleFilename(sample);
  return playFile(path.c_str());
}

bool TTSPlayer::playFile(const char *path) {
  if (!_initialized)
    return false;

  Serial.printf("[TTS] Reproduzindo: %s\n", path);

  _currentFile = SD_MMC.open(path, FILE_READ);
  if (!_currentFile) {
    Serial.printf("[TTS] Erro ao abrir arquivo: %s\n", path);
    return false;
  }

  // Carrega header WAV
  if (!loadWavHeader(_currentFile, _currentHeader)) {
    Serial.println("[TTS] Erro ao ler header WAV");
    _currentFile.close();
    return false;
  }

  // Valida formato
  if (_currentHeader.audioFormat != 1 || // PCM
      _currentHeader.bitsPerSample != 16) {
    Serial.println("[TTS] Formato WAV não suportado (precisa PCM 16-bit)");
    _currentFile.close();
    return false;
  }

  Serial.printf("[TTS] WAV: %dHz, %d canais, %d bits\n",
                _currentHeader.sampleRate, _currentHeader.numChannels,
                _currentHeader.bitsPerSample);

  _bytesRemaining = _currentHeader.dataSize;
  _playing = true;

  return true;
}

void TTSPlayer::update() {
  if (!_playing || !_currentFile)
    return;

  // Streaming de áudio
  size_t bytesToRead =
      min(_bytesRemaining, (size_t)(STREAM_BUFFER_SIZE * sizeof(int16_t)));

  if (bytesToRead == 0) {
    stop();
    return;
  }

  size_t bytesRead = _currentFile.read((uint8_t *)_streamBuffer, bytesToRead);

  if (bytesRead > 0) {
    size_t bytesWritten = 0;

    // Escreve no I2S
    i2s_write(I2S_NUM_0, _streamBuffer, bytesRead, &bytesWritten,
              portMAX_DELAY);

    _bytesRemaining -= bytesRead;
  }

  if (_bytesRemaining == 0 || bytesRead == 0) {
    stop();
  }
}

void TTSPlayer::stop() {
  if (_currentFile) {
    _currentFile.close();
  }
  _playing = false;
  _bytesRemaining = 0;
}

bool TTSPlayer::loadWavHeader(File &file, WavHeader &header) {
  // Lê header RIFF
  if (file.read((uint8_t *)&header, 44) != 44) {
    return false;
  }

  // Valida assinatura RIFF/WAVE
  if (strncmp(header.riff, "RIFF", 4) != 0 ||
      strncmp(header.wave, "WAVE", 4) != 0) {
    return false;
  }

  return true;
}

bool TTSPlayer::checkSamplesAvailable() {
  String dirPath = TTS_SAMPLES_DIR;

  File dir = SD_MMC.open(dirPath);
  if (!dir || !dir.isDirectory()) {
    return false;
  }

  // Verifica se pelo menos alguns samples existem
  int foundCount = 0;
  for (int i = 0; i < TTS_SAMPLE_COUNT && foundCount < 3; i++) {
    String path = dirPath + "/" + sampleFilenames[i];
    if (SD_MMC.exists(path)) {
      foundCount++;
    }
  }

  dir.close();
  return foundCount > 0;
}

void TTSPlayer::listAvailableSamples() {
  String dirPath = TTS_SAMPLES_DIR;

  Serial.println("[TTS] Samples disponíveis:");
  for (int i = 0; i < TTS_SAMPLE_COUNT; i++) {
    String path = dirPath + "/" + sampleFilenames[i];
    bool exists = SD_MMC.exists(path);
    Serial.printf("  [%c] %s\n", exists ? 'x' : ' ', sampleFilenames[i]);
  }
}

const char *TTSPlayer::getSampleFilename(TTSSample sample) {
  if (sample < TTS_SAMPLE_COUNT) {
    return sampleFilenames[sample];
  }
  return "unknown.wav";
}

void TTSPlayer::playFallbackTone(TTSSample sample) {
  // Usa tons musicais como fallback quando samples não disponíveis
  switch (sample) {
  case TTS_SAMPLE_HELLO:
    audioDriver.playMelody(fallback_hello, 3);
    Serial.println("[TTS] 'Olá, sou o LeleWatch'");
    break;

  case TTS_SAMPLE_GOODBYE:
    audioDriver.playMelody(fallback_goodbye, 3);
    Serial.println("[TTS] 'Até logo'");
    break;

  case TTS_SAMPLE_LISTENING:
    audioDriver.playMelody(fallback_listening, 2);
    Serial.println("[TTS] 'Estou ouvindo'");
    break;

  case TTS_SAMPLE_SCANNING:
    audioDriver.playMelody(fallback_scanning, 2);
    Serial.println("[TTS] 'Escaneando redes'");
    break;

  case TTS_SAMPLE_SCAN_DONE:
  case TTS_SAMPLE_OK:
    audioDriver.playMelody(fallback_success, 3);
    Serial.println("[TTS] 'Ok'");
    break;

  case TTS_SAMPLE_ATTACK_START:
  case TTS_SAMPLE_BLE_START:
    audioDriver.playMelody(fallback_success, 3);
    Serial.println("[TTS] 'Ataque iniciado'");
    break;

  case TTS_SAMPLE_ATTACK_STOP:
    audioDriver.playMelody(fallback_success, 3);
    Serial.println("[TTS] 'Ataque parado'");
    break;

  case TTS_SAMPLE_HANDSHAKE:
    audioDriver.playMelody(fallback_success, 3);
    audioDriver.playMelody(fallback_success, 3);
    Serial.println("[TTS] 'Handshake capturado!'");
    break;

  case TTS_SAMPLE_ALERT:
    audioDriver.playMelody(fallback_alert, 3);
    Serial.println("[TTS] 'Alerta!'");
    break;

  case TTS_SAMPLE_STATUS:
    audioDriver.playMelody(fallback_beep, 1);
    Serial.println("[TTS] 'Status'");
    break;

  case TTS_SAMPLE_ERROR:
  case TTS_SAMPLE_NOT_UNDERSTOOD:
    audioDriver.playMelody(fallback_error, 2);
    Serial.println("[TTS] 'Erro/Não entendi'");
    break;

  case TTS_SAMPLE_WAKE:
    audioDriver.playMelody(fallback_listening, 2);
    break;

  default:
    audioDriver.playMelody(fallback_beep, 1);
    break;
  }
}
