#pragma once

/**
 * @file wps_attacks.h
 * @brief Módulo de ataques WPS para WavePwn
 *
 * AVISO: Apenas para fins educacionais e pesquisa em segurança.
 * Use apenas em redes próprias ou com autorização explícita.
 *
 * Implementa:
 * - WPS Pixie Dust (offline attack)
 * - WPS PIN Brute-force
 * - WPS PIN Flood DoS
 */

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <vector>

/**
 * @brief Tipos de ataques WPS
 */
enum WPSAttackType {
  WPS_ATTACK_NONE = 0,
  WPS_ATTACK_PIXIE_DUST, // Ataque offline via vulnerabilidade E-Hash
  WPS_ATTACK_BRUTEFORCE, // Força bruta dos 11.000 PINs
  WPS_ATTACK_PIN_FLOOD,  // DoS por flood de M4 malformados
  WPS_ATTACK_SCAN        // Apenas escanear redes WPS
};

/**
 * @brief Status do ataque WPS
 */
enum WPSAttackStatus {
  WPS_STATUS_IDLE = 0,
  WPS_STATUS_SCANNING,
  WPS_STATUS_ATTACKING,
  WPS_STATUS_SUCCESS,
  WPS_STATUS_FAILED,
  WPS_STATUS_LOCKED // AP bloqueou WPS
};

/**
 * @brief Informações de uma rede WPS vulnerável
 */
struct WPSNetwork {
  uint8_t bssid[6];
  char ssid[33];
  uint8_t channel;
  int8_t rssi;
  bool wps_enabled;
  bool wps_locked;
  char pin[9];  // PIN descoberto (se sucesso)
  char psk[65]; // Password descoberta (se sucesso)
};

/**
 * @brief Estatísticas do ataque WPS
 */
struct WPSStats {
  WPSAttackType attack_type;
  WPSAttackStatus status;
  uint32_t pins_tried;
  uint32_t total_pins;
  uint32_t packets_sent;
  unsigned long start_time;
  unsigned long elapsed_ms;
  char current_pin[9];
  bool pin_found;
};

/**
 * @brief Classe principal de ataques WPS
 */
class WPSAttacks {
public:
  WPSAttacks();

  /**
   * @brief Inicializa o módulo
   */
  void begin();

  /**
   * @brief Escaneia redes com WPS habilitado
   * @return Número de redes WPS encontradas
   */
  int scanWPSNetworks();

  /**
   * @brief Obtém lista de redes WPS encontradas
   */
  std::vector<WPSNetwork> &getWPSNetworks();

  /**
   * @brief Define o alvo do ataque
   */
  void setTarget(const uint8_t *bssid, const char *ssid, uint8_t channel);

  /**
   * @brief Inicia ataque Pixie Dust
   * Explora vulnerabilidade no E-Hash para recuperar PIN offline
   */
  bool startPixieDust();

  /**
   * @brief Inicia brute-force de PIN WPS
   * @param start_pin PIN inicial (0-99999999)
   */
  bool startBruteforce(uint32_t start_pin = 0);

  /**
   * @brief Inicia PIN Flood DoS
   * Envia milhares de M4 malformados para travar o AP
   */
  bool startPINFlood();

  /**
   * @brief Para o ataque atual
   */
  void stopAttack();

  /**
   * @brief Atualiza lógica do ataque (chamar no loop)
   */
  void update();

  /**
   * @brief Obtém estatísticas do ataque
   */
  WPSStats getStats();

  /**
   * @brief Verifica se ataque está ativo
   */
  bool isActive() { return attack_active; }

  /**
   * @brief Obtém PIN descoberto
   */
  const char *getDiscoveredPIN() { return discovered_pin; }

  /**
   * @brief Obtém PSK descoberta
   */
  const char *getDiscoveredPSK() { return discovered_psk; }

private:
  bool attack_active;
  WPSAttackType current_attack;
  WPSAttackStatus status;

  // Alvo
  uint8_t target_bssid[6];
  char target_ssid[33];
  uint8_t target_channel;

  // Estado do brute-force
  uint32_t current_pin;
  uint32_t pins_tried;
  uint32_t packets_sent;
  unsigned long attack_start_time;

  // Resultados
  char discovered_pin[9];
  char discovered_psk[65];

  // Lista de redes WPS
  std::vector<WPSNetwork> wps_networks;

  // Métodos internos
  bool sendM1();
  bool sendM3(const uint8_t *m2_data);
  bool sendM5(const uint8_t *m4_data);
  bool sendM7(const uint8_t *m6_data);
  bool parseM2(const uint8_t *data, int len);
  bool parseM4(const uint8_t *data, int len);
  bool parseM6(const uint8_t *data, int len);
  bool parseM8(const uint8_t *data, int len);

  uint32_t calculateChecksum(uint32_t pin);
  uint32_t generateNextPIN();
  bool isValidPIN(uint32_t pin);

  // Pixie Dust
  bool attemptPixieDust(const uint8_t *e_hash1, const uint8_t *e_hash2,
                        const uint8_t *pke, const uint8_t *pkr,
                        const uint8_t *authkey);
};

extern WPSAttacks wps_attacks;
