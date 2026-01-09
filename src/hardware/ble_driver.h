#pragma once

#include "ble_payloads.h"
#include "ble_payloads_extended.h"
#include <Arduino.h>
#include <BLEAdvertisedDevice.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#include <map>
#include <string>
#include <vector>

/**
 * @brief Tipos de BLE spam e ataques disponíveis
 */
enum BLESpamType {
  BLE_SPAM_OFF,
  BLE_SPAM_APPLE,         // Apple AirPods/AirTag/Continuity (15 payloads)
  BLE_SPAM_SAMSUNG,       // Samsung Galaxy Buds/Watch (4 payloads)
  BLE_SPAM_WINDOWS,       // Windows Swift Pair (3 payloads)
  BLE_SPAM_GOOGLE,        // Google Fast Pair (9 payloads)
  BLE_SPAM_KITCHEN_SINK,  // All payloads cycled (Chaos)
  BLE_SPAM_RICKROLL,      // Rick Astley Speaker
  BLE_SPAM_AIRTAG_FLOOD,  // Multiple AirTags (FindMy Flood)
  BLE_SPAM_AIRTAG_MOVING, // Simulates AirTag approaching (tracker alert)
  BLE_SPAM_IBEACON_FLOOD, // iBeacon spam
  BLE_SPAM_EDDYSTONE,     // Eddystone URL/UID spam
  BLE_SPAM_DYNAMIC_NAME,  // Name changes every 200ms
  BLE_SPAM_EASTER_EGG,    // Fun names: "Free iPhone 16", "FBI Van", etc.
  BLE_SPAM_SILENT,        // Same as chaos but no sound/visual feedback
  BLE_SPAM_EXTREME_DOOS   // Connection Request Flood (DoS) 125 pkts/s
};

/**
 * @brief Dados de dispositivo BLE escaneado
 */
struct BLEDeviceData {
  String name;
  String address;
  int rssi;
  int manufacturerId;
  String manufacturerData;
};

/**
 * @brief Driver BLE com suporte a scan, spam e ataques avançados
 */
class BLEDriver {
public:
  BLEDriver();
  void begin(String deviceName);
  void stop();

  // Scan
  void startScan(int duration);
  void startScanAsync(int duration);
  void updateScan();
  std::vector<BLEDeviceData> getScanResults();

  // Spam / Attacks
  void startSpam(BLESpamType type);
  void stopSpam();
  void updateSpam(); // Rotaciona payloads ou executa lógica por frame

  // Utilities
  bool isSpamming();
  uint32_t getPacketsSpammed();
  String
  getAttackStatus(); // Retorna status para UI (ex: "Apple Spam: 450 pkts")

private:
  BLEScan *_pBLEScan;
  bool _scanning;
  std::vector<BLEDeviceData> _foundDevices;

  bool _spamming;
  BLESpamType _spamType;
  unsigned long _lastSpamFrame;
  BLEAdvertising *_pAdvertising;

  // Gerenciamento de spam
  uint32_t _spamIndex;
  uint32_t _packetsSpammed;
  bool _initialized;

  // Helpers internos
  void runAppleSpam();
  void runWindowsSpam();
  void runSamsungSpam();
  void runGoogleSpam();
  void runRickRoll();
  void runGenericChaos();
  void runAirtagFlood();    // FindMy Flood (10 AirTags)
  void runAirtagMoving();   // Simula AirTag se aproximando
  void runIBeaconFlood();   // iBeacon spam
  void runEddystoneFlood(); // Eddystone URL/UID
  void runDynamicName();    // Nome muda a cada 200ms
  void runEasterEgg();      // Nomes engraçados

  bool _silentMode;            // Modo silencioso (sem feedback)
  int8_t _airtagRssiSimulated; // RSSI simulado para AirTag moving
};

// INSTÂNCIA GLOBAL
extern BLEDriver ble_driver;
