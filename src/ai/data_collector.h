#pragma once
#include "feature_extractor.h"
#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

/**
 * @file data_collector.h
 * @brief Tip 26: Auto-coleta de dados para treino
 */

struct TrainingSample {
  Neura9Features features;
  int label;
  uint32_t timestamp;
};

class DataCollector {
public:
  DataCollector();
  bool begin();
  void logSample(const Neura9Features &features, int userLabel);
  String createCSVExport();
  void cleanOldData();

  // Legacy method fix/remove if not needed, but for now keep for compatibility
  // if called elsewhere
  void logFeatures(const Neura9Features &f, int label) { logSample(f, label); }

private:
  int _sampleCount;
};
