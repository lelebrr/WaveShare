#pragma once

/**
 * @file internet_check.h
 * @brief Plugin de verificação de conexão internet (inspirado no
 * internet-connection.py)
 */

#include "plugin_base.h"

class InternetCheckPlugin : public PluginBase {
public:
  InternetCheckPlugin();

  const char *getName() const override { return "internet"; }
  const char *getAuthor() const override { return "WavePwn"; }
  const char *getVersion() const override { return "1.0.0"; }

  bool onLoad() override;
  void update() override;

  // Status
  bool isConnected() const { return _connected; }
  int getLatency() const { return _latency; } // ms, -1 se offline
  const char *getPublicIP() const { return _publicIP; }

  // Força verificação imediata
  void check();

private:
  bool _connected;
  int _latency;
  char _publicIP[16];
  unsigned long _lastCheck;

  static const unsigned long CHECK_INTERVAL = 30000; // 30 segundos

  bool pingTest();
  void getExternalIP();
};

extern InternetCheckPlugin internetPlugin;
