#pragma once

/**
 * @file clock_display.h
 * @brief Plugin de relógio usando RTC PCF85063
 */

#include "plugin_base.h"

struct DateTime {
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t weekday;
};

class ClockDisplayPlugin : public PluginBase {
public:
  ClockDisplayPlugin();

  const char *getName() const override { return "clock"; }
  const char *getAuthor() const override { return "WavePwn"; }
  const char *getVersion() const override { return "1.0.0"; }

  bool onLoad() override;
  void update() override;

  // Getters
  const DateTime &getDateTime() const { return _dt; }
  bool isRTCAvailable() const { return _rtcAvailable; }

  // Setters
  void setDateTime(const DateTime &dt);
  void setFromUnix(uint32_t timestamp);

  // Formatação
  String getTimeString() const; // "14:35:22"
  String getDateString() const; // "2025-12-07"
  String getFullString() const; // "2025-12-07 14:35:22"
  String getShortTime() const;  // "14:35"

private:
  DateTime _dt;
  bool _rtcAvailable;
  unsigned long _lastUpdate;

  void readRTC();
  void writeRTC();
};

extern ClockDisplayPlugin clockPlugin;
