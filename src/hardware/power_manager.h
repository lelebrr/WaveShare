/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Power Manager (Otimizações de Performance)
 * Gerenciamento de energia e economia de bateria
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <driver/rtc_io.h>
#include <esp_pm.h>
#include <esp_sleep.h>
#include <esp_wifi.h>


// Modos de energia
enum PowerMode : uint8_t {
  POWER_FULL = 0,  // Performance máxima (240MHz, WiFi full)
  POWER_BALANCED,  // Balanceado (160MHz, WiFi PS)
  POWER_ECO,       // Economia (80MHz, WiFi modem sleep)
  POWER_ULTRA_ECO, // Ultra economia (40MHz, light sleep)
  POWER_DEEP_SLEEP // Deep sleep (acorda por touch/timer)
};

// Configurações de economia
struct PowerConfig {
  PowerMode mode;
  uint32_t displayTimeout;   // Segundos para desligar display
  uint32_t deepSleepTimeout; // Segundos para deep sleep
  uint8_t brightness;        // 0-100%
  bool wifiPowerSave;        // WiFi power save
  bool bleSuspended;         // BLE suspenso quando não usado
  bool cpuThrottling;        // Reduzir CPU quando idle
};

class PowerManager {
public:
  PowerManager();

  void begin();
  void update();

  // Modo de energia
  void setMode(PowerMode mode);
  PowerMode getMode() const { return _config.mode; }

  // Configurações
  PowerConfig &getConfig() { return _config; }
  void saveConfig();
  void loadConfig();

  // Display
  void displayOn();
  void displayOff();
  void setDisplayTimeout(uint32_t seconds);
  bool isDisplayOn() const { return _displayOn; }

  // Sleep
  void enterLightSleep(uint32_t ms);
  void enterDeepSleep(uint32_t seconds);
  void scheduleWakeup(uint32_t seconds);

  // CPU
  void setCPUFrequency(uint32_t mhz);
  uint32_t getCPUFrequency() const;

  // WiFi power
  void setWiFiPowerSave(bool enable);

  // Estatísticas
  uint32_t getBatteryPercent() const;
  float getEstimatedRuntime() const; // Horas restantes

  // Activity tracking
  void registerActivity();
  uint32_t getIdleTime() const;

private:
  PowerConfig _config;
  bool _displayOn;
  uint32_t _lastActivity;
  uint32_t _displayOffTime;

  void applyPowerMode();
};

extern PowerManager power_manager;

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO
// ═══════════════════════════════════════════════════════════════════════════

PowerManager power_manager;

PowerManager::PowerManager() {
  _displayOn = true;
  _lastActivity = 0;
  _displayOffTime = 0;

  // Defaults
  _config.mode = POWER_BALANCED;
  _config.displayTimeout = 30;
  _config.deepSleepTimeout = 300;
  _config.brightness = 80;
  _config.wifiPowerSave = true;
  _config.bleSuspended = false;
  _config.cpuThrottling = true;
}

void PowerManager::begin() {
  loadConfig();
  _lastActivity = millis();
  applyPowerMode();
  Serial.println("[Power] Manager inicializado");
}

void PowerManager::update() {
  uint32_t now = millis();
  uint32_t idle = getIdleTime();

  // Auto display off
  if (_displayOn && _config.displayTimeout > 0 &&
      idle > _config.displayTimeout * 1000) {
    displayOff();
  }

  // Auto deep sleep (se configurado)
  if (_config.deepSleepTimeout > 0 && idle > _config.deepSleepTimeout * 1000) {
    if (_config.mode >= POWER_ECO) {
      enterDeepSleep(_config.deepSleepTimeout);
    }
  }

  // CPU throttling quando idle
  if (_config.cpuThrottling && idle > 5000) {
    if (getCPUFrequency() > 80) {
      setCPUFrequency(80);
    }
  }
}

void PowerManager::setMode(PowerMode mode) {
  _config.mode = mode;
  applyPowerMode();
  saveConfig();

  const char *modeNames[] = {"FULL", "BALANCED", "ECO", "ULTRA_ECO",
                             "DEEP_SLEEP"};
  Serial.printf("[Power] Modo: %s\n", modeNames[mode]);
}

