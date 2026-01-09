#pragma once

// ==========================================
// WavePwn Global Configuration
// Tip 3: Header-only config with #pragma once
// ==========================================

// --- System ---
#define SYSTEM_VERSION "2.1.0"
#define DEVICE_NAME "LeleWatch Pwn"

// --- Hardware ---
#define HAS_SD_CARD
#define HAS_TOUCH
#define HAS_IMU_QMI8658
#define HAS_MAX98357A
#define HAS_INMP441

// --- Performance (Tips 11-20) ---
#define USE_PSRAM_FOR_LVGL 1 // Tip 48
#define USE_FAST_DEAUTH 1    // Tip 18
#define USE_CORE_PINNING 1   // Tip 8/18/32

// --- Logging (Tip 27/36/37) ---
// Levels: 0=None, 1=Error, 2=Warn, 3=Info, 4=Debug, 5=Verbose
#ifdef RELEASE_BUILD
#define LOG_LEVEL 0 // Tip 36: No logs in production
#else
#define LOG_LEVEL 3
#endif

#define DEBUG_ATTACKS 1 // Tip 37 helper

#if DEBUG_ATTACKS && (LOG_LEVEL > 0)
#define LOG_ATTACK(fmt, ...) Serial.printf("[ATK] " fmt "\n", ##__VA_ARGS__)
#else
#define LOG_ATTACK(...)
#endif

// --- WiFi ---
#define DEFAULT_TX_POWER 52 // 13dBm (Tip 57: 13 * 4 = 52)

// --- Task Stack Sizes ---
#define LVGL_TASK_STACK_SIZE 16384
#define ATTACK_TASK_STACK_SIZE 8192
#define WIFI_TASK_STACK_SIZE 4096

// --- Concepts (Tip 5) ---
#if __cplusplus >= 202002L
// C++20 is active
#define HAS_CPP20
#endif
