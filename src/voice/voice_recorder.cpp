#include "voice_recorder.h"
#include "../hardware/audio_driver.h"
#include "Arduino.h"

VoiceRecorder voiceRecorder;

// WAV Header Constants
struct WavHeader {
  char riff[4];
  uint32_t fileSize;
  char wave[4];
  char fmt[4];
  uint32_t fmtLen;
  uint16_t type;
  uint16_t channels;
  uint32_t sampleRate;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample;
  char data[4];
  uint32_t dataSize;
};

VoiceRecorder::VoiceRecorder()
    : _isRecording(false), _vadEnabled(true),
      _vadThreshold(300), // Default threshold (adjust based on noise env)
      _startTime(0), _lastAudioTime(0) {}

void VoiceRecorder::begin() {
  // AudioDriver should be initialized by main
  // Ensure SD is ready
  if (!SD_MMC.exists("/recordings")) {
    SD_MMC.mkdir("/recordings");
  }
}

void VoiceRecorder::update() {
  // If VAD enabled and not recording, check level
  // Note: This requires constant listening, which might conflict with other
  // audio tasks. For now, we assume this is called when "Audio Recorder" app is
  // active or background service.

  // Read samples from mic (if not exclusively claimed)
  // Actually, we need to poll audio samples.

  if (_isRecording) {
    size_t bytesRead = 0;
    // This is a blocking read in most I2S implementations, careful in main
    // loop! We should move this to a task or use non-blocking available check.
    // For simplicity in this iteration, we read small chunks.

    int samplesRead = audioDriver.readAudioSamples(_buffer, BUFFER_SIZE);

    if (samplesRead > 0) {
      // Write to SD
      if (_file) {
        _file.write((uint8_t *)_buffer, samplesRead * 2);
      }

      // Check VAD to stop if silent for too long
      if (_vadEnabled) {
        if (checkVAD(_buffer, samplesRead)) {
          _lastAudioTime = millis();
        } else {
          if (millis() - _lastAudioTime > 5000) { // 5s silence
            Serial.println("[REC] Silence detected, stopping...");
            stopRecording();
          }
        }
      }
    }
  } else if (_vadEnabled) {
    // Monitor for trigger?
    // This usually requires a loop.
    // Let's implement manual start or UI trigger for now to avoid complexity
    // sharing I2S.
  }
}

void VoiceRecorder::startRecording(const char *filename) {
  if (_isRecording)
    stopRecording();

  char path[32];
  if (filename) {
    snprintf(path, sizeof(path), "/recordings/%s.wav", filename);
  } else {
    // Auto name timestamp
    snprintf(path, sizeof(path), "/recordings/rec_%u.wav", millis());
  }

  Serial.printf("[REC] Starting recording: %s\n", path);

  _file = SD_MMC.open(path, FILE_WRITE);
  if (!_file) {
    Serial.println("[REC] Failed to open file for writing");
    return;
  }

  writeWavHeader(_file, SAMPLE_RATE);

  audioDriver.startRecording(); // Ensure mic is on
  _isRecording = true;
  _startTime = millis();
  _lastAudioTime = millis();
}

void VoiceRecorder::stopRecording() {
  if (!_isRecording)
    return;

  Serial.println("[REC] Stopping recording");

  if (_file) {
    updateWavHeader(_file);
    _file.close();
    Serial.println("[REC] Saved.");
  }

  _isRecording = false;
  // audioDriver.enableMicrophone(false); // Maybe keep on if needed elsewhere
}

uint32_t VoiceRecorder::getRecordingDuration() const {
  if (!_isRecording)
    return 0;
  return (millis() - _startTime) / 1000;
}

bool VoiceRecorder::checkVAD(int16_t *samples, int count) {
  int32_t sum = 0;
  for (int i = 0; i < count; i++) {
    sum += abs(samples[i]);
  }
  int average = sum / count;
  return (average > _vadThreshold);
}

void VoiceRecorder::writeWavHeader(File &file, int sampleRate) {
  WavHeader h;
  memcpy(h.riff, "RIFF", 4);
  h.fileSize = 0; // Placeholder
  memcpy(h.wave, "WAVE", 4);
  memcpy(h.fmt, "fmt ", 4);
  h.fmtLen = 16;
  h.type = 1;     // PCM
  h.channels = 1; // Mono
  h.sampleRate = sampleRate;
  h.bitsPerSample = 16;
  h.byteRate = sampleRate * 1 * (16 / 8);
  h.blockAlign = 1 * (16 / 8);
  memcpy(h.data, "data", 4);
  h.dataSize = 0; // Placeholder

  file.write((uint8_t *)&h, sizeof(WavHeader));
}

void VoiceRecorder::updateWavHeader(File &file) {
  if (!file)
    return;

  uint32_t dataSize = file.size() - sizeof(WavHeader);
  uint32_t fileSize = file.size() - 8;

  file.seek(4);
  file.write((uint8_t *)&fileSize, 4);

  file.seek(40);
  file.write((uint8_t *)&dataSize, 4);

  file.seek(file.size()); // Return to end (good practice)
}
