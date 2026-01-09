/**
 * @file system_hardware.cpp
 * @brief Gerenciador de hardware do sistema LeleWatch
 *
 * Inicializa e controla todos os periféricos:
 * - Display QSPI AMOLED
 * - Touch FT3168
 * - PMU AXP2101
 * - IMU QMI8658
 * - RTC PCF85063
 * - SD Card
 * - Audio ES8311
 */

#include "system_hardware.h"
#include "../core/globals.h"
#include "audio_driver.h"
#include <FS.h>
#include <SD_MMC.h>

// Declaração externa da flag de suspensão BLE/LVGL
extern bool g_suspend_ble_lvgl;

SystemHardware sys_hw;

SystemHardware::SystemHardware()
    : bus(nullptr), gfx(nullptr), expander(nullptr), display_initialized(false),
      touch_initialized(false), pmu_initialized(false), rtc_initialized(false) {
}

bool SystemHardware::init() {
  Serial.println("[HW] System Hardware Init Starting...");

  // 1. I2C Initialization (Critical for PMU and IO Expander)
  if (!Wire.begin(IIC_SDA, IIC_SCL)) {
    Serial.println("[HW] I2C Init Failed!");
    return false;
  }
  Serial.println("[HW] I2C Initialized");

  // 2. PMU (Power Management Unit)
  initPMU();

  // 3. IO Expander (CRITICAL for Display Reset)
  expander = new ESP_IOExpander_TCA95xx_8bit((i2c_port_t)0, TCA9554_ADDR);

  if (expander) {
    expander->init();
    expander->begin();
    Serial.println("[HW] IO Expander Initialized");

    // Setup control pins for display reset
    expander->pinMode(0, OUTPUT);
    expander->pinMode(1, OUTPUT);
    expander->pinMode(2, OUTPUT); // Display RST

    // Reset Display Sequence
    resetDisplayViaExpander();
  } else {
    Serial.println("[HW] IO Expander Creation Failed!");
    return false;
  }

  // 4. Display (QSPI)
  initDisplay();

  // 5. Touch
  initTouch();

  // 6. Other Sensors (IMU, RTC)
  initSensors();

  // 7. SD Card
  initSD();

  // 8. Audio
  initAudio();

  // 9. LVGL
  initLVGL();

  Serial.println("[HW] Hardware Init Complete");
  return true;
}

void SystemHardware::resetDisplayViaExpander() {
  if (!expander)
    return;
  Serial.println("[HW] Resetting display...");
  expander->digitalWrite(0, LOW);
  expander->digitalWrite(1, LOW);
  expander->digitalWrite(2, LOW); // RST LOW
  delay(20);
  expander->digitalWrite(0, HIGH);
  expander->digitalWrite(1, HIGH);
  expander->digitalWrite(2, HIGH); // RST HIGH
  delay(50);
}

bool SystemHardware::initDisplay() {
  if (display_initialized)
    return true;
  Serial.println("[HW] Initializing Display QSPI...");
  bus = new Arduino_ESP32QSPI(LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1, LCD_SDIO2,
                              LCD_SDIO3);
  gfx = new Arduino_SH8601(bus, -1, 0, false, LCD_WIDTH, LCD_HEIGHT);
  if (!gfx->begin()) {
    Serial.println("[HW] Display Begin Failed!");
    return false;
  }
  gfx->fillScreen(BLACK);
  gfx->Display_Brightness(BRIGHTNESS_DEFAULT);
  g_display = gfx;
  display_initialized = true;
  Serial.println("[HW] Display Initialized");
  return true;
}

bool SystemHardware::initTouch() {
  Wire.beginTransmission(FT3168_ADDR);
  if (Wire.endTransmission() == 0) {
    Serial.println("[HW] Touch FT3168 Found");
    touch_initialized = true;
    pinMode(TP_INT, INPUT_PULLUP);
    return true;
  } else {
    Serial.println("[HW] Touch FT3168 NOT Found!");
    return false;
  }
}

SystemHardware::TouchPoint SystemHardware::getTouch() {
  TouchPoint p = {0, 0, false};
  if (!touch_initialized)
    return p;

  // Verifica se há interrupção antes de usar I2C
  if (digitalRead(TP_INT) == HIGH) {
    return p;
  }

  Wire.beginTransmission(FT3168_ADDR);
  Wire.write(0x02); // Registrador de status/coordenadas
  if (Wire.endTransmission() != 0) {
    return p;
  }

  Wire.requestFrom((uint8_t)FT3168_ADDR, (uint8_t)5);
  if (Wire.available() >= 5) {
    uint8_t status = Wire.read();
    uint8_t xh = Wire.read();
    uint8_t xl = Wire.read();
    uint8_t yh = Wire.read();
    uint8_t yl = Wire.read();

    if ((status & 0x0F) > 0) {
      p.x = ((xh & 0x0F) << 8) | xl;
      p.y = ((yh & 0x0F) << 8) | yl;
      p.touched = true;
    }
  }
  return p;
}

