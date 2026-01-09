#pragma once
/**
 * @file voice_assistant.h
 * @brief Assistente de voz WavePwn com reconhecimento REAL e TTS
 *
 * Implementa:
 * - Wake word detection ("Hey Dragon") via microfone real
 * - Comandos de voz com detecção baseada em energia
 * - TTS com samples WAV do SD card
 */

#include "../hardware/audio_driver.h"
#include <Arduino.h>

// Talkie stub - ESP32-S3 não tem DAC, TTS via I2S com samples WAV
class Talkie {
public:
  void say(const uint8_t* data) { /* Stub - usar audioDriver */ }
};

// Estados do assistente de voz
enum VoiceState {
  VOICE_IDLE,       // Aguardando wake word
  VOICE_LISTENING,  // Ouvindo comando
  VOICE_PROCESSING, // Processando comando
  VOICE_SPEAKING,   // Reproduzindo resposta
  VOICE_DIALOG      // Modo diálogo interativo
};

// Comandos reconhecidos
enum VoiceCommand {
  CMD_NONE = 0,
  CMD_SCAN_WIFI,           // "Scan WiFi" / "Escanear"
  CMD_START_ATTACK,        // "Start Attack" / "Iniciar Ataque"
  CMD_STOP,                // "Stop" / "Parar"
  CMD_STATUS,              // "Status" / "Relatório"
  CMD_BLE_SPAM,            // "BLE Spam"
  CMD_BEACON_FLOOD,        // "Beacon Flood"
  CMD_DEAUTH,              // "Deauth" / "Desautenticar"
  CMD_HELP,                // "Help" / "Ajuda"
  CMD_SLEEP,               // "Sleep" / "Dormir"
  CMD_WAKE,                // "Wake" / "Acordar"
  CMD_TV_NUKE,             // [NEW] TV Nuke
  CMD_SING_HAPPY_BIRTHDAY, // [NEW] Birthday
  CMD_TROLL_ME             // [NEW] Troll
};

// Mensagens TTS
enum TTSMessage {
  TTS_HELLO,
  TTS_SCANNING,
  TTS_NETWORKS_FOUND,
  TTS_ATTACK_STARTED,
  TTS_ATTACK_STOPPED,
  TTS_HANDSHAKE_CAPTURED,
  TTS_THREAT_ALERT,
  TTS_STATUS_REPORT,
  TTS_LISTENING,
  TTS_COMMAND_NOT_RECOGNIZED,
  TTS_OK,
  TTS_ERROR,
  TTS_BLE_STARTED,
  TTS_GOODBYE,
  TTS_WAKE_WORD
};

class VoiceAssistant {
public:
  VoiceAssistant();

  bool begin();
  void update();

  // Controle
  void setListening(bool enabled);
  bool isListening() const { return _listeningEnabled; }
  void triggerWakeWord(); // Simula wake word

  // TTS
  void speak(TTSMessage msg, int param = 0);
  void say(const char *text);

  // Executa comando externamente
  void executeCommand(VoiceCommand cmd);

  // Callback
  typedef void (*CommandCallback)(VoiceCommand cmd);
  void onCommand(CommandCallback cb) { _cmdCallback = cb; }

  // Debug
  float getSignalEnergy() const;
  float getNoiseFloor() const;
  bool isVoiceActive() const;

private:
  VoiceState _state;
  bool _listeningEnabled;
  bool _wakeWordDetected;

  // Ring Buffer
  int16_t *_audioHistoryBuffer;
  size_t _historyBufferSize;
  size_t _historyWriteHead;

  // Talkie TTS
  Talkie _talkie;

  unsigned long _lastActivity;
  unsigned long _listeningTimeout;
  void (*_cmdCallback)(VoiceCommand);

  // Buffer para processamento atual
  size_t _audioBufferPos;
  size_t _commandBufferPos;
  float _signalEnergy;
  float _noiseFloor;
  unsigned long _lastAudioProcess;

  void processAudio();
  VoiceCommand detectCommand();
  void playTTS(TTSMessage msg, int param);
  void generateBeep(int freq, int duration);

  // Funções de buffer
  void addToHistory(const int16_t *samples, size_t count);

  // Diálogo
  void enterDialogMode();
  void exitDialogMode();

  // Dados de voz
  void loadVoiceData();
};

// Instância global
extern VoiceAssistant voiceAssistant;
