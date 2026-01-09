/**
 * @file clock_display.cpp
 * @brief Implementação do plugin de relógio
 */

#include "clock_display.h"
#include "../hardware/system_hardware.h"

ClockDisplayPlugin clockPlugin;

ClockDisplayPlugin::ClockDisplayPlugin()
    : _rtcAvailable(false), _lastUpdate(0) {
  memset(&_dt, 0, sizeof(_dt));
  _dt.year = 2025;
  _dt.month = 1;
  _dt.day = 1;
}

bool ClockDisplayPlugin::onLoad() {
  _rtcAvailable = hardware.isRTCAvailable();

  if (_rtcAvailable) {
    readRTC();
    Serial.printf("[clock] RTC: %s\n", getFullString().c_str());
  } else {
    Serial.println("[clock] RTC não disponível - usando hora simulada");
  }

  return true;
}

void ClockDisplayPlugin::update() {
  unsigned long now = millis();
  if (now - _lastUpdate < 1000)
    return; // Atualiza a cada 1s
  _lastUpdate = now;

  if (_rtcAvailable) {
    readRTC();
  } else {
    // Incrementa manualmente para simulação
    _dt.second++;
    if (_dt.second >= 60) {
      _dt.second = 0;
      _dt.minute++;
      if (_dt.minute >= 60) {
        _dt.minute = 0;
        _dt.hour++;
        if (_dt.hour >= 24) {
          _dt.hour = 0;
          _dt.day++;
          // Simplificado: não trata meses corretamente
        }
      }
    }
  }
}

void ClockDisplayPlugin::setDateTime(const DateTime &dt) {
  _dt = dt;
  if (_rtcAvailable) {
    writeRTC();
  }
}

void ClockDisplayPlugin::setFromUnix(uint32_t timestamp) {
  // Conversão simplificada de Unix timestamp
  // Para implementação completa, usar time.h
  time_t t = timestamp;
  struct tm *tm_info = localtime(&t);

  if (tm_info) {
    _dt.year = tm_info->tm_year + 1900;
    _dt.month = tm_info->tm_mon + 1;
    _dt.day = tm_info->tm_mday;
    _dt.hour = tm_info->tm_hour;
    _dt.minute = tm_info->tm_min;
    _dt.second = tm_info->tm_sec;
    _dt.weekday = tm_info->tm_wday;

    if (_rtcAvailable) {
      writeRTC();
    }
  }
}

String ClockDisplayPlugin::getTimeString() const {
  char buf[12];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", _dt.hour, _dt.minute,
           _dt.second);
  return String(buf);
}

String ClockDisplayPlugin::getDateString() const {
  char buf[12];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d", _dt.year, _dt.month, _dt.day);
  return String(buf);
}

String ClockDisplayPlugin::getFullString() const {
  char buf[24];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", _dt.year,
           _dt.month, _dt.day, _dt.hour, _dt.minute, _dt.second);
  return String(buf);
}

String ClockDisplayPlugin::getShortTime() const {
  char buf[8];
  snprintf(buf, sizeof(buf), "%02d:%02d", _dt.hour, _dt.minute);
  return String(buf);
}

void ClockDisplayPlugin::readRTC() {
  // Lê do hardware RTC
  hardware.getRTCDateTime(&_dt.year, &_dt.month, &_dt.day, &_dt.hour,
                          &_dt.minute, &_dt.second);
}

void ClockDisplayPlugin::writeRTC() {
  // Escreve no hardware RTC
  hardware.setRTCDateTime(_dt.year, _dt.month, _dt.day, _dt.hour, _dt.minute,
                          _dt.second);
}
