#pragma once
#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>

class VoiceRecorder {
public:
  VoiceRecorder();

  void begin();
  void update();

  void startRecording(const char *filename = nullptr);
  void stopRecording();

  bool isRecording() const { return _isRecording; }
  void setVAD(bool enable) { _vadEnabled = enable; }
  void setVADThreshold(int threshold) { _vadThreshold = threshold; }

  uint32_t getRecordingDuration() const;

private:
  bool _isRecording;
  bool _vadEnabled;
  int _vadThreshold;
  File _file;
  uint32_t _startTime;
  uint32_t _lastAudioTime;

  static const int SAMPLE_RATE = 16000;
  static const int BUFFER_SIZE = 512;
  int16_t _buffer[BUFFER_SIZE];

  void writeWavHeader(File &file, int sampleRate);
  void updateWavHeader(File &file);
  bool checkVAD(int16_t *samples, int count);
};

extern VoiceRecorder voiceRecorder;
