#include "data_collector.h"
#include <LittleFS.h>

DataCollector::DataCollector() : _sampleCount(0) {}

bool DataCollector::begin() {
  if (!LittleFS.exists("/ai_data")) {
    LittleFS.mkdir("/ai_data");
  }
  return true;
}

void DataCollector::logSample(const Neura9Features &features, int userLabel) {
  File f = LittleFS.open("/ai_data/samples.bin", "ab");
  if (f) {
    TrainingSample sample;
    sample.features = features;
    sample.label = userLabel;
    sample.timestamp = millis();

    f.write((uint8_t *)&sample, sizeof(TrainingSample));
    f.close();
    _sampleCount++;

    Serial.printf("[AI-DATA] Sample saved. Total: %d\n", _sampleCount);
  }
}

String DataCollector::createCSVExport() {
  // Converte binário para CSV para o usuário baixar
  File bin = LittleFS.open("/ai_data/samples.bin", "rb");
  if (!bin)
    return "";

  File csv = LittleFS.open("/ai_training.csv", "w");
  if (!csv) {
    bin.close();
    return "";
  }

  // Header
  csv.println("rssi,snr,beacon_rate,probe_rate,deauth_rate,label");
  // ... adicione todas as 56 headers

  TrainingSample s;
  while (bin.read((uint8_t *)&s, sizeof(TrainingSample)) ==
         sizeof(TrainingSample)) {
    csv.printf("%.3f,%.3f,%.3f,%.3f,%.3f,%d\n", s.features.rssi_norm,
               s.features.snr_norm, s.features.beacon_rate,
               s.features.probereq_rate, s.features.deauth_rate, s.label);
  }

  bin.close();
  csv.close();
  return "/ai_training.csv";
}

void DataCollector::cleanOldData() {
  LittleFS.remove("/ai_data/samples.bin");
  _sampleCount = 0;
}
