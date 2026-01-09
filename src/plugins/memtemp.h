#pragma once

/**
 * @file memtemp.h
 * @brief Plugin de memória e temperatura (inspirado no Pwnagotchi memtemp)
 *
 * Exibe:
 * - RAM livre (heap)
 * - Temperatura do chip
 * - PSRAM livre (se disponível)
 */

#include "plugin_base.h"

class MemTempPlugin : public PluginBase {
public:
  MemTempPlugin();

  // PluginBase interface
  const char *getName() const override { return "memtemp"; }
  const char *getAuthor() const override { return "WavePwn"; }
  const char *getVersion() const override { return "1.0.0"; }

  bool onLoad() override;
  void update() override;
  void onUIUpdate(const UIState &state) override;

  // Getters
  uint32_t getFreeHeap() const { return _freeHeap; }
  uint32_t getFreePSRAM() const { return _freePSRAM; }
  float getTemperature() const { return _temperature; }
  uint8_t getHeapPercent() const { return _heapPercent; }

  // Para exibição formatada
  String getFormattedHeap() const;
  String getFormattedPSRAM() const;
  String getFormattedTemp() const;

private:
  uint32_t _freeHeap;
  uint32_t _freePSRAM;
  uint32_t _totalHeap;
  uint32_t _totalPSRAM;
  float _temperature;
  uint8_t _heapPercent;
  unsigned long _lastUpdate;

  static const unsigned long UPDATE_INTERVAL = 2000; // 2 segundos
};

// Instância global
extern MemTempPlugin memtempPlugin;
