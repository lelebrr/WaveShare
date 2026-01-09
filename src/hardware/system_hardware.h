#pragma once

#include "../core/config.h"
#include "../core/pin_definitions.h"
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// Bibliotecas Externas
#include "Arduino_GFX_Library.h"
// SD temporariamente desabilitado - FS.h não encontrado no framework ESP32 3.x
// Requer investigação adicional para resolver dependência
// #include "FS.h"
// #include "SD_MMC.h"
#include "SensorPCF85063.hpp"
#include "SensorQMI8658.hpp"
#include <ESP_IOExpander_Library.h>
#include <XPowersLib.h>
#include <lvgl.h>

class SystemHardware {
public:
  SystemHardware();

  // Touch Data
  struct TouchPoint {
    int x;
    int y;
    bool touched;
  };

  // Initialization
  bool init();

  // Sub-system access
  bool initDisplay();
  bool initTouch();
  bool initPMU();
  bool initSensors(); // IMU, RTC
  bool initSD();
  bool initAudio();
  void initLVGL(); // LVGL wrapper

  // Hardware Control
  void setDisplayBrightness(uint8_t brightness);
  void vibratorOn(uint32_t ms);
  void vibratorOff();

  // Battery/Power Status
  float getBatteryVoltage();
  int getBatteryPercent();
  int16_t getBatteryCurrent(); // mA (positive=charging, negative=discharging)
  bool isCharging();
  bool isUSBConnected();
  bool isPMUAvailable() const { return pmu_initialized; }

  // RTC Functions
  bool isRTCAvailable() const { return rtc_initialized; }
  void getRTCDateTime(uint16_t *year, uint8_t *month, uint8_t *day,
                      uint8_t *hour, uint8_t *minute, uint8_t *second);
  void setRTCDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour,
                      uint8_t minute, uint8_t second);
  String getTimeString();

  // Input
  TouchPoint getTouch();

  // Objects Access (for other modules)
  ESP_IOExpander *getExpander() { return expander; }
  SensorQMI8658 *getIMU() { return &imu; }
  Arduino_GFX *getDisplay() { return gfx; }

  // Peripheral Control (Tip 9)
  void disableSensors(); // IMU, Mic
  void enableSensors();
  void disableAudio();
  void enableAudio();

  // Power Management & Sleep
  void setGhostMode(bool enabled);
  void enterLightSleep();
  void checkDeepSleep();
  void setDisplayPower(bool on);

  // CPU Frequency Control
  void setCpuFrequency(int mhz);

  // Idle Timer
  void resetIdleTimer() { last_interaction = millis(); }
  void setIdleTimeout(uint32_t ms) { idle_timeout_ms = ms; }
  bool isIdleTimeout() {
    return (millis() - last_interaction > idle_timeout_ms);
  }

  // Tip 66: Battery Saver Mode
  void enableBatterySaver(bool enabled);

  uint32_t getLastInteraction() { return last_interaction; }

private:
  uint32_t last_interaction = 0;
  uint32_t idle_timeout_ms =
      15000; // Default 15s (User Tip 66 says 8s just for saver)

  // Pointers to hardware objects
  Arduino_DataBus *bus;
  Arduino_GFX *gfx;
  ESP_IOExpander *expander;
  XPowersAXP2101 pmu;
  SensorPCF85063 rtc;
  SensorQMI8658 imu;

  bool display_initialized;
  bool touch_initialized;
  bool pmu_initialized;
  bool rtc_initialized;

  void resetDisplayViaExpander();
};

extern SystemHardware sys_hw;

// Alias for plugins compatibility
#define hardware sys_hw
