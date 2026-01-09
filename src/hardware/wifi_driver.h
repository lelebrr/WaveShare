#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <vector>

// Renamed to avoid conflicts with esp_wifi_types.h
enum WiFiDriverMode {
  DRIVER_WIFI_MODE_OFF,
  DRIVER_WIFI_MODE_STA,
  DRIVER_WIFI_MODE_AP,
  DRIVER_WIFI_MODE_AP_STA,
  DRIVER_WIFI_MODE_MONITOR
};

struct NetworkInfo {
  int id;
  char ssid[33];
  uint8_t bssid[6];
  int rssi;
  int channel;
  int encryptionType;
};

// Alias para compatibilidade
typedef NetworkInfo WiFiNetwork;

// Callback for monitor mode packets
typedef void (*MonitorCallback)(void *buf, wifi_promiscuous_pkt_type_t type);

class WiFiDriver {
public:
  WiFiDriver();

  void begin();
  bool setMode(WiFiDriverMode mode);
  WiFiDriverMode getMode();

  // Station
  bool connect(const char *ssid, const char *password);
  bool isConnected();

  // AP
  bool startAP(const char *ssid, const char *password = NULL,
               uint8_t channel = 1);
  void stopAP();

  // Monitor / Scan
  int scanNetworks(bool async = false);
  int getNetworkCount();
  NetworkInfo getNetwork(int index);

  void setMonitorCallback(MonitorCallback cb);
  void sendRawPacket(uint8_t *frame, size_t len);

  // Channel Hopping
  void startChannelHopping(int delayMs = 200);
  void stopChannelHopping();
  void update(); // Call in loop

private:
  WiFiDriverMode _currentMode;
  MonitorCallback _monitorCb;
  bool _hoppingActive;
  unsigned long _lastHop;
  int _currentChannel;
};

// GLOBAL INSTANCE
extern WiFiDriver wifi_driver;
