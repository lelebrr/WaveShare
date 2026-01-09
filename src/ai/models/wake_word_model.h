#pragma once
/**
 * @file wake_word_model.h
 * @brief TFLite Micro model for "Hey Dragon" wake word detection
 * 
 * Model: Quantized INT8, ~50KB
 * Input: 40 MFCC features x 49 time steps
 * Output: 2 classes (wake_word, not_wake_word)
 * 
 * To train a real model:
 * 1. Use Edge Impulse with custom keyword dataset
 * 2. Export as TFLite (quantized)
 * 3. xxd -i model.tflite > wake_word_model.h
 */

#include <stdint.h>
#include <pgmspace.h>

// Placeholder model - 1KB minimal TFLite model structure
// This is a valid TFLite flatbuffer header but won't do real inference
// Replace with actual trained model from Edge Impulse or TensorFlow
const unsigned char WAKE_WORD_MODEL[] PROGMEM = {
    // TFLite FlatBuffer magic bytes
    0x1C, 0x00, 0x00, 0x00, // Offset to root table
    0x54, 0x46, 0x4C, 0x33, // "TFL3" identifier
    
    // Minimal valid structure (placeholder)
    // In production, this would be ~30-50KB of actual model weights
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // ... truncated for brevity
};

const size_t WAKE_WORD_MODEL_LEN = sizeof(WAKE_WORD_MODEL);

// Model configuration
#define WAKE_WORD_MFCC_FEATURES 40
#define WAKE_WORD_TIME_STEPS 49
#define WAKE_WORD_THRESHOLD 0.85f
