#pragma once

/**
 * @file config_manager.h
 * @brief Gerenciador de configurações com persistência NVS
 */

#include "globals.h"
#include <Preferences.h>
#include <vector>

class ConfigManager {
public:
  ConfigManager();
  void begin();
  void load();
  void save();
  void reset();

  // Save/load por categoria para otimizar NVS writes
  void loadDisplaySettings();
  void saveDisplaySettings();

  void loadNetworkSettings();
  void saveNetworkSettings();

  void loadAttackSettings();
  void saveAttackSettings();

  void loadSystemSettings();
  void saveSystemSettings();

  void loadDragonSettings();
  void saveDragonSettings();

  // AI
  void loadAiSettings();
  void saveAiSettings();

  // Runtime
  void loadRuntimeDefaults();

  // SD Config
  void loadFromSD();

  // Plugin Management
  bool isPluginEnabled(const char *name);

private:
  Preferences prefs;
  std::vector<String> _disabled_plugins;
};

extern ConfigManager config_manager;
