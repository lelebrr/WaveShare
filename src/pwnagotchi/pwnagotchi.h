#pragma once

/**
 * @file pwnagotchi.h
 * @brief Header da classe principal WavePwn
 */

#include "../hardware/ble_driver.h"
#include "../hardware/system_hardware.h"
#include "../hardware/wifi_driver.h"
#include "../web/web_server.h"
#include "../wifi/wps_attacks.h"
#include "dragon_mascot.h"
#include <Arduino.h>

class Pwnagotchi {
public:
  Pwnagotchi();

  void begin();
  void loop();

  // Status
  String getMoodString();
  bool isUIReady();

  // Controle de ataques WiFi
  void startDeauthAttack(const uint8_t *bssid, const char *ssid,
                         uint8_t channel);
  void startBeaconFlood();
  void startHandshakeCapture(const uint8_t *bssid, const char *ssid,
                             uint8_t channel);

  // Controle de ataques WPS
  void startWPSPixieDust(const uint8_t *bssid, const char *ssid,
                         uint8_t channel);
  void startWPSBruteforce(const uint8_t *bssid, const char *ssid,
                          uint8_t channel);
  void startWPSFlood(const uint8_t *bssid, const char *ssid, uint8_t channel);

  // Controle BLE
  void startBLESpam(BLESpamType type);
  void stopAllAttacks();

  // Funções públicas para voice assistant
  void performWiFiScan();
  void triggerVoiceCommand();

private:
  DragonMascot *_mascot;
  unsigned long _lastUpdate;
  unsigned long _lastScan;

  // Core Logic
  void updateLogic();
  void processInputs();
  void updateStatsDisplay();

  // States
  bool _isScanning;
  void checkScanResults();
};

// Instância global
extern Pwnagotchi pwn;