bool SystemHardware::initPMU() {
  if (!pmu.begin(Wire, AXP2101_ADDR, IIC_SDA, IIC_SCL)) {
    Serial.println("[HW] PMU AXP2101 Not Found");
    return false;
  }
  pmu_initialized = true;
  Serial.println("[HW] PMU AXP2101 Found");

  // Configuração de voltagem
  pmu.setDC1Voltage(3300); // Main 3.3V
  pmu.setALDO1Voltage(1800);
  pmu.setALDO2Voltage(1800);
  pmu.setALDO3Voltage(1800);
  pmu.setALDO4Voltage(1800);
  pmu.setBLDO1Voltage(1800);
  pmu.setBLDO2Voltage(1800);

  return true;
}

bool SystemHardware::initSensors() {
  if (rtc.begin(Wire, PCF85063_ADDR, IIC_SDA, IIC_SCL)) {
    Serial.println("[HW] RTC Found");
    rtc_initialized = true;
  }
  if (imu.begin(Wire, QMI8658_ADDR, IIC_SDA, IIC_SCL)) {
    Serial.println("[HW] IMU Found");
    imu.configAccelerometer(SensorQMI8658::ACC_RANGE_4G,
                            SensorQMI8658::ACC_ODR_1000Hz,
                            SensorQMI8658::LPF_MODE_0);
    imu.enableAccelerometer();
  }
  return true;
}

bool SystemHardware::initSD() {
  SD_MMC.setPins(SD_CLK, SD_CMD, SD_DATA0);
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("[HW] SD Card Mount Failed");
    return false;
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("[HW] No SD Card attached");
    return false;
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("[HW] SD Card Initialized: %lluMB\n", cardSize);
  return true;
}

bool SystemHardware::initAudio() {
  Wire.beginTransmission(ES8311_ADDR);
  if (Wire.endTransmission() == 0) {
    Serial.println("[HW] Audio ES8311 Found");
    return true;
  }
  return false;
}

void SystemHardware::initLVGL() {
  lv_init();
  Serial.println("[HW] LVGL Initialized");
}

void SystemHardware::setDisplayBrightness(uint8_t brightness) {
  if (gfx)
    gfx->Display_Brightness(brightness);
}

float SystemHardware::getBatteryVoltage() {
  if (pmu_initialized)
    return pmu.getBattVoltage() / 1000.0f;
  return 0.0f;
}

// LiPo Discharge Curve LUT (Voltage values in mV for 0% to 100% in 10% steps)
// 3.0V, 3.3V, 3.6V, 3.7V, 3.75V, 3.79V, 3.83V, 3.87V, 3.92V, 4.0V, 4.2V
const uint32_t BATTERY_LUT[11] = {
    3000, 3300, 3600, 3700, 3750, 3790, 3830, 3870, 3920, 4000, 4200
};

// Map voltage to percentage using LUT with interpolation
int SystemHardware::getBatteryPercent() {
  if (!pmu_initialized) return 0;
  
  float voltage = getBatteryVoltage() * 1000.0f; // Convert to mV
  // Clamp boundaries
  if (voltage <= BATTERY_LUT[0]) return 0;
  if (voltage >= BATTERY_LUT[10]) return 100;

  for (int i = 0; i < 10; i++) {
    if (voltage >= BATTERY_LUT[i] && voltage < BATTERY_LUT[i+1]) {
      // Linear interpolation
      float range = BATTERY_LUT[i+1] - BATTERY_LUT[i];
      float diff = voltage - BATTERY_LUT[i];
      float stepPercent = (diff / range) * 10.0f;
      return (i * 10) + (int)stepPercent;
    }
  }
  return 100; // Should be covered by clamp
}

int16_t SystemHardware::getBatteryCurrent() {
  if (pmu_initialized) {
    if (pmu.isCharging()) {
      return 100; // Approximate charging current
    } else {
      return -100; // Approximate discharge current
    }
  }
  return 0;
}

bool SystemHardware::isCharging() {
  if (pmu_initialized)
    return pmu.isCharging();
  return false;
}

bool SystemHardware::isUSBConnected() {
  if (pmu_initialized)
    return pmu.isVbusIn();
  return false;
}

