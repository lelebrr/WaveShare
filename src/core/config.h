#pragma once

// ========== WavePwn Global Configuration ==========

// === PROJECT INFO ===
#define WAVEPWN_VERSION "1.0.0"
#define WAVEPWN_CODENAME "Dragon"

// === HARDWARE PLATFORM ===
// Defined in platformio.ini or board defs
// #define WAVESHARE_AMOLED_1_8
// #define BOARD_HAS_PSRAM

// === MEMORY CONFIGURATION ===
#define PSRAM_SIZE_MB 8
#define FLASH_SIZE_MB 16

// === LVGL CONFIGURATION ===
#define LVGL_BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT / 10)
#define LVGL_TICK_PERIOD_MS 2

// === WIFI CONFIGURATION ===
#define WIFI_AP_SSID "WavePwn"
#define WIFI_AP_PASSWORD "wavepwn123"
#define WIFI_AP_CHANNEL 6
#define WIFI_MAX_CLIENTS 4

// === WEB SERVER ===
#define WEB_SERVER_PORT 80
#define WEBSOCKET_PORT 81

// === PWNAGOTCHI CONFIGURATION ===
#define DRAGON_NAME "WaveDragon"
#define DRAGON_UPDATE_MS 100         // Animation update rate
#define MOOD_UPDATE_MS 5000          // Mood calculation interval
#define AUTO_SAVE_INTERVAL_MS 300000 // 5 minutes

// === WIFI ATTACKS CONFIGURATION ===
#define MAX_HANDSHAKES 1000
#define PCAP_BUFFER_SIZE 4096
#define DEAUTH_PACKETS_BURST 64
#define BEACON_FLOOD_DELAY_US 100

// === BATTERY THRESHOLDS ===
#define BATTERY_CRITICAL 10 // % - força sleep
#define BATTERY_LOW 20      // % - aviso
#define BATTERY_FULL 95     // % - para carga

// === SERIAL DEBUG ===
#define SERIAL_BAUD 115200
#define DEBUG_ENABLED true

// === FILE PATHS (SD CARD) ===
#define PATH_HANDSHAKES "/handshakes"
#define PATH_PAYLOADS "/payloads"
#define PATH_LOGS "/logs"
#define PATH_CONFIG "/config.json"

// === DISPLAY BRIGHTNESS ===
#define BRIGHTNESS_MIN 10
#define BRIGHTNESS_DEFAULT 150
#define BRIGHTNESS_MAX 255

// === TOUCH SENSITIVITY ===
#define TOUCH_THRESHOLD 40

// === SLEEP TIMEOUT ===
#define SLEEP_TIMEOUT_MS 300000 // 5 minutes idle

// === AI DEBUG ===
#define DEBUG_SAVE_RAW_FEATURES 1

// === OPENAI API CONFIGURATION ===
// Get your API key from: https://platform.openai.com/api-keys
// SECURITY WARNING: Do not commit real keys to public repos!
#define OPENAI_API_KEY "sk-YOUR-API-KEY-HERE"  // Replace with your key

// TTS Voice options: alloy, echo, fable, onyx, nova, shimmer
#define OPENAI_TTS_VOICE "nova"
#define OPENAI_TTS_MODEL "tts-1"
#define OPENAI_WHISPER_MODEL "whisper-1"

// === COLORS (RGB565) ===
#define COLOR_PRIMARY 0x07E0    // Green
#define COLOR_SECONDARY 0x001F  // Blue
#define COLOR_ACCENT 0xF800     // Red
#define COLOR_SUCCESS 0x07E0    // Green
#define COLOR_WARNING 0xFFE0    // Yellow
#define COLOR_ERROR 0xF800      // Red
#define COLOR_INFO 0x07FF       // Cyan
#define COLOR_TEXT 0xFFFF       // White
#define COLOR_BACKGROUND 0x0000 // Black
