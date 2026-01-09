/**
 * @file memtemp.cpp
 * @brief Implementação do plugin memtemp
 */

#include "memtemp.h"
#include <esp_system.h>

// Instância global
MemTempPlugin memtempPlugin;

MemTempPlugin::MemTempPlugin()
    : _freeHeap(0), _freePSRAM(0), _totalHeap(0), _totalPSRAM(0),
      _temperature(0), _heapPercent(0), _lastUpdate(0) {}

bool MemTempPlugin::onLoad() {
  _totalHeap = ESP.getHeapSize();
  _totalPSRAM = ESP.getPsramSize();

  Serial.printf("[memtemp] Heap total: %u bytes\n", _totalHeap);
  Serial.printf("[memtemp] PSRAM total: %u bytes\n", _totalPSRAM);

  // Leitura inicial
  update();
  return true;
}

void MemTempPlugin::update() {
  unsigned long now = millis();
  if (now - _lastUpdate < UPDATE_INTERVAL)
    return;
  _lastUpdate = now;

  // Atualiza valores
  _freeHeap = ESP.getFreeHeap();
  _freePSRAM = ESP.getFreePsram();

  // Calcula porcentagem usada
  if (_totalHeap > 0) {
    _heapPercent = 100 - ((_freeHeap * 100) / _totalHeap);
  }

// Temperatura (ESP32-S3 tem sensor interno)
// Nota: temperatureRead() pode não estar disponível em todas as versões
#if CONFIG_IDF_TARGET_ESP32S3
  _temperature = temperatureRead();
#else
  _temperature = 0;
#endif
}

void MemTempPlugin::onUIUpdate(const UIState &state) {
  // Chamado quando UI atualiza - pode adicionar elementos aqui
}

String MemTempPlugin::getFormattedHeap() const {
  if (_freeHeap >= 1024 * 1024) {
    return String(_freeHeap / (1024.0 * 1024.0), 1) + "MB";
  } else if (_freeHeap >= 1024) {
    return String(_freeHeap / 1024.0, 1) + "KB";
  }
  return String(_freeHeap) + "B";
}

String MemTempPlugin::getFormattedPSRAM() const {
  if (_freePSRAM >= 1024 * 1024) {
    return String(_freePSRAM / (1024.0 * 1024.0), 1) + "MB";
  } else if (_freePSRAM >= 1024) {
    return String(_freePSRAM / 1024.0, 1) + "KB";
  }
  return String(_freePSRAM) + "B";
}

String MemTempPlugin::getFormattedTemp() const {
  return String(_temperature, 1) + "C";
}
