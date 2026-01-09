/**
 * @file battery_monitor.cpp
 * @brief Implementação do plugin de bateria
 */

#include "battery_monitor.h"
#include "../hardware/system_hardware.h"

BatteryMonitorPlugin batteryPlugin;

BatteryMonitorPlugin::BatteryMonitorPlugin()
    : _percent(100), _voltage(4200), _current(0), _charging(false),
      _usbConnected(false), _batteryPresent(true), _pmuAvailable(false),
      _lastUpdate(0), _currentAvg(0), _sampleIndex(0) {
  memset(_currentSamples, 0, sizeof(_currentSamples));
}

bool BatteryMonitorPlugin::onLoad() {
  // Verifica se hardware disponível
  _pmuAvailable = hardware.isPMUAvailable();

  if (!_pmuAvailable) {
    Serial.println("[battery] PMU não disponível - usando valores simulados");
  } else {
    Serial.println("[battery] PMU AXP2101 detectado");
  }

  update();
  return true;
}

void BatteryMonitorPlugin::update() {
  unsigned long now = millis();
  if (now - _lastUpdate < UPDATE_INTERVAL)
    return;
  _lastUpdate = now;

  if (_pmuAvailable) {
    // Lê do hardware real
    _percent = hardware.getBatteryPercent();
    _voltage = hardware.getBatteryVoltage();
    _current = hardware.getBatteryCurrent();
    _charging = hardware.isCharging();
    _usbConnected = hardware.isUSBConnected();
    _batteryPresent = _voltage > 2500; // Bateria presente se >2.5V

    // Atualiza média de corrente para estimativa
    _currentSamples[_sampleIndex] = _current;
    _sampleIndex = (_sampleIndex + 1) % 10;

    _currentAvg = 0;
    for (int i = 0; i < 10; i++) {
      _currentAvg += _currentSamples[i];
    }
    _currentAvg /= 10;
  } else {
    // Valores simulados para teste
    _percent = 75;
    _voltage = 3850;
    _current = -150; // Descarregando
    _charging = false;
    _usbConnected = false;
    _batteryPresent = true;
  }
}

String BatteryMonitorPlugin::getFormattedVoltage() const {
  char buf[8];
  snprintf(buf, sizeof(buf), "%.2fV", _voltage / 1000.0);
  return String(buf);
}

String BatteryMonitorPlugin::getFormattedCurrent() const {
  char buf[12];
  if (_current >= 0) {
    snprintf(buf, sizeof(buf), "+%dmA", _current);
  } else {
    snprintf(buf, sizeof(buf), "%dmA", _current);
  }
  return String(buf);
}

String BatteryMonitorPlugin::getStatusText() const {
  if (!_batteryPresent)
    return "No Bat";
  if (_charging) {
    if (_percent >= 100)
      return "Full";
    return "Charging";
  }
  return "Discharging";
}

char BatteryMonitorPlugin::getIcon() const {
  if (!_batteryPresent)
    return 'X';
  if (_charging)
    return '+';
  if (_percent > 75)
    return '#';
  if (_percent > 50)
    return '=';
  if (_percent > 25)
    return '-';
  return '!'; // Bateria baixa
}

uint16_t BatteryMonitorPlugin::getEstimatedTimeMinutes() const {
  if (!_batteryPresent || _currentAvg == 0)
    return 0;

  if (_charging && _currentAvg > 0) {
    // Tempo para carregar completamente
    uint8_t remaining = 100 - _percent;
    // Estimativa simples: assume capacidade de 500mAh
    // remaining% * 500mAh / current_mA * 60 min
    return (remaining * 500 * 60) / (100 * _currentAvg);
  } else if (!_charging && _currentAvg < 0) {
    // Tempo para descarregar
    // percent% * 500mAh / current_mA * 60 min
    return (_percent * 500 * 60) / (100 * (-_currentAvg));
  }

  return 0;
}
