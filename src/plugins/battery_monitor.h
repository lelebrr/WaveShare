#pragma once

/**
 * @file battery_monitor.h
 * @brief Plugin de monitoramento de bateria (inspirado no ups_lite.py)
 *
 * Usa o AXP2101 PMU para:
 * - Porcentagem de bateria
 * - Voltagem
 * - Corrente de carga/descarga
 * - Status de carregamento
 */

#include "plugin_base.h"

class BatteryMonitorPlugin : public PluginBase {
public:
  BatteryMonitorPlugin();

  const char *getName() const override { return "battery"; }
  const char *getAuthor() const override { return "WavePwn"; }
  const char *getVersion() const override { return "1.0.0"; }

  bool onLoad() override;
  void update() override;

  // Getters
  uint8_t getPercent() const { return _percent; }
  uint16_t getVoltage() const { return _voltage; } // mV
  int16_t getCurrent() const { return _current; }  // mA (positivo = carregando)
  bool isCharging() const { return _charging; }
  bool isUSBConnected() const { return _usbConnected; }
  bool isBatteryPresent() const { return _batteryPresent; }

  // Formatação
  String getFormattedVoltage() const; // "4.15V"
  String getFormattedCurrent() const; // "+350mA" ou "-120mA"
  String getStatusText() const;       // "Charging", "Discharging", "Full"
  char getIcon() const;               // Para UI baseada em texto

  // Estimativas
  uint16_t getEstimatedTimeMinutes() const; // Tempo restante estimado

private:
  uint8_t _percent;
  uint16_t _voltage;
  int16_t _current;
  bool _charging;
  bool _usbConnected;
  bool _batteryPresent;
  bool _pmuAvailable;
  unsigned long _lastUpdate;

  // Para estimativa de tempo
  int32_t _currentAvg;
  int16_t _currentSamples[10];
  uint8_t _sampleIndex;

  static const unsigned long UPDATE_INTERVAL = 3000;
};

extern BatteryMonitorPlugin batteryPlugin;
