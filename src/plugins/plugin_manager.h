#pragma once

/**
 * @file plugin_manager.h
 * @brief Gerenciador de plugins WavePwn
 *
 * Responsável por:
 * - Registrar plugins
 * - Dispatch de eventos para todos plugins
 * - Controle de estado (enable/disable)
 */

#include "plugin_base.h"
#include <vector>

#define MAX_PLUGINS 16

/**
 * @brief Gerenciador central de plugins
 */
class PluginManager {
public:
  PluginManager();

  /**
   * @brief Inicializa o gerenciador
   */
  void begin();

  /**
   * @brief Registra um plugin
   * @param plugin Ponteiro para o plugin
   * @return true se registrado com sucesso
   */
  bool registerPlugin(PluginBase *plugin);

  /**
   * @brief Remove um plugin pelo nome
   * @param name Nome do plugin
   */
  void unregisterPlugin(const char *name);

  /**
   * @brief Obtém plugin pelo nome
   * @param name Nome do plugin
   * @return Ponteiro para o plugin ou nullptr
   */
  PluginBase *getPlugin(const char *name);

  /**
   * @brief Obtém lista de plugins
   */
  const std::vector<PluginBase *> &getPlugins() const { return _plugins; }

  /**
   * @brief Atualiza todos os plugins (loop)
   */
  void update();

  // ========== Event Dispatchers ==========

  // WiFi events
  void dispatchWiFiUpdate(const PwnNetwork *networks, int count);
  void dispatchChannelHop(uint8_t new_channel);
  void dispatchHandshake(const HandshakeData &hs);
  void dispatchAssociation(const uint8_t *client_mac, const uint8_t *bssid);
  void dispatchDeauthentication(const uint8_t *client_mac,
                                const uint8_t *bssid);

  // Epoch events
  void dispatchEpochStart();
  void dispatchEpochEnd(const SessionStats &stats);

  // UI events
  void dispatchUISetup(lv_obj_t *parent);
  void dispatchUIUpdate(const UIState &state);

  // Internet events
  void dispatchInternetAvailable();
  void dispatchInternetLost();

  // Mood events
  void dispatchBored();
  void dispatchExcited();
  void dispatchSad();
  void dispatchLonely();
  void dispatchSleep();
  void dispatchAwake();

  // Stats
  int getPluginCount() const;
  int getEnabledCount() const;

private:
  std::vector<PluginBase *> _plugins;
  bool _initialized;
};

// Instância global
extern PluginManager pluginManager;

inline int PluginManager::getPluginCount() const { return _plugins.size(); }
