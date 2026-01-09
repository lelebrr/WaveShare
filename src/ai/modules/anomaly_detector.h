#pragma once
#include <Arduino.h>
#include <vector>

/**
 * @file anomaly_detector.h
 * @brief Detector de anomalias WiFi leve
 * 
 * Detecta comportamentos anormais usando heurísticas e análise de tendência.
 * Otimizado para ESP32-S3 (sem TFLite pesado).
 */

// Histórico de pacotes para análise de tendência
struct PacketRecord {
  uint8_t type;
  uint8_t subtype;
  uint16_t length;
  uint32_t timestamp;
};

class WiFiAnomalyDetector {
public:
  WiFiAnomalyDetector();
  void begin();

  /**
   * @brief Analisa características de pacote
   * @param length Tamanho do pacote
   * @param type Tipo do frame (0=Mgmt, 1=Control, 2=Data)
   * @param subtype Subtipo do frame
   * @return Score de anomalia (0.0 normal, 1.0 muito anômalo)
   */
  float feedPacketCharacteristics(int length, int type, int subtype);

  /**
   * @brief Retorna score médio baseado no histórico
   */
  float getAverageAnomalyScore() const;

  /**
   * @brief Verifica se está sob ataque baseado no threshold
   */
  bool isUnderAttack() const;

  /**
   * @brief Reseta contadores e histórico
   */
  void reset();

  // Setters
  void setThreshold(float threshold) { _threshold = threshold; }
  float getThreshold() const { return _threshold; }

private:
  static const int MAX_HISTORY = 100;
  
  float _threshold;
  std::vector<PacketRecord> _packetHistory;
  uint32_t _lastCheckTime = 0;
  uint16_t _deauthCount = 0;
  uint16_t _probeCount = 0;
};
