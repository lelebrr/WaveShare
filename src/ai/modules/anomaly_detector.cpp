/**
 * @file anomaly_detector.cpp
 * @brief Detector de anomalias WiFi leve para ESP32-S3
 * 
 * Usa heurísticas + scoring para detectar comportamentos anormais
 * sem necessidade de modelo TFLite pesado.
 */

#include "anomaly_detector.h"

WiFiAnomalyDetector::WiFiAnomalyDetector() : _threshold(0.5f) {}

void WiFiAnomalyDetector::begin() {
  // Inicializar historial de padrões
  _packetHistory.clear();
  _lastCheckTime = millis();
  Serial.println("[ANOMALY] Detector inicializado");
}

float WiFiAnomalyDetector::feedPacketCharacteristics(int length, int type,
                                                     int subtype) {
  float anomalyScore = 0.0f;
  
  // 1. Pacote Beacon gigante -> Possível buffer overflow exploit
  if (type == 0 && subtype == 8 && length > 1000) {
    anomalyScore += 0.8f;
  }

  // 2. Pacote de dados muito pequeno -> Possível injeção de controle
  if (type == 2 && length < 10) {
    anomalyScore += 0.2f;
  }

  // 3. Tipos reservados/inválidos (Management type = 0, Data = 2, Control = 1)
  if (type == 3) {
    anomalyScore += 1.0f;
  }

  // 4. Deauth flood detection (subtype 12 = deauth)
  if (type == 0 && subtype == 12) {
    _deauthCount++;
    // Se mais de 10 deauths em 1 segundo = ataque
    if (_deauthCount > 10) {
      anomalyScore += 0.9f;
    }
  }

  // 5. Probe request flood (subtype 4 = probe request)
  if (type == 0 && subtype == 4) {
    _probeCount++;
    // Muitos probes = possível scanner malicioso
    if (_probeCount > 50) {
      anomalyScore += 0.3f;
    }
  }

  // 6. Beacon com SSID muito longo ou caracteres especiais
  if (type == 0 && subtype == 8 && length > 300) {
    anomalyScore += 0.4f;
  }

  // Reset contadores a cada segundo
  uint32_t now = millis();
  if (now - _lastCheckTime > 1000) {
    _deauthCount = 0;
    _probeCount = 0;
    _lastCheckTime = now;
  }

  // Atualiza histórico para análise de tendência
  if (_packetHistory.size() >= MAX_HISTORY) {
    _packetHistory.erase(_packetHistory.begin());
  }
  _packetHistory.push_back({type, subtype, length, now});

  return min(anomalyScore, 1.0f);
}

float WiFiAnomalyDetector::getAverageAnomalyScore() const {
  if (_packetHistory.empty()) return 0.0f;
  
  float total = 0.0f;
  int count = 0;
  
  // Calcula score baseado no histórico
  for (const auto& pkt : _packetHistory) {
    // Heurística simples de peso
    if (pkt.type == 3) total += 1.0f;
    if (pkt.type == 0 && pkt.subtype == 12) total += 0.3f;
    count++;
  }
  
  return count > 0 ? total / count : 0.0f;
}

bool WiFiAnomalyDetector::isUnderAttack() const {
  return getAverageAnomalyScore() > _threshold;
}

void WiFiAnomalyDetector::reset() {
  _packetHistory.clear();
  _deauthCount = 0;
  _probeCount = 0;
  _lastCheckTime = millis();
}
