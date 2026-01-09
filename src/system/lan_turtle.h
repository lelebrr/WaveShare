#pragma once
#include <Arduino.h>
#include <WiFi.h>

// Definição do serviço Lan Turtle
class LanTurtle {
public:
  LanTurtle();
  void begin();
  void update();

  // Comandos de controle
  void start(const char *ssid, const char *password);
  void stop();

  bool isActive() const { return _active; }
  String getIP() const;

private:
  bool _active;
  bool _connected;
  String _targetSSID;
  String _targetPass;
  uint32_t _lastCheck;

  // Servidor Telnet simples para Shell Remoto
  WiFiServer _telnetServer;
  WiFiClient _telnetClient;

  void handleTelnet();
  void sendDiscordNotification();
};

extern LanTurtle lanTurtle;
