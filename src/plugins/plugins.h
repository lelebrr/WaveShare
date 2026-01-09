#pragma once

/**
 * @file plugins.h
 * @brief Header central para incluir todos os plugins
 */

#include "age_tracker.h"
#include "battery_monitor.h"
#include "clock_display.h"
#include "exp_system.h"
#include "internet_check.h"
#include "memtemp.h"
#include "plugin_manager.h"
#include "session_stats.h"


/**
 * @brief Registra todos os plugins padrão
 *
 * Chamar esta função no setup() após inicializar o sistema
 */
inline void registerDefaultPlugins() {
  Serial.println("[Plugins] Registrando plugins padrão...");

  pluginManager.registerPlugin(&memtempPlugin);
  pluginManager.registerPlugin(&expPlugin);
  pluginManager.registerPlugin(&agePlugin);
  pluginManager.registerPlugin(&batteryPlugin);
  pluginManager.registerPlugin(&sessionPlugin);
  pluginManager.registerPlugin(&clockPlugin);
  pluginManager.registerPlugin(&internetPlugin);

  Serial.printf("[Plugins] %d plugins registrados\n",
                pluginManager.getPluginCount());
}
