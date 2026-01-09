#include "wifi_driver.h"

// GLOBAL INSTANCE DEFINITION
WiFiDriver wifi_driver;

WiFiDriver::WiFiDriver()
    : _currentMode(DRIVER_WIFI_MODE_OFF), _monitorCb(nullptr),
      _hoppingActive(false), _lastHop(0), _currentChannel(1) {}

void WiFiDriver::begin() {
  Serial.println("[WiFi] Inicializando driver...");

  // Aguarda TCP/IP stack estar pronto
  delay(100);

  // Inicializa WiFi com modo seguro
  WiFi.disconnect(true); // Desconecta e limpa estado
  delay(50);
  WiFi.mode(WIFI_OFF); // Desliga primeiro
  delay(50);

  _currentMode = DRIVER_WIFI_MODE_OFF;
  WiFi.persistent(false);

  Serial.println("[WiFi] Driver pronto");
}

bool WiFiDriver::setMode(WiFiDriverMode mode) {
  if (mode == _currentMode)
    return true;

  switch (mode) {
  case DRIVER_WIFI_MODE_OFF:
    WiFi.mode(WIFI_OFF);
    break;
  case DRIVER_WIFI_MODE_STA:
    WiFi.mode(WIFI_STA);
    break;
  case DRIVER_WIFI_MODE_AP:
    WiFi.mode(WIFI_AP);
    break;
  case DRIVER_WIFI_MODE_AP_STA:
    WiFi.mode(WIFI_AP_STA);
    break;
  case DRIVER_WIFI_MODE_MONITOR:
    // Monitor mode often requires STA mode + promiscuous
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    esp_wifi_set_promiscuous(true);
    break;
  }
  _currentMode = mode;
  return true;
}

WiFiDriverMode WiFiDriver::getMode() { return _currentMode; }

bool WiFiDriver::connect(const char *ssid, const char *password) {
  setMode(DRIVER_WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(100);
    retries++;
  }
  return WiFi.status() == WL_CONNECTED;
}

bool WiFiDriver::isConnected() { return WiFi.status() == WL_CONNECTED; }

bool WiFiDriver::startAP(const char *ssid, const char *password,
                         uint8_t channel) {
  setMode(DRIVER_WIFI_MODE_AP);

  // Configura IP fixo para o AP
  IPAddress local_IP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);

  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("[WiFi] AP Config Failed!");
    return false;
  }

  // Desabilita power save para evitar queda de conexão
  WiFi.setSleep(false);

  // Inicia AP com limite de 4 clientes para economizar RAM
  return WiFi.softAP(ssid, password, channel, 0, 4);
}

void WiFiDriver::stopAP() {
  WiFi.softAPdisconnect(true);
  setMode(DRIVER_WIFI_MODE_OFF);
}
WiFiNetwork WiFiDriver::getNetwork(int index) {
  WiFiNetwork info;
  if (index < 0 || index >= WiFi.scanComplete()) {
    memset(&info, 0, sizeof(info));
    return info;
  }

  strncpy(info.ssid, WiFi.SSID(index).c_str(), 32);
  info.ssid[32] = 0;

  info.rssi = WiFi.RSSI(index);
  info.channel = WiFi.channel(index);
  info.encryptionType = WiFi.encryptionType(index);

  // FIX FOR BSSID
  uint8_t *b = WiFi.BSSID(index);
  if (b) {
    memcpy(info.bssid, b, 6);
  } else {
    memset(info.bssid, 0, 6);
  }

  return info;
}

void WiFiDriver::setMonitorCallback(MonitorCallback cb) {
  _monitorCb = cb;
  if (_currentMode == DRIVER_WIFI_MODE_MONITOR) {
    esp_wifi_set_promiscuous_rx_cb((wifi_promiscuous_cb_t)cb);
  }
}

void WiFiDriver::sendRawPacket(uint8_t *frame, size_t len) {
  if (_currentMode == DRIVER_WIFI_MODE_MONITOR) {
    esp_wifi_80211_tx((wifi_interface_t)WIFI_IF_STA, frame, len, true);
  }
}

void WiFiDriver::startChannelHopping(int delayMs) {
  _hoppingActive = true;
  _lastHop = millis();
}

void WiFiDriver::stopChannelHopping() { _hoppingActive = false; }

int WiFiDriver::scanNetworks(bool async) {
  if (_currentMode == DRIVER_WIFI_MODE_OFF) {
    setMode(DRIVER_WIFI_MODE_STA);
  }
  return WiFi.scanNetworks(async);
}

int WiFiDriver::getNetworkCount() { return WiFi.scanComplete(); }

void WiFiDriver::update() {
  if (_hoppingActive && (millis() - _lastHop > 200)) {
    _lastHop = millis();

    // Intelligent hopping sequence: 1, 6, 11 are prioritized
    static const uint8_t hop_sequence[] = {1, 6,  11, 2, 7,  12, 3,
                                           8, 13, 4,  9, 14, 5,  10};
    static int hop_idx = 0;

    _currentChannel = hop_sequence[hop_idx];
    hop_idx++;
    if (hop_idx >= sizeof(hop_sequence))
      hop_idx = 0;

    esp_wifi_set_channel(_currentChannel, WIFI_SECOND_CHAN_NONE);
  }
}
