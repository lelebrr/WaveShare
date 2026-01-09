#pragma once

/**
 * @file session_stats.h
 * @brief Plugin de estatísticas da sessão (inspirado no session-stats.py)
 *
 * Rastreia estatísticas da sessão atual:
 * - Redes encontradas
 * - Handshakes capturados
 * - Deauths enviados
 * - Beacons enviados
 * - Dispositivos BLE
 */

#include "plugin_base.h"

class SessionStatsPlugin : public PluginBase {
public:
  SessionStatsPlugin();

  const char *getName() const override { return "session-stats"; }
  const char *getAuthor() const override { return "WavePwn"; }
  const char *getVersion() const override { return "1.0.0"; }

  bool onLoad() override;
  void onReady() override;

  // Callbacks de eventos
  void onWiFiUpdate(const PwnNetwork *networks, int count) override;
  void onHandshake(const HandshakeData &hs) override;
  void onEpochEnd(const SessionStats &stats) override;

  // Incrementadores (chamados pelo sistema)
  void addDeauthsSent(uint16_t count);
  void addBeaconsSent(uint16_t count);
  void addBLEDevice();
  void addPacketsSniffed(uint32_t count);

  // Getters
  const SessionStats &getStats() const { return _stats; }
  uint32_t getSessionDuration() const; // Segundos desde início

  // Formatação
  String getSummary() const;

  // Reset
  void reset();

private:
  SessionStats _stats;
  uint16_t _lastNetworkCount;
};

extern SessionStatsPlugin sessionPlugin;
