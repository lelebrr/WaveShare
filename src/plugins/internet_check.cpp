/**
 * @file internet_check.cpp
 * @brief Implementação da verificação de internet
 */

#include "internet_check.h"
#include <HTTPClient.h>
#include <WiFi.h>


InternetCheckPlugin internetPlugin;

InternetCheckPlugin::InternetCheckPlugin()
    : _connected(false), _latency(-1), _lastCheck(0) {
  memset(_publicIP, 0, sizeof(_publicIP));
  strcpy(_publicIP, "0.0.0.0");
}

bool InternetCheckPlugin::onLoad() {
  Serial.println("[internet] Plugin carregado");
  return true;
}

void InternetCheckPlugin::update() {
  unsigned long now = millis();
  if (now - _lastCheck < CHECK_INTERVAL)
    return;
  _lastCheck = now;

  // Só verifica se conectado a uma rede WiFi
  if (WiFi.status() != WL_CONNECTED) {
    _connected = false;
    _latency = -1;
    return;
  }

  check();
}

void InternetCheckPlugin::check() {
  if (!pingTest()) {
    _connected = false;
    _latency = -1;
    Serial.println("[internet] Offline");
    return;
  }

  _connected = true;
  getExternalIP();
  Serial.printf("[internet] Online, latency: %dms, IP: %s\n", _latency,
                _publicIP);
}

bool InternetCheckPlugin::pingTest() {
  // Usa HTTP GET para simular ping
  HTTPClient http;
  http.setTimeout(5000);

  unsigned long start = millis();

  // Tenta acessar um serviço rápido
  http.begin("http://captive.apple.com/hotspot-detect.html");
  int httpCode = http.GET();

  if (httpCode > 0) {
    _latency = millis() - start;
    http.end();
    return true;
  }

  http.end();
  return false;
}

void InternetCheckPlugin::getExternalIP() {
  HTTPClient http;
  http.setTimeout(5000);
  http.begin("http://api.ipify.org");

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    String ip = http.getString();
    ip.trim();
    if (ip.length() < sizeof(_publicIP)) {
      strcpy(_publicIP, ip.c_str());
    }
  }

  http.end();
}
