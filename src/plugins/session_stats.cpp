/**
 * @file session_stats.cpp
 * @brief Implementação do plugin de estatísticas
 */

#include "session_stats.h"

SessionStatsPlugin sessionPlugin;

SessionStatsPlugin::SessionStatsPlugin() : _lastNetworkCount(0) {
  memset(&_stats, 0, sizeof(_stats));
}

bool SessionStatsPlugin::onLoad() {
  reset();
  return true;
}

void SessionStatsPlugin::onReady() {
  Serial.println("[session-stats] Iniciando nova sessão");
}

void SessionStatsPlugin::onWiFiUpdate(const PwnNetwork *networks, int count) {
  if (count > _lastNetworkCount) {
    _stats.networks_found += (count - _lastNetworkCount);
  }
  _lastNetworkCount = count;
}

void SessionStatsPlugin::onHandshake(const HandshakeData &hs) {
  _stats.handshakes_captured++;
  Serial.printf("[session-stats] Total handshakes: %d\n",
                _stats.handshakes_captured);
}

void SessionStatsPlugin::onEpochEnd(const SessionStats &stats) {
  // Pode ser usado para logging detalhado
}

void SessionStatsPlugin::addDeauthsSent(uint16_t count) {
  _stats.deauths_sent += count;
}

void SessionStatsPlugin::addBeaconsSent(uint16_t count) {
  _stats.beacons_sent += count;
}

void SessionStatsPlugin::addBLEDevice() { _stats.ble_devices_found++; }

void SessionStatsPlugin::addPacketsSniffed(uint32_t count) {
  _stats.packets_sniffed += count;
}

uint32_t SessionStatsPlugin::getSessionDuration() const {
  return (millis() - _stats.start_time) / 1000;
}

String SessionStatsPlugin::getSummary() const {
  char buf[128];
  uint32_t duration = getSessionDuration();
  uint32_t hours = duration / 3600;
  uint32_t mins = (duration % 3600) / 60;

  snprintf(buf, sizeof(buf),
           "Session: %02u:%02u | Nets: %u | HS: %u | Pkts: %u", hours, mins,
           _stats.networks_found, _stats.handshakes_captured,
           _stats.packets_sniffed);

  return String(buf);
}

void SessionStatsPlugin::reset() {
  memset(&_stats, 0, sizeof(_stats));
  _stats.start_time = millis();
  _lastNetworkCount = 0;
  Serial.println("[session-stats] Estatísticas resetadas");
}
