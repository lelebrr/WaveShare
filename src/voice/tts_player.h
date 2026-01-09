#pragma once
/**
 * @file tts_player.h
 * @brief Offline TTS Player using pre-generated audio samples
 */

#include "../hardware/audio_driver.h"
#include "../voice/voice_assistant.h" // For TTSMessage enum
#include <Arduino.h>

class TTSPlayer {
public:
    TTSPlayer();
    
    bool begin();
    
    // Play a pre-recorded TTS message
    void play(TTSMessage msg);
    
    // Stop current playback
    void stop();
    
    // Check if playing
    bool isPlaying() const { return _playing; }
    
    // Update (call in loop for async playback)
    void update();

private:
    bool _playing;
    const uint8_t* _currentSample;
    size_t _sampleLength;
    size_t _playbackPosition;
    uint32_t _lastSampleTime;
};

extern TTSPlayer ttsPlayer;
