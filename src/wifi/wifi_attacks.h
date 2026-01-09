#pragma once

/**
 * @file wifi_attacks.h
 * @brief Header do módulo de ataques WiFi
 *
 * AVISO: Apenas para fins educacionais e pesquisa em segurança.
 */

#include "../core/config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_attr.h> // For IRAM_ATTR
#include <vector>

/**
 * @brief Tipos de ataques disponíveis
 */
enum AttackType {
  ATTACK_NONE = 0,
  ATTACK_DEAUTH,
  ATTACK_BEACON_FLOOD,
  ATTACK_PROBE_SPAM,
  ATTACK_EVIL_TWIN,
  ATTACK_HANDSHAKE,
  ATTACK_PMKID,
  ATTACK_HANDSHAKE_SNIPER,
  ATTACK_KARMA_MANA,
  ATTACK_WPS_PIXIE,
  ATTACK_AP_FLOOD_CHAOS,
  ATTACK_RICKROLL,
  ATTACK_GHOST_AP,
  ATTACK_ONE_TAP_NUKE,   // [NEW] Tip 26
  ATTACK_DOWNGRADE_TKIP, // [NEW] Tip 12
  ATTACK_HIDDEN_SSID     // [NEW] Tip 13
};

/**
 * @brief Estatísticas do ataque atual
 */
struct AttackStats {
  AttackType attack_type;
  uint32_t packets_sent;
  uint32_t handshakes_captured;
  bool is_active;
  uint32_t pmkids_captured;
  uint32_t clients_kicked;
};

/**
 * @brief Informações de um cliente conectado ao alvo
 */
struct ClientInfo {
  uint8_t mac[6];
  int8_t rssi;
  uint32_t last_seen;
};

/**
 * @brief Classe principal de ataques WiFi
 */
class WiFiAttacks {
public:
  WiFiAttacks();

  /**
   * @brief Inicializa o módulo
   */
  void begin();

  /**
   * @brief Define o alvo do ataque
   */
  void setTarget(const uint8_t *bssid, const char *ssid, uint8_t channel);

  /**
   * @brief Inicia ataque de deautenticação
   * @param burst_count Número de pacotes por burst
   */
  bool startDeauth(int burst_count = 10);

  /**
   * @brief Inicia beacon flood
   * @param ssid_list Lista de SSIDs (nullptr = usar padrão)
   * @param count Número de SSIDs
   */
  bool startBeaconFlood(const char **ssid_list = nullptr, int count = 0);

  /**
   * @brief Inicia captura de handshakes
   */
  bool startHandshakeCapture();

  /**
   * @brief Inicia ataque Evil Twin com Captive Portal
   * @param target_ssid SSID a clonar
   * @param portal_type Tipo de portal (0=genérico, 1=Google)
   */
  bool startEvilTwin(const char *target_ssid, int portal_type = 0);

  /**
   * @brief Inicia ataque Smart Deauth (apenas clientes conectados)
   */
  bool startSmartDeauth();

  /**
   * @brief Inicia ataque Turbo Deauth (raw injection 300pps)
   */
  bool startTurboDeauth();

  /**
   * @brief Inicia ataque PMKID (client-less)
   */
  bool startPMKIDAttack();

  /**
   * @brief Inicia ataque Handshake Sniper (auto-target)
   */
  bool startHandshakeSniper();

  /**
   * @brief Inicia ataque Karma/Mana Hybrid
   */
  bool startKarmaMana();

  bool startRickRoll();
  bool startGhostAP();
  bool startProbeFlood(const char *prefix = nullptr);
  bool startAPFloodChaos();

  void setTxPower(int8_t dbm);
  void enableTurboMode(bool enabled);

  // [NEW] Ataques Avançados & Otimizações
  bool startOneTapNuke();
  bool startDowngradeAttack();
  bool startHiddenSSIDReveal();
  void toggleSleepAttack(bool enabled);

  // Tip 22: FreeRTOS Queue Handshake saving
  bool enqueueHandshake(const uint8_t *pcap_data, size_t len);

  // FreeRTOS Task (Internal RAM for stack speed)
  static void pcapSaveTask(void *parameter);

  /**
   * @brief Para o ataque atual
   */
  void stopAttack();

  /**
   * @brief Atualiza lógica do ataque (chamar no loop)
   */
  void update();

  /**
   * @brief Callback quando handshake é detectado
   */
  void onHandshakeDetected(const uint8_t *data, int len, int8_t rssi);

  /**
   * @brief Obtém tamanho do buffer PCAP
   */
  int getPcapSize();

  /**
   * @brief Obtém ponteiro para buffer PCAP
   */
  uint8_t *getPcapBuffer();

  /**
   * @brief Obtém estatísticas do ataque
   */
  AttackStats getStats();

  // Status
  bool isActive() { return attack_active; }
  AttackType getCurrentAttack() { return current_attack; }
  uint32_t getPacketCount() { return packets_sent; }

  void processPacket(const uint8_t *frame, int len, int8_t rssi);

private:
  bool attack_active;
  AttackType current_attack;
  uint32_t packets_sent;
  uint32_t handshakes_captured;
  uint32_t pmkids_captured;
  uint32_t clients_kicked;

  // Lista de clientes do alvo (para Smart Deauth)
  std::vector<ClientInfo> target_clients;

  uint8_t target_bssid[6];
  char target_ssid[33];
  char target_ssid_str[33]; // For Evil Twin
  uint8_t target_channel;

  // Buffer PCAP
  uint8_t *pcap_buffer;
  int pcap_buffer_pos;

  int buildBeaconFrame(uint8_t *frame, const char *ssid, uint8_t channel);
  void initPcapBuffer();
  void sendBeaconBatch();
  void sendProbeRequestBatch();
  void sendDeauthBurst(bool smart = false, bool turbo = false);

  // [NEW] Private helpers
  void sendDowngradeFrame();
  void sendDeauthAll(); // For Nuke

  // FreeRTOS
  QueueHandle_t pcap_queue;
  TaskHandle_t pcap_task_handle;
};

extern WiFiAttacks wifi_attacks;