void PowerManager::applyPowerMode() {
  switch (_config.mode) {
  case POWER_FULL:
    setCPUFrequency(240);
    setWiFiPowerSave(false);
    _config.displayTimeout = 0; // Nunca desliga
    break;

  case POWER_BALANCED:
    setCPUFrequency(160);
    setWiFiPowerSave(true);
    _config.displayTimeout = 30;
    break;

  case POWER_ECO:
    setCPUFrequency(80);
    setWiFiPowerSave(true);
    _config.displayTimeout = 15;
    break;

  case POWER_ULTRA_ECO:
    setCPUFrequency(40);
    setWiFiPowerSave(true);
    _config.displayTimeout = 10;
    break;

  case POWER_DEEP_SLEEP:
    enterDeepSleep(0); // Imediato
    break;
  }
}

void PowerManager::displayOn() {
  if (!_displayOn) {
    _displayOn = true;
    // TODO: Ligar backlight via hardware
    Serial.println("[Power] Display ON");
  }
  registerActivity();
}

void PowerManager::displayOff() {
  if (_displayOn) {
    _displayOn = false;
    _displayOffTime = millis();
    // TODO: Desligar backlight via hardware
    Serial.println("[Power] Display OFF");
  }
}

void PowerManager::setDisplayTimeout(uint32_t seconds) {
  _config.displayTimeout = seconds;
  saveConfig();
}

void PowerManager::enterLightSleep(uint32_t ms) {
  Serial.printf("[Power] Entrando em light sleep por %d ms\n", ms);

  esp_sleep_enable_timer_wakeup(ms * 1000);
  esp_light_sleep_start();

  registerActivity();
  Serial.println("[Power] Acordou de light sleep");
}

void PowerManager::enterDeepSleep(uint32_t seconds) {
  Serial.printf("[Power] Entrando em deep sleep por %d s\n", seconds);

  // Salva estado antes de dormir
  saveConfig();

  // Configura wakeup
  if (seconds > 0) {
    esp_sleep_enable_timer_wakeup(seconds * 1000000ULL);
  }

  // Touch wakeup (GPIO)
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_XX, 0);

  esp_deep_sleep_start();
}

void PowerManager::scheduleWakeup(uint32_t seconds) {
  esp_sleep_enable_timer_wakeup(seconds * 1000000ULL);
}

void PowerManager::setCPUFrequency(uint32_t mhz) {
  // ESP32-S3 suporta: 40, 80, 160, 240 MHz
  mhz = constrain(mhz, 40, 240);

  esp_pm_config_esp32s3_t pm_config = {.max_freq_mhz = (int)mhz,
                                       .min_freq_mhz = 40,
                                       .light_sleep_enable =
                                           (_config.mode >= POWER_ECO)};

  esp_pm_configure(&pm_config);
}

uint32_t PowerManager::getCPUFrequency() const { return getCpuFrequencyMhz(); }

void PowerManager::setWiFiPowerSave(bool enable) {
  _config.wifiPowerSave = enable;

  if (enable) {
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
  } else {
    esp_wifi_set_ps(WIFI_PS_NONE);
  }
}

uint32_t PowerManager::getBatteryPercent() const {
  // TODO: Ler do AXP2101
  return 75;
}

float PowerManager::getEstimatedRuntime() const {
  // Estimativa baseada no modo e bateria
  uint32_t battery = getBatteryPercent();
  float hoursPerPercent = 0.0f;

  switch (_config.mode) {
  case POWER_FULL:
    hoursPerPercent = 0.03f;
    break; // ~3h
  case POWER_BALANCED:
    hoursPerPercent = 0.05f;
    break; // ~5h
  case POWER_ECO:
    hoursPerPercent = 0.10f;
    break; // ~10h
  case POWER_ULTRA_ECO:
    hoursPerPercent = 0.20f;
    break; // ~20h
  default:
    hoursPerPercent = 0.05f;
    break;
  }

  return battery * hoursPerPercent;
}

void PowerManager::registerActivity() {
  _lastActivity = millis();

  // Religa display se estava desligado
  if (!_displayOn) {
    displayOn();
  }

  // Restaura CPU se estava throttled
  if (_config.cpuThrottling && getCPUFrequency() < 160) {
    setCPUFrequency(_config.mode == POWER_FULL ? 240 : 160);
  }
}

uint32_t PowerManager::getIdleTime() const { return millis() - _lastActivity; }

void PowerManager::saveConfig() {
  // TODO: Salvar em NVS
}

void PowerManager::loadConfig() {
  // TODO: Carregar de NVS
}
