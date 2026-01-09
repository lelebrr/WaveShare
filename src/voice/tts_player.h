#pragma once
/**
 * @file tts_player.h
 * @brief Player de TTS (Text-to-Speech) usando samples WAV
 *
 * Reproduz mensagens de voz pré-gravadas do SD card via I2S
 */

#include "../hardware/audio_driver.h"
#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>

// Diretório dos samples TTS no SD card
#define TTS_SAMPLES_DIR "/sounds/tts"

// Mensagens TTS disponíveis
enum TTSSample {
  TTS_SAMPLE_HELLO,          // "Olá, sou o LeleWatch"
  TTS_SAMPLE_GOODBYE,        // "Até logo"
  TTS_SAMPLE_LISTENING,      // "Estou ouvindo"
  TTS_SAMPLE_SCANNING,       // "Escaneando redes"
  TTS_SAMPLE_SCAN_DONE,      // "Escaneamento concluído"
  TTS_SAMPLE_ATTACK_START,   // "Ataque iniciado"
  TTS_SAMPLE_ATTACK_STOP,    // "Ataque parado"
  TTS_SAMPLE_HANDSHAKE,      // "Handshake capturado"
  TTS_SAMPLE_ALERT,          // "Alerta de ameaça"
  TTS_SAMPLE_STATUS,         // "Relatório de status"
  TTS_SAMPLE_OK,             // "Ok"
  TTS_SAMPLE_ERROR,          // "Erro"
  TTS_SAMPLE_NOT_UNDERSTOOD, // "Não entendi"
  TTS_SAMPLE_BLE_START,      // "BLE spam ativado"
  TTS_SAMPLE_WAKE,           // Som de wake word detectado
  TTS_SAMPLE_COUNT
};

// Estrutura de header WAV
struct WavHeader {
  char riff[4];           // "RIFF"
  uint32_t fileSize;      // Tamanho total - 8
  char wave[4];           // "WAVE"
  char fmt[4];            // "fmt "
  uint32_t fmtSize;       // 16 para PCM
  uint16_t audioFormat;   // 1 para PCM
  uint16_t numChannels;   // 1 ou 2
  uint32_t sampleRate;    // Ex: 16000
  uint32_t byteRate;      // sampleRate * numChannels * bitsPerSample/8
  uint16_t blockAlign;    // numChannels * bitsPerSample/8
  uint16_t bitsPerSample; // 8, 16, etc
  char data[4];           // "data"
  uint32_t dataSize;      // Tamanho dos dados de áudio
};

class TTSPlayer {
public:
  TTSPlayer();

  // Inicializa o player
  bool begin();

  // Reproduz um sample TTS
  bool play(TTSSample sample);

  // Reproduz arquivo WAV do SD card
  bool playFile(const char *path);

  // Para reprodução atual
  void stop();

  // Verifica se está reproduzindo
  bool isPlaying() const { return _playing; }

  // Atualiza (chamar no loop para streaming)
  void update();

  // Verifica se samples existem no SD
  bool checkSamplesAvailable();

  // Lista arquivos de samples disponíveis
  void listAvailableSamples();

  // Usa fallback (tons) se samples não disponíveis
  void setFallbackMode(bool enabled) { _fallbackMode = enabled; }
  bool isFallbackMode() const { return _fallbackMode; }

private:
  bool loadWavHeader(File &file, WavHeader &header);
  void playFallbackTone(TTSSample sample);
  const char *getSampleFilename(TTSSample sample);

  bool _initialized;
  bool _playing;
  bool _fallbackMode;
  bool _samplesAvailable;

  File _currentFile;
  WavHeader _currentHeader;

  // Buffer de streaming
  static const size_t STREAM_BUFFER_SIZE = 2048;
  int16_t _streamBuffer[STREAM_BUFFER_SIZE];
  size_t _bytesRemaining;
};

// Instância global
extern TTSPlayer ttsPlayer;
