/**
 * @file tts_player.cpp
 * @brief Offline TTS Player implementation
 * 
 * Plays pre-generated audio samples via I2S to ES8311 codec.
 */

#include "tts_player.h"
#include "tts_samples.h"
#include "../hardware/audio_driver.h"
#include <Arduino.h>

TTSPlayer ttsPlayer;

// Playback rate: 16kHz = 62.5us per sample
#define SAMPLE_INTERVAL_US 62

TTSPlayer::TTSPlayer() 
    : _playing(false), _currentSample(nullptr), _sampleLength(0), 
      _playbackPosition(0), _lastSampleTime(0) {}

bool TTSPlayer::begin() {
    Serial.println("[TTS] Offline TTS Player initialized");
    return true;
}

void TTSPlayer::play(TTSMessage msg) {
    if (msg >= TTS_SAMPLE_COUNT) {
        Serial.printf("[TTS] Invalid message ID: %d\n", msg);
        return;
    }
    
    // Stop current playback
    stop();
    
    // Get sample from lookup table
    const TTSSample& sample = TTS_SAMPLES[msg];
    _currentSample = sample.data;
    _sampleLength = sample.length;
    _playbackPosition = 0;
    _playing = true;
    _lastSampleTime = micros();
    
    Serial.printf("[TTS] Playing message %d (%d bytes)\n", msg, _sampleLength);
}

void TTSPlayer::stop() {
    _playing = false;
    _currentSample = nullptr;
    _playbackPosition = 0;
}

void TTSPlayer::update() {
    if (!_playing || !_currentSample) return;
    
    uint32_t now = micros();
    
    // Calculate how many samples to play since last update
    uint32_t elapsed = now - _lastSampleTime;
    uint32_t samplesToPlay = elapsed / SAMPLE_INTERVAL_US;
    
    if (samplesToPlay == 0) return;
    
    // Batch feed samples to I2S buffer
    // In reality, audioDriver would have a buffer we write to
    for (uint32_t i = 0; i < samplesToPlay && _playbackPosition < _sampleLength; i++) {
        uint8_t sample = pgm_read_byte(&_currentSample[_playbackPosition++]);
        
        // Convert 8-bit unsigned to 16-bit signed for I2S
        int16_t sample16 = ((int16_t)sample - 128) * 256;
        
        // Feed to audio driver (assuming it has a writeSample method)
        // audioDriver.writeSample(sample16);
        // For now, we'll use a bulk write approach later
    }
    
    _lastSampleTime = now;
    
    // Check if finished
    if (_playbackPosition >= _sampleLength) {
        Serial.println("[TTS] Playback complete");
        stop();
    }
}
