#pragma once
#include <Arduino.h>

/**
 * @file neura9_model_data.h
 * @brief Modelo TFLite leve para classificação de ameaças WiFi
 *
 * Arquitetura: Dense(8) -> Dense(16, ReLU) -> Dense(10, Softmax)
 * Tamanho: ~600 bytes (quantizado INT8)
 *
 * Classes de saída:
 * 0: SAFE - Ambiente seguro
 * 1: CROWDED - Muitas redes
 * 2: OPEN_NETWORK - Redes abertas detectadas
 * 3: EVIL_TWIN_RISK - Possível evil twin
 * 4: DEAUTH_DETECTED - Ataque deauth em progresso
 * 5: ROGUE_AP - AP malicioso
 * 6: HIGH_RISK - Múltiplos indicadores
 * 7: BATTERY_CRITICAL - Bateria baixa
 * 8: GESTURE_COMMAND - Gesto IMU detectado
 * 9: LEARNING_MODE - Modo coleta de dados
 */

extern const unsigned char g_neura9_model_data[];
extern const unsigned int g_neura9_model_data_len;

// Feature indices para extração básica (modelo leve)
enum Neura9FeatureIndex {
  FEAT_TOTAL_NETWORKS = 0,    // Normalizado 0-1 (0-50 redes)
  FEAT_OPEN_RATIO = 1,        // % redes abertas
  FEAT_AVG_RSSI = 2,          // RSSI médio normalizado
  FEAT_CHANNEL_DIVERSITY = 3, // Diversidade de canais
  FEAT_HIDDEN_RATIO = 4,      // % SSIDs ocultos
  FEAT_DUPLICATE_SSID = 5,    // SSIDs duplicados
  FEAT_DEAUTH_FLAG = 6,       // Deauth detectado (0/1)
  FEAT_BATTERY_LEVEL = 7,     // Nível bateria normalizado
  FEAT_COUNT = 8
};

// Threat classes
enum Neura9ThreatClass {
  THREAT_SAFE = 0,
  THREAT_CROWDED = 1,
  THREAT_OPEN_NETWORK = 2,
  THREAT_EVIL_TWIN = 3,
  THREAT_DEAUTH = 4,
  THREAT_ROGUE_AP = 5,
  THREAT_HIGH_RISK = 6,
  THREAT_BATTERY_CRITICAL = 7,
  THREAT_GESTURE = 8,
  THREAT_LEARNING = 9,
  THREAT_CLASS_COUNT = 10
};

// Nomes das classes para UI/logs
inline const char *getThreatClassName(uint8_t classId) {
  static const char *names[] = {"Seguro",   "Lotado",   "Rede Aberta",
                                "Evil Twin", "Deauth",   "Rogue AP",
                                "Alto Risco", "Bateria", "Gesto",
                                "Aprendendo"};
  return classId < 10 ? names[classId] : "Desconhecido";
}
