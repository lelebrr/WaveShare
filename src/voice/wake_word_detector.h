#pragma once
/**
 * @file wake_word_detector.h
 * @brief Neural Wake Word Detector using TFLite Micro
 */

#include <Arduino.h>

class WakeWordDetector {
public:
    WakeWordDetector();
    ~WakeWordDetector();
    
    // Initialize TFLite model
    bool begin();
    
    // Process audio frame and check for wake word
    // Returns true if wake word detected
    bool processAudioFrame(const int16_t* audioData, size_t sampleCount);
    
    // Get confidence of last detection
    float getLastConfidence() const { return _lastConfidence; }
    
    // Enable/disable detection
    void setEnabled(bool enabled) { _enabled = enabled; }
    bool isEnabled() const { return _enabled; }

private:
    bool _initialized;
    bool _enabled;
    float _lastConfidence;
    
    // MFCC feature buffer
    float* _mfccBuffer;
    int _mfccBufferPos;
    
    // TFLite components (forward declared)
    void* _interpreter; // Actually tflite::MicroInterpreter*
    uint8_t* _tensorArena;
    
    // Internal methods
    void extractMFCC(const int16_t* audio, size_t len, float* mfccOut);
    float runInference();
};

extern WakeWordDetector wakeWordDetector;
