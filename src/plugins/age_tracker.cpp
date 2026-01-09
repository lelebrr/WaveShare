/**
 * @file age_tracker.cpp
 * @brief Implementação do plugin age tracker
 */

#include "age_tracker.h"
#include <time.h>

AgeTrackerPlugin agePlugin;

AgeTrackerPlugin::AgeTrackerPlugin()
    : _firstBootTime(0), _ageDays(0), _ageHours(0), _totalHours(0),
      _bootCount(0), _bootMillis(0), _lastSave(0) {}

bool AgeTrackerPlugin::onLoad() {
  _bootMillis = millis();
  load();

  // Incrementa contador de boots
  _bootCount++;

  // Se primeiro boot, salva timestamp atual
  if (_firstBootTime == 0) {
    _firstBootTime = time(nullptr);
    if (_firstBootTime < 1000000000) {
      // RTC não configurado, usa contador interno
      _firstBootTime = 0;
    }
  }

  save();

  Serial.printf("[age] Boot #%u, Idade: %s, Total runtime: %uh\n", _bootCount,
                getFormattedAge().c_str(), _totalHours);

  return true;
}

void AgeTrackerPlugin::onUnload() {
  // Atualiza total de horas antes de desligar
  uint32_t sessionHours = getUptimeSeconds() / 3600;
  _totalHours += sessionHours;
  save();
}

void AgeTrackerPlugin::update() {
  unsigned long now = millis();

  // Salva a cada hora
  if (now - _lastSave >= 3600000) {
    _lastSave = now;
    _totalHours++;
    save();
  }

  // Atualiza idade
  if (_firstBootTime > 0) {
    uint32_t currentTime = time(nullptr);
    if (currentTime > _firstBootTime) {
      uint32_t ageSeconds = currentTime - _firstBootTime;
      _ageDays = ageSeconds / 86400;
      _ageHours = (ageSeconds % 86400) / 3600;
    }
  } else {
    // Usa total de horas como idade aproximada
    _ageDays = _totalHours / 24;
    _ageHours = _totalHours % 24;
  }
}

uint32_t AgeTrackerPlugin::getUptimeSeconds() const {
  return (millis() - _bootMillis) / 1000;
}

String AgeTrackerPlugin::getFormattedAge() const {
  if (_ageDays > 0) {
    return String(_ageDays) + "d " + String(_ageHours) + "h";
  }
  return String(_ageHours) + "h";
}

String AgeTrackerPlugin::getFormattedUptime() const {
  uint32_t seconds = getUptimeSeconds();
  uint32_t hours = seconds / 3600;
  uint32_t minutes = (seconds % 3600) / 60;
  uint32_t secs = seconds % 60;

  char buf[16];
  snprintf(buf, sizeof(buf), "%02u:%02u:%02u", hours, minutes, secs);
  return String(buf);
}

String AgeTrackerPlugin::getFormattedTotalRuntime() const {
  uint32_t current = _totalHours + (getUptimeSeconds() / 3600);
  return String(current) + "h";
}

void AgeTrackerPlugin::save() {
  if (!_prefs.begin("wavepwn_age", false))
    return;

  _prefs.putUInt("first_boot", _firstBootTime);
  _prefs.putUInt("total_hours", _totalHours);
  _prefs.putUInt("boot_count", _bootCount);
  _prefs.end();
}

void AgeTrackerPlugin::load() {
  if (!_prefs.begin("wavepwn_age", true))
    return;

  _firstBootTime = _prefs.getUInt("first_boot", 0);
  _totalHours = _prefs.getUInt("total_hours", 0);
  _bootCount = _prefs.getUInt("boot_count", 0);
  _prefs.end();
}
