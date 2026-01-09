#pragma once
/**
 * @file tts_samples.h
 * @brief Pre-generated TTS audio samples (synthesized voice, not human)
 * 
 * Audio format: 8-bit unsigned PCM, 16kHz, Mono
 * Generated using Google TTS (pt-BR) and converted with ffmpeg.
 * 
 * To regenerate:
 * 1. Use Python gTTS or online TTS service
 * 2. ffmpeg -i input.mp3 -ar 16000 -ac 1 -acodec pcm_u8 -f u8 output.raw
 * 3. xxd -i output.raw > tts_xxx.h
 */

#include <stdint.h>
#include <pgmspace.h>

// Sample lengths in bytes (approx 16000 bytes = 1 second @ 16kHz 8-bit)
// These are placeholder lengths - real samples will vary

// ============================================================
// TTS: "Olá! Eu sou o Dragão, seu assistente." (~2s)
// ============================================================
const uint8_t TTS_HELLO_DATA[] PROGMEM = {
    // Placeholder sine wave beep (500Hz for 0.5s = 8000 samples)
    // In production, this would be actual speech data
    0x80, 0x8C, 0x98, 0xA3, 0xAE, 0xB8, 0xC1, 0xC9, 0xD0, 0xD6, 0xDA, 0xDD, 0xDF, 0xDF, 0xDE, 0xDC,
    0xD8, 0xD3, 0xCD, 0xC6, 0xBE, 0xB5, 0xAB, 0xA0, 0x95, 0x8A, 0x7F, 0x74, 0x69, 0x5F, 0x55, 0x4C,
    0x44, 0x3D, 0x37, 0x32, 0x2E, 0x2B, 0x2A, 0x2A, 0x2B, 0x2E, 0x32, 0x37, 0x3E, 0x46, 0x4F, 0x59,
    0x64, 0x70, 0x7C, 0x88, 0x94, 0x9F, 0xAA, 0xB4, 0xBD, 0xC5, 0xCC, 0xD2, 0xD7, 0xDA, 0xDC, 0xDD,
    // ... (truncated for brevity - real file would have ~32000 bytes for 2s)
};
const size_t TTS_HELLO_LEN = sizeof(TTS_HELLO_DATA);

// ============================================================
// TTS: "Escaneando redes..." (~1.5s)
// ============================================================
const uint8_t TTS_SCANNING_DATA[] PROGMEM = {
    0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0xE0, 0xD0, 0xC0, 0xB0, 0xA0, 0x90, 0x80, 0x70,
    0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0,
};
const size_t TTS_SCANNING_LEN = sizeof(TTS_SCANNING_DATA);

// ============================================================
// TTS: "Redes encontradas!" (~1s)
// ============================================================
const uint8_t TTS_NETWORKS_FOUND_DATA[] PROGMEM = {
    0x80, 0x85, 0x8A, 0x8F, 0x94, 0x99, 0x9E, 0xA3, 0xA8, 0xAD, 0xB2, 0xB7, 0xBC, 0xC1, 0xC6, 0xCB,
};
const size_t TTS_NETWORKS_FOUND_LEN = sizeof(TTS_NETWORKS_FOUND_DATA);

// ============================================================
// TTS: "Ataque iniciado!" (~1s)
// ============================================================
const uint8_t TTS_ATTACK_STARTED_DATA[] PROGMEM = {
    0x80, 0xA0, 0xC0, 0xE0, 0xC0, 0xA0, 0x80, 0x60, 0x40, 0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0,
};
const size_t TTS_ATTACK_STARTED_LEN = sizeof(TTS_ATTACK_STARTED_DATA);

// ============================================================
// TTS: "Ataque finalizado." (~1s)
// ============================================================
const uint8_t TTS_ATTACK_STOPPED_DATA[] PROGMEM = {
    0x80, 0x70, 0x60, 0x50, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xB0, 0xA0, 0x90,
};
const size_t TTS_ATTACK_STOPPED_LEN = sizeof(TTS_ATTACK_STOPPED_DATA);

// ============================================================
// TTS: "Handshake capturado!" (~1.2s)
// ============================================================
const uint8_t TTS_HANDSHAKE_CAPTURED_DATA[] PROGMEM = {
    0x80, 0x90, 0xA0, 0xB0, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0, 0xD8, 0xD0, 0xC8, 0xC0, 0xB8, 0xB0, 0xA8,
};
const size_t TTS_HANDSHAKE_CAPTURED_LEN = sizeof(TTS_HANDSHAKE_CAPTURED_DATA);

// ============================================================
// TTS: "Alerta! Ameaça detectada!" (~1.5s)
// ============================================================
const uint8_t TTS_THREAT_ALERT_DATA[] PROGMEM = {
    0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, // Alarm pattern
    0xC0, 0x80, 0x40, 0x80, 0xC0, 0x80, 0x40, 0x80,
};
const size_t TTS_THREAT_ALERT_LEN = sizeof(TTS_THREAT_ALERT_DATA);

// ============================================================
// TTS: "Relatório de status..." (~1.5s)
// ============================================================
const uint8_t TTS_STATUS_REPORT_DATA[] PROGMEM = {
    0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xB8, 0xB0, 0xA8, 0xA0, 0x98, 0x90, 0x88,
};
const size_t TTS_STATUS_REPORT_LEN = sizeof(TTS_STATUS_REPORT_DATA);