void SystemHardware::getRTCDateTime(uint16_t *year, uint8_t *month,
                                    uint8_t *day, uint8_t *hour,
                                    uint8_t *minute, uint8_t *second) {
  if (rtc_initialized) {
    RTC_DateTime dt = rtc.getDateTime();
    *year = dt.year;
    *month = dt.month;
    *day = dt.day;
    *hour = dt.hour;
    *minute = dt.minute;
    *second = dt.second;
  } else {
    *year = 2025;
    *month = 1;
    *day = 1;
    *hour = 0;
    *minute = 0;
    *second = 0;
  }
}

void SystemHardware::setRTCDateTime(uint16_t year, uint8_t month, uint8_t day,
                                    uint8_t hour, uint8_t minute,
                                    uint8_t second) {
  if (rtc_initialized) {
    rtc.setDateTime(year, month, day, hour, minute, second);
  }
}

String SystemHardware::getTimeString() {
  uint16_t year;
  uint8_t month, day, hour, minute, second;
  getRTCDateTime(&year, &month, &day, &hour, &minute, &second);
  char buf[24];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", year, month, day,
           hour, minute, second);
  return String(buf);
}

// ===================================
// Power Management Optimizations
// ===================================

void SystemHardware::setCpuFrequency(int mhz) { setCpuFrequencyMhz(mhz); }

void SystemHardware::checkDeepSleep() {
  if (!pmu_initialized)
    return;

  // Não dorme enquanto carregando
  if (isCharging() || isUSBConnected()) {
    return;
  }

  // Deep Sleep quando ocioso e sem ataques ativos
  if (isIdleTimeout() && !g_suspend_ble_lvgl) {
    Serial.println("[PMU] System Idle. Preparing for Deep Sleep (5-10s)...");

    // Timer Wakeup: 5 to 10 seconds (randomizado)
    uint64_t sleep_us = random(5000000, 10000000);
    esp_sleep_enable_timer_wakeup(sleep_us);

    // Touch Wakeup
    esp_sleep_enable_ext0_wakeup((gpio_num_t)TP_INT, 0);

    // Desliga display e áudio
    setDisplayPower(false);
    disableAudio();

    Serial.printf("[PMU] Sleeping for %llu ms\n", sleep_us / 1000);
    Serial.flush();

    esp_deep_sleep_start();
  }
}

void SystemHardware::setDisplayPower(bool on) {
  if (on) {
    if (gfx) {
      gfx->Display_Brightness(BRIGHTNESS_DEFAULT);
    }
  } else {
    if (gfx) {
      gfx->Display_Brightness(0);
    }
  }
}

void SystemHardware::setGhostMode(bool enabled) {
  g_state.ghost_mode_enabled = enabled;
  if (enabled) {
    Serial.println("[HW] Activating GHOST MODE (Stealth)");
    disableAudio();
    if (gfx)
      gfx->Display_Brightness(0);
  } else {
    Serial.println("[HW] Deactivating GHOST MODE");
    if (g_state.audio_enabled)
      enableAudio();
    if (gfx)
      gfx->Display_Brightness(g_state.screen_brightness);
  }
}

void SystemHardware::enterLightSleep() {
  Serial.println("[PWR] Entering Light Sleep...");

  esp_sleep_enable_ext0_wakeup((gpio_num_t)TP_INT, 0);
  setCpuFrequencyMhz(80);

  esp_light_sleep_start();

  Serial.println("[PWR] Woke up from Light Sleep!");
  setCpuFrequencyMhz(240);
  resetIdleTimer();
}

void SystemHardware::disableSensors() {
  // Desabilita IMU para economizar energia
  imu.disableAccelerometer();
  imu.disableGyroscope();
  Serial.println("[PWR] IMU Disabled");
}

void SystemHardware::enableSensors() {
  imu.enableAccelerometer();
  Serial.println("[PWR] IMU Enabled");
}

void SystemHardware::disableAudio() {
  if (audioDriver.isInitialized()) {
    audioDriver.setVolume(0);
    Serial.println("[PWR] Audio Codec Disabled (Muted)");
  }
}

void SystemHardware::enableAudio() {
  if (audioDriver.isInitialized()) {
    audioDriver.setVolume(g_state.audio_volume > 0 ? g_state.audio_volume : 50);
    Serial.println("[PWR] Audio Codec Enabled");
  }
}

void SystemHardware::enableBatterySaver(bool enabled) {
  if (enabled) {
    Serial.println("[PWR] Battery Saver ENABLED (8s Timeout, No BLE, 80MHz)");
    setIdleTimeout(8000);
    btStop();
    setCpuFrequencyMhz(80);
  } else {
    Serial.println("[PWR] Battery Saver DISABLED");
    setIdleTimeout(30000);
    setCpuFrequencyMhz(240);
  }
}
