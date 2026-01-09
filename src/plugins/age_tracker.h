#pragma once

/**
 * @file age_tracker.h
 * @brief Plugin de idade do dispositivo (inspirado no age.py do Pwnagotchi)
 *
 * Rastreia:
 * - Tempo desde primeiro boot (idade)
 * - Uptime da sessão atual
 * - Total de horas de operação
 */

#include "plugin_base.h"
#include <Preferences.h>

class AgeTrackerPlugin : public PluginBase {
public:
  AgeTrackerPlugin();

  const char *getName() const override { return "age"; }
  const char *getAuthor() const override { return "WavePwn"; }
  const char *getVersion() const override { return "1.0.0"; }

  bool onLoad() override;
  void onUnload() override;
  void update() override;

  // Getters
  uint32_t getAgeDays() const { return _ageDays; }
  uint32_t getAgeHours() const { return _ageHours; }
  uint32_t getTotalHours() const { return _totalHours; }
  uint32_t getUptimeSeconds() const;
  uint32_t getBootCount() const { return _bootCount; }

  // Formatação
  String getFormattedAge() const;          // "15d 3h"
  String getFormattedUptime() const;       // "02:35:17"
  String getFormattedTotalRuntime() const; // "127h"

private:
  uint32_t _firstBootTime; // Unix timestamp do primeiro boot
  uint32_t _ageDays;
  uint32_t _ageHours;
  uint32_t _totalHours; // Total de horas operando
  uint32_t _bootCount;
  unsigned long _bootMillis; // millis() no boot
  unsigned long _lastSave;

  Preferences _prefs;

  void save();
  void load();
};

extern AgeTrackerPlugin agePlugin;
