#pragma once
#include <Arduino.h>
#include <vector>

/**
 * @file anomaly_detector.h
 * @brief Tip 18: Autoencoder simulado para detecção de anomalias
 */

class AnomalyDetector {
public:
  AnomalyDetector() {
    // Inicializa stats
    _avgRssi = -70;
    _avgPackets = 50;
  }

  // Retorna 'true' se detectar anomalia (desvio > 2.5 sigma)
  bool isAnomalous(float rssi, float packetRate, float deauthRate) {
    // Atualização média móvel simples (EMA)
    _avgRssi = (_avgRssi * 0.95) + (rssi * 0.05);
    _avgPackets = (_avgPackets * 0.95) + (packetRate * 0.05);

    // Calcula desvio do padrão
    float zScoreRssi = abs(rssi - _avgRssi) / 10.0f; // Assumindo desvio base 10
    float zScorePkts =
        abs(packetRate - _avgPackets) / 20.0f; // Assumindo desvio base 20

    // Se deauthRate spike absurdo (> 50% de frames são deauth)
    if (deauthRate > 0.5f)
      return true;

    // Multi-variate score
    float anomalyScore = zScoreRssi + zScorePkts;

    if (anomalyScore > 4.0f) {
      return true; // Anomaly!
    }
    return false;
  }

private:
  float _avgRssi;
  float _avgPackets;
};
