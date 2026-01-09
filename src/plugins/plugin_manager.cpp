/**
 * @file plugin_manager.cpp
 * @brief Implementação do gerenciador de plugins
 */

#include "plugin_manager.h"
#include "../core/config_manager.h" // Added for isPluginEnabled

// Instância global
PluginManager pluginManager;

PluginManager::PluginManager() : _initialized(false) {
  _plugins.reserve(MAX_PLUGINS);
}

void PluginManager::begin() {
  Serial.println("[PluginManager] Inicializando...");

  // Chama onLoad em todos os plugins registrados
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      // Check Global Config / SD Config override
      if (!config_manager.isPluginEnabled(plugin->getName())) {
        Serial.printf("[PluginManager] Plugin %s DISABLED by config.\n",
                      plugin->getName());
        plugin->setEnabled(false);
        continue;
      }
      Serial.printf("[PluginManager] Carregando: %s v%s\n", plugin->getName(),
                    plugin->getVersion());

      if (!plugin->onLoad()) {
        Serial.printf("[PluginManager] Falha ao carregar: %s\n",
                      plugin->getName());
        plugin->setEnabled(false);
      }
    }
  }

  // Chama onReady após todos carregados
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onReady();
    }
  }

  _initialized = true;
  Serial.printf("[PluginManager] %d plugins carregados\n", getEnabledCount());
}

bool PluginManager::registerPlugin(PluginBase *plugin) {
  if (!plugin)
    return false;

  if (_plugins.size() >= MAX_PLUGINS) {
    Serial.println("[PluginManager] Limite de plugins atingido!");
    return false;
  }

  // Verifica duplicata
  for (auto *p : _plugins) {
    if (p && strcmp(p->getName(), plugin->getName()) == 0) {
      Serial.printf("[PluginManager] Plugin já registrado: %s\n",
                    plugin->getName());
      return false;
    }
  }

  _plugins.push_back(plugin);
  Serial.printf("[PluginManager] Registrado: %s\n", plugin->getName());

  // Se já inicializado, carrega imediatamente
  if (_initialized && plugin->isEnabled()) {
    if (plugin->onLoad()) {
      plugin->onReady();
    } else {
      plugin->setEnabled(false);
    }
  }

  return true;
}

void PluginManager::unregisterPlugin(const char *name) {
  for (auto it = _plugins.begin(); it != _plugins.end(); ++it) {
    if (*it && strcmp((*it)->getName(), name) == 0) {
      (*it)->onUnload();
      _plugins.erase(it);
      Serial.printf("[PluginManager] Removido: %s\n", name);
      return;
    }
  }
}

PluginBase *PluginManager::getPlugin(const char *name) {
  for (auto *plugin : _plugins) {
    if (plugin && strcmp(plugin->getName(), name) == 0) {
      return plugin;
    }
  }
  return nullptr;
}

void PluginManager::update() {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->update();
    }
  }
}

int PluginManager::getEnabledCount() const {
  int count = 0;
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled())
      count++;
  }
  return count;
}

// ========== Event Dispatchers ==========

void PluginManager::dispatchWiFiUpdate(const PwnNetwork *networks, int count) {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onWiFiUpdate(networks, count);
    }
  }
}

void PluginManager::dispatchChannelHop(uint8_t new_channel) {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onChannelHop(new_channel);
    }
  }
}

void PluginManager::dispatchHandshake(const HandshakeData &hs) {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onHandshake(hs);
    }
  }
}

void PluginManager::dispatchAssociation(const uint8_t *client_mac,
                                        const uint8_t *bssid) {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onAssociation(client_mac, bssid);
    }
  }
}

void PluginManager::dispatchDeauthentication(const uint8_t *client_mac,
                                             const uint8_t *bssid) {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onDeauthentication(client_mac, bssid);
    }
  }
}

void PluginManager::dispatchEpochStart() {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onEpochStart();
    }
  }
}

void PluginManager::dispatchEpochEnd(const SessionStats &stats) {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onEpochEnd(stats);
    }
  }
}

void PluginManager::dispatchUISetup(lv_obj_t *parent) {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onUISetup(parent);
    }
  }
}

void PluginManager::dispatchUIUpdate(const UIState &state) {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onUIUpdate(state);
    }
  }
}

void PluginManager::dispatchInternetAvailable() {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onInternetAvailable();
    }
  }
}

void PluginManager::dispatchInternetLost() {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onInternetLost();
    }
  }
}

void PluginManager::dispatchBored() {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onBored();
    }
  }
}

void PluginManager::dispatchExcited() {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onExcited();
    }
  }
}

void PluginManager::dispatchSad() {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onSad();
    }
  }
}

void PluginManager::dispatchLonely() {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onLonely();
    }
  }
}

void PluginManager::dispatchSleep() {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onSleep();
    }
  }
}

void PluginManager::dispatchAwake() {
  for (auto *plugin : _plugins) {
    if (plugin && plugin->isEnabled()) {
      plugin->onAwake();
    }
  }
}
