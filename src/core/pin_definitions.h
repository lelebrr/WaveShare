#pragma once

// ========== WAVESHARE ESP32-S3-Touch-AMOLED-1.8 Pin Definitions ==========
// Official specs: https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8

// === DISPLAY QSPI (SH8601 driver) ===
#define LCD_SDIO0 4
#define LCD_SDIO1 5
#define LCD_SDIO2 6
#define LCD_SDIO3 7
#define LCD_SCLK 11
#define LCD_CS 12
#define LCD_WIDTH 368
#define LCD_HEIGHT 448

// === TOUCH I2C (FT3168 controller) ===
#define IIC_SDA 15
#define IIC_SCL 14
#define TP_INT 21
#define FT3168_ADDR 0x38

// === I2C DEVICES ADDRESSES ===
#define TCA9554_ADDR 0x20  // IO Expander (CRITICAL for display!)
#define AXP2101_ADDR 0x34  // PMU (power management)
#define PCF85063_ADDR 0x51 // RTC
#define QMI8658_ADDR 0x6B  // IMU (accelerometer + gyroscope)
#define ES8311_ADDR 0x18   // Audio codec

// === AUDIO I2S (ES8311 codec) ===
#define I2S_MCK_IO 16
#define I2S_BCK_IO 9
#define I2S_WS_IO 45
#define I2S_DI_IO 10 // Data In (microphone)
#define I2S_DO_IO 8  // Data Out (speaker)
#define AUDIO_PA 46  // Power Amplifier enable

// === SD CARD SDMMC ===
#define SD_CLK 2
#define SD_CMD 1
#define SD_DATA0 3

// === BUTTONS ===
#define BTN_PWR 0   // Power/Boot button
#define BTN_USER 47 // User button

// === VIBRATION MOTOR (if populated) ===
#define VIBRA_PIN 42 // Optional vibration motor

// === STATUS LED ===
#define STATUS_LED_PIN 48 // RGB LED (Renamed to avoid conflict)
