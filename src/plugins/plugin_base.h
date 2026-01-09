#pragma once

/**
 * @file plugin_base.h
 * @brief Interface base para plugins WavePwn (inspirado no Pwnagotchi)
 *
 * Sistema de plugins com callbacks para eventos:
 * - on_ready: Plugin carregado
 * - on_wifi_update: Nova rede detectada
 * - on_handshake: Handshake capturado
 * - on_ui_update: Atualização de UI
 * - on_epoch: Nova epoch (ciclo completo de scan)
 */

#include <Arduino.h>
#include <functional>
#include <lvgl.h>

// Forward declarations
struct PwnNetwork;
struct HandshakeData;
struct UIState;

/**
 * @brief Dados de uma rede WiFi (para plugins)
 * @note Renomeado para PwnNetwork para evitar conflito com WiFiNetwork em
 * wifi_driver.h
 */
struct PwnNetwork {
  char ssid[33];
  uint8_t bssid[6];
  int8_t rssi;
  uint8_t channel;
  uint8_t encryption; // 0=open, 1=WEP, 2=WPA, 3=WPA2, 4=WPA3
  bool wps_enabled;
};

/**
 * @brief Dados de um handshake capturado
 */
struct HandshakeData {
  char ssid[33];
  uint8_t bssid[6];
  uint8_t client_mac[6];
  uint32_t timestamp;
  size_t pcap_size;
  bool pmkid;
};

/**
 * @brief Estado atual da UI
 */
struct UIState {
  uint8_t current_channel;
  uint16_t visible_aps;
  uint16_t total_aps;
  uint32_t uptime_seconds;
  uint16_t handshakes_session;
  uint16_t handshakes_total;
  char last_ssid[33];
  uint8_t mode; // 0=MANU, 1=AUTO, 2=AI
};

/**
 * @brief Estatísticas da sessão
 */
struct SessionStats {
  uint32_t start_time;
  uint16_t networks_found;
  uint16_t handshakes_captured;
  uint16_t deauths_sent;
  uint16_t beacons_sent;
  uint16_t ble_devices_found;
  uint32_t packets_sniffed;
};

/**
 * @brief Interface base para plugins
 */
class PluginBase {
public:
  virtual ~PluginBase() = default;

  // Identificação
  virtual const char *getName() const = 0;
  virtual const char *getAuthor() const = 0;
  virtual const char *getVersion() const = 0;

  // Lifecycle
  virtual bool onLoad() { return true; }
  virtual void onUnload() {}
  virtual void onReady() {}

  // Update (chamado no loop)
  virtual void update() {}

  // WiFi events
  virtual void onWiFiUpdate(const PwnNetwork *networks, int count) {}
  virtual void onChannelHop(uint8_t new_channel) {}
  virtual void onHandshake(const HandshakeData &hs) {}
  virtual void onAssociation(const uint8_t *client_mac, const uint8_t *bssid) {}
  virtual void onDeauthentication(const uint8_t *client_mac,
                                  const uint8_t *bssid) {}

  // Epoch events (ciclo completo de scan)
  virtual void onEpochStart() {}
  virtual void onEpochEnd(const SessionStats &stats) {}

  // UI events
  virtual void onUISetup(lv_obj_t *parent) {}
  virtual void onUIUpdate(const UIState &state) {}

  // Internet events
  virtual void onInternetAvailable() {}
  virtual void onInternetLost() {}

  // Mood events (para mascote)
  virtual void onBored() {}
  virtual void onExcited() {}
  virtual void onSad() {}
  virtual void onLonely() {}
  virtual void onSleep() {}
  virtual void onAwake() {}

  // Estado do plugin
  bool isEnabled() const { return _enabled; }
  void setEnabled(bool en) { _enabled = en; }

protected:
  bool _enabled = true;
};