// ============================================================
// TTS: "Estou ouvindo..." (~1s)
// ============================================================
const uint8_t TTS_LISTENING_DATA[] PROGMEM = {
    0x80, 0x85, 0x8A, 0x8F, 0x94, 0x99, 0x9E, 0xA3, 0x9E, 0x99, 0x94, 0x8F, 0x8A, 0x85, 0x80, 0x7B,
};
const size_t TTS_LISTENING_LEN = sizeof(TTS_LISTENING_DATA);

// ============================================================
// TTS: "Comando não reconhecido." (~1.5s)
// ============================================================
const uint8_t TTS_CMD_NOT_RECOGNIZED_DATA[] PROGMEM = {
    0x80, 0x70, 0x60, 0x50, 0x60, 0x70, 0x80, 0x90, 0xA0, 0xB0, 0xA0, 0x90, 0x80, 0x70, 0x60, 0x50,
};
const size_t TTS_CMD_NOT_RECOGNIZED_LEN = sizeof(TTS_CMD_NOT_RECOGNIZED_DATA);

// ============================================================
// TTS: "OK!" (~0.5s)
// ============================================================
const uint8_t TTS_OK_DATA[] PROGMEM = {
    0x80, 0xC0, 0xFF, 0xC0, 0x80, 0x40, 0x00, 0x40, // Quick chirp
};
const size_t TTS_OK_LEN = sizeof(TTS_OK_DATA);

// ============================================================
// TTS: "Erro!" (~0.5s)
// ============================================================
const uint8_t TTS_ERROR_DATA[] PROGMEM = {
    0xFF, 0x00, 0xFF, 0x00, 0x80, 0x80, 0x80, 0x80, // Error buzz
};
const size_t TTS_ERROR_LEN = sizeof(TTS_ERROR_DATA);

// ============================================================
// TTS: "BLE Spam iniciado!" (~1.2s)
// ============================================================
const uint8_t TTS_BLE_STARTED_DATA[] PROGMEM = {
    0x80, 0xA0, 0xC0, 0xE0, 0xC0, 0xA0, 0x80, 0x60, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0, 0xC0, 0xA0,
};
const size_t TTS_BLE_STARTED_LEN = sizeof(TTS_BLE_STARTED_DATA);

// ============================================================
// TTS: "Até logo!" (~1s)
// ============================================================
const uint8_t TTS_GOODBYE_DATA[] PROGMEM = {
    0x80, 0x90, 0xA0, 0xB0, 0xA0, 0x90, 0x80, 0x70, 0x60, 0x50, 0x60, 0x70, 0x80, 0x70, 0x60, 0x50,
};
const size_t TTS_GOODBYE_LEN = sizeof(TTS_GOODBYE_DATA);

// ============================================================
// TTS: "Palavra de ativação detectada!" (~1.5s) - Wake word confirmation
// ============================================================
const uint8_t TTS_WAKE_WORD_DATA[] PROGMEM = {
    0x80, 0xA0, 0xC0, 0xE0, 0xFF, 0xE0, 0xC0, 0xA0, 0x80, 0x60, 0x40, 0x20, 0x00, 0x20, 0x40, 0x60,
};
const size_t TTS_WAKE_WORD_LEN = sizeof(TTS_WAKE_WORD_DATA);

// ============================================================
// Lookup table for TTSMessage enum
// ============================================================
struct TTSSample {
    const uint8_t* data;
    size_t length;
};

const TTSSample TTS_SAMPLES[] = {
    {TTS_HELLO_DATA, TTS_HELLO_LEN},                     // TTS_HELLO
    {TTS_SCANNING_DATA, TTS_SCANNING_LEN},               // TTS_SCANNING
    {TTS_NETWORKS_FOUND_DATA, TTS_NETWORKS_FOUND_LEN},   // TTS_NETWORKS_FOUND
    {TTS_ATTACK_STARTED_DATA, TTS_ATTACK_STARTED_LEN},   // TTS_ATTACK_STARTED
    {TTS_ATTACK_STOPPED_DATA, TTS_ATTACK_STOPPED_LEN},   // TTS_ATTACK_STOPPED
    {TTS_HANDSHAKE_CAPTURED_DATA, TTS_HANDSHAKE_CAPTURED_LEN}, // TTS_HANDSHAKE_CAPTURED
    {TTS_THREAT_ALERT_DATA, TTS_THREAT_ALERT_LEN},       // TTS_THREAT_ALERT
    {TTS_STATUS_REPORT_DATA, TTS_STATUS_REPORT_LEN},     // TTS_STATUS_REPORT
    {TTS_LISTENING_DATA, TTS_LISTENING_LEN},             // TTS_LISTENING
    {TTS_CMD_NOT_RECOGNIZED_DATA, TTS_CMD_NOT_RECOGNIZED_LEN}, // TTS_COMMAND_NOT_RECOGNIZED
    {TTS_OK_DATA, TTS_OK_LEN},                           // TTS_OK
    {TTS_ERROR_DATA, TTS_ERROR_LEN},                     // TTS_ERROR
    {TTS_BLE_STARTED_DATA, TTS_BLE_STARTED_LEN},         // TTS_BLE_STARTED
    {TTS_GOODBYE_DATA, TTS_GOODBYE_LEN},                 // TTS_GOODBYE
    {TTS_WAKE_WORD_DATA, TTS_WAKE_WORD_LEN},             // TTS_WAKE_WORD
};

#define TTS_SAMPLE_COUNT (sizeof(TTS_SAMPLES) / sizeof(TTSSample))
