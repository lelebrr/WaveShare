/**
 * Configuração de Hardware - Waveshare ESP32-S3-Touch-AMOLED-1.8
 *
 * ESPECIFICAÇÕES OFICIAIS:
 * - MCU: ESP32-S3-WROOM-1-N16R8 (16MB Flash, 8MB OPI PSRAM)
 * - Display: SH8601 AMOLED 368x448 pixels (QSPI interface)
 * - Touch: FT3168 capacitive (I2C)
 * - PMU: AXP2101
 * - IMU: QMI8658C
 * - RTC: PCF85063
 * - Audio: ES8311 codec
 * - WiFi: 2.4GHz 802.11 b/g/n
 * - Bluetooth: BLE 5.0
 *
 * Fonte: https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8
 */

#pragma once

// ========== DISPLAY SH8601 (QSPI) ==========
#define DISPLAY_MODEL "SH8601"
#define DISPLAY_INTERFACE "QSPI" // Quad SPI, não SPI normal!
#define DISPLAY_WIDTH 368
#define DISPLAY_HEIGHT 448
#define DISPLAY_COLOR_BITS 16 // RGB565

// QSPI Data Pins (quad, 4 data lines)
#define LCD_SDIO0 4 // QSPI D0
#define LCD_SDIO1 5 // QSPI D1
#define LCD_SDIO2 6 // QSPI D2
#define LCD_SDIO3 7 // QSPI D3

// QSPI Control Pins
#define LCD_SCLK 11 // QSPI Clock
#define LCD_CS 12   // Chip Select
#define LCD_DC 10   // Data/Command
#define LCD_RST 13  // Reset
#define LCD_BL 1    // Backlight (PWM)

// ========== TOUCH FT3168 (I2C) ==========
#define TOUCH_MODEL "FT3168"
#define TOUCH_I2C_ADDR 0x38   // FT3168 I2C address
#define TOUCH_INT 21          // Touch interrupt (TP_INT)
#define TOUCH_RST -1          // Touch reset (not used directly)
#define TOUCH_SDA 15          // I2C SDA (IIC_SDA)
#define TOUCH_SCL 14          // I2C SCL (IIC_SCL)
#define TOUCH_I2C_FREQ 400000 // 400 kHz

// ========== I2C SENSORS (shared bus) ==========
#define I2C_SDA 15      // Shared with touch (IIC_SDA)
#define I2C_SCL 14      // Shared with touch (IIC_SCL)
#define I2C_FREQ 400000 // 400 kHz

// PMU AXP2101
#define AXP2101_ADDR 0x34

// IMU QMI8658C
#define QMI8658_ADDR 0x6B

// RTC PCF85063
#define PCF85063_ADDR 0x51

// ========== AUDIO ES8311 (I2S) ==========
#define ES8311_ADDR 0x18  // I2C address
#define I2S_MCLK 16       // Master clock (I2S_MCK_IO)
#define I2S_BCLK 9        // Bit clock (I2S_BCK_IO)
#define I2S_WS 45         // Word select (I2S_WS_IO)
#define I2S_DOUT 8        // Data out to codec (I2S_DO_IO)
#define I2S_DIN 10        // Data in from mic (I2S_DI_IO)
#define AUDIO_PA 46       // Power Amplifier enable

// ========== BUTTONS ==========
#define BTN_BOOT 0 // Boot button (GPIO0)
#define BTN_PWR -1 // Power button (handled by AXP2101)

// ========== SD CARD (1-bit SDMMC) ==========
#define SD_CLK 2    // SD CLK
#define SD_CMD 1    // SD CMD
#define SD_DATA0 3  // SD DATA0
// Note: Using 1-bit mode, DATA1-3 not used

// ========== USB ==========
#define USB_DN 19 // USB D-
#define USB_DP 20 // USB D+

// ========== GPIO DISPONÍVEIS ==========
// GPIOs expostos nos pads 1mm:
// GPIO17, GPIO18, GPIO43, GPIO44, GPIO35, GPIO36, GPIO37
// + UART: GPIO43 (TX), GPIO44 (RX)

// ========== POWER MANAGEMENT ==========
#define BATTERY_CONNECTOR "MX1.25" // 3.7V Lithium battery
#define BACKUP_BATTERY_PAD true    // Backup battery pad for RTC

// ========== MEMÓRIA ==========
#define FLASH_SIZE_MB 16 // 16MB NOR Flash
#define PSRAM_SIZE_MB 8  // 8MB OPI PSRAM
#define PSRAM_TYPE "OPI" // Octal Peripheral Interface
#define SRAM_SIZE_KB 512 // Internal SRAM

// ========== WIRELESS ==========
#define WIFI_STANDARD "802.11 b/g/n"
#define WIFI_FREQ "2.4GHz"
#define BLE_VERSION "5.0"
#define ANTENNA_TYPE "Onboard SMD"

// ========== PINOUT SUMMARY ==========
/*
DISPLAY QSPI (SH8601):
- D0-D3: GPIO4-7
- CLK: GPIO11
- CS: GPIO12
- DC: GPIO10
- RST: GPIO13
- BL: GPIO1

TOUCH I2C (FT3168):
- SDA: GPIO8
- SCL: GPIO9
- INT: GPIO14
- RST: GPIO21
- ADDR: 0x38

SENSORS I2C (shared):
- SDA: GPIO8
- SCL: GPIO9
- AXP2101: 0x34
- QMI8658: 0x6B
- PCF85063: 0x51

AUDIO I2S (ES8311):
- I2C: GPIO8/9 (config)
- ADDR: 0x18
- MCLK: GPIO2
- BCLK: GPIO3
- WS: GPIO15
- DOUT: GPIO16
- DIN: GPIO42

SD CARD SDIO:
- CLK: GPIO40
- CMD: GPIO39
- DATA0-3: GPIO38,41,45,46

BUTTONS:
- BOOT: GPIO0
- PWR: AXP2101 controlled

USB:
- D-: GPIO19
- D+: GPIO20

FREE GPIOs:
- GPIO17, 18, 35, 36, 37, 43, 44
*/

// ========== HARDWARE VALIDATION ==========
static inline bool hardware_validate_config() {
// Verify critical pins don't overlap
#if LCD_SDIO0 == I2C_SDA || LCD_SDIO1 == I2C_SCL
#error "Display QSPI pins conflict with I2C!"
#endif

#if TOUCH_SDA != I2C_SDA || TOUCH_SCL != I2C_SCL
#error "Touch must share I2C bus with sensors!"
#endif

  return true;
}
