/**
 * @file ui_settings_network.cpp
 * @brief Submenu de configurações de Rede & Wireless (13 opções)
 *
 * Opções:
 * 1. Potência TX WiFi (8-20 dBm)
 * 2. Canal WiFi (fixo/auto)
 * 3. MAC address + Randomizar
 * 4. Spoofing MAC a cada boot
 * 5. Ativar 2.4 GHz
 * 6. Beacon Spam (On/Off + velocidade)
 * 7. Evil Twin automático
 * 8. Karma + Mana attack
 * 9. Deauth em todas redes
 * 10. Modo monitor apenas
 * 11. BLE Spam (On/Off + tipo)
 * 12. Forçar promiscuous
 */

#include "ui_settings_network.h"
#include "../core/config_manager.h"
#include "../core/globals.h"
#include "ui_settings.h"
#include "ui_themes.h"
#include "widgets/ui_settings_widgets.h"
#include <WiFi.h>
#include <esp_wifi.h>

static lv_obj_t *scr_network = nullptr;
static lv_obj_t *lbl_mac = nullptr;
static bool is_active = false;

// ============================================================
// CALLBACKS
// ============================================================
static void on_tx_power_change(int32_t value) {
  g_state.wifi_tx_power_dbm = value;
  esp_wifi_set_max_tx_power(value * 4); // ESP32 usa 0.25dBm steps
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] TX Power: %ld dBm\n", value);
}

static void on_channel_change(uint16_t selected) {
  g_state.wifi_channel = selected == 0 ? -1 : selected;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] WiFi Channel: %d\n", g_state.wifi_channel);
}

static void on_randomize_mac() {
  // Gera MAC aleatório
  for (int i = 0; i < 6; i++) {
    g_state.custom_mac[i] = random(256);
  }
  g_state.custom_mac[0] =
      (g_state.custom_mac[0] & 0xFE) | 0x02; // Local unicast
  g_state.use_custom_mac = true;

  // Aplica novo MAC
  esp_wifi_set_mac(WIFI_IF_STA, g_state.custom_mac);
  config_manager.saveNetworkSettings();

  // Atualiza label
  if (lbl_mac) {
    char mac_str[20];
    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
             g_state.custom_mac[0], g_state.custom_mac[1],
             g_state.custom_mac[2], g_state.custom_mac[3],
             g_state.custom_mac[4], g_state.custom_mac[5]);
    ui_update_info_row(lbl_mac, mac_str);
  }

  Serial.printf("[CFG] MAC Randomizado: %02X:%02X:%02X:%02X:%02X:%02X\n",
                g_state.custom_mac[0], g_state.custom_mac[1],
                g_state.custom_mac[2], g_state.custom_mac[3],
                g_state.custom_mac[4], g_state.custom_mac[5]);
}

static void on_mac_spoof_boot_change(bool checked) {
  g_state.mac_spoof_on_boot = checked;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] MAC Spoof on Boot: %s\n", checked ? "ON" : "OFF");
}

static void on_2ghz_change(bool checked) {
  g_state.radio_2ghz_enabled = checked;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] 2.4GHz Radio: %s\n", checked ? "ON" : "OFF");
}

static void on_beacon_spam_change(bool checked) {
  g_state.beacon_spam_enabled = checked;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] Beacon Spam: %s\n", checked ? "ON" : "OFF");
}

static void on_beacon_speed_change(int32_t value) {
  g_state.beacon_spam_speed = value;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] Beacon Speed: %ld\n", value);
}

static void on_evil_twin_change(bool checked) {
  g_state.evil_twin_auto = checked;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] Evil Twin Auto: %s\n", checked ? "ON" : "OFF");
}

static void on_karma_mana_change(bool checked) {
  g_state.karma_mana_enabled = checked;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] Karma/Mana: %s\n", checked ? "ON" : "OFF");
}

static void on_deauth_all_change(bool checked) {
  g_state.deauth_all_networks = checked;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] Deauth All: %s\n", checked ? "ON" : "OFF");
}

static void on_monitor_only_change(bool checked) {
  g_state.monitor_mode_only = checked;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] Monitor Only: %s\n", checked ? "ON" : "OFF");
}

static void on_ble_spam_change(bool checked) {
  g_state.ble_spam_enabled = checked;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] BLE Spam: %s\n", checked ? "ON" : "OFF");
}

static void on_ble_type_change(uint16_t selected) {
  g_state.ble_spam_type = selected;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] BLE Spam Type: %d\n", selected);
}

static void on_promiscuous_change(bool checked) {
  g_state.promiscuous_mode = checked;
  config_manager.saveNetworkSettings();
  Serial.printf("[CFG] Promiscuous Mode: %s\n", checked ? "ON" : "OFF");
}

static void on_back_click(lv_event_t *e) {
  ui_settings_network_hide();
  ui_settings_show();
}

// ============================================================
// CRIAÇÃO DA TELA
// ============================================================
static void create_screen() {
  lv_obj_t *content = ui_create_settings_screen(
      &scr_network, LV_SYMBOL_WIFI " Rede & Wireless", on_back_click);

  // === SEÇÃO WIFI ===
  ui_create_section_header(content, "WiFi");

  // TX Power
  ui_create_slider_row(content, "Potência TX", LV_SYMBOL_WIFI, 8, 20,
                       g_state.wifi_tx_power_dbm, on_tx_power_change);

  // Canal
  ui_create_dropdown_row(content, "Canal", LV_SYMBOL_SETTINGS,
                         "Auto\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13",
                         g_state.wifi_channel < 0 ? 0 : g_state.wifi_channel,
                         on_channel_change);

  // MAC Address (info + botão)
  char mac_str[20];
  uint8_t mac[6];
  esp_wifi_get_mac(WIFI_IF_STA, mac);
  snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],
           mac[1], mac[2], mac[3], mac[4], mac[5]);
  lbl_mac = ui_create_info_row(content, "MAC", LV_SYMBOL_COPY, mac_str);

  // Randomizar MAC
  ui_create_button_row(content, "Randomizar MAC", LV_SYMBOL_SHUFFLE, "Random",
                       on_randomize_mac);

  // MAC Spoof on Boot
  ui_create_switch_row(content, "Spoof MAC Boot", LV_SYMBOL_REFRESH,
                       g_state.mac_spoof_on_boot, on_mac_spoof_boot_change);

  // 2.4 GHz
  ui_create_switch_row(content, "Rádio 2.4 GHz", LV_SYMBOL_WIFI,
                       g_state.radio_2ghz_enabled, on_2ghz_change);

  // === SEÇÃO ATAQUES WIFI ===
  ui_create_section_header(content, "Ataques WiFi");

  // Beacon Spam
  ui_create_switch_row(content, "Beacon Spam", LV_SYMBOL_LOOP,
                       g_state.beacon_spam_enabled, on_beacon_spam_change);

  // Beacon Speed
  ui_create_slider_row(content, "Velocidade Beacon", LV_SYMBOL_CHARGE, 1, 10,
                       g_state.beacon_spam_speed, on_beacon_speed_change);

  // Evil Twin
  ui_create_switch_row(content, "Evil Twin Auto", LV_SYMBOL_WARNING,
                       g_state.evil_twin_auto, on_evil_twin_change);

  // Karma/Mana
  ui_create_switch_row(content, "Karma + Mana", LV_SYMBOL_CALL,
                       g_state.karma_mana_enabled, on_karma_mana_change);

  // Deauth All
  ui_create_switch_row(content, "Deauth Todas", LV_SYMBOL_CLOSE,
                       g_state.deauth_all_networks, on_deauth_all_change);

  // Monitor Only
  ui_create_switch_row(content, "Modo Monitor", LV_SYMBOL_EYE_OPEN,
                       g_state.monitor_mode_only, on_monitor_only_change);

  // Promiscuous
  ui_create_switch_row(content, "Promiscuous", LV_SYMBOL_EYE_OPEN,
                       g_state.promiscuous_mode, on_promiscuous_change);

  // === SEÇÃO BLE ===
  ui_create_section_header(content, "Bluetooth");

  // BLE Spam
  ui_create_switch_row(content, "BLE Spam", LV_SYMBOL_BLUETOOTH,
                       g_state.ble_spam_enabled, on_ble_spam_change);

  // Tipo BLE
  ui_create_dropdown_row(content, "Tipo Spam", LV_SYMBOL_SETTINGS,
                         "Apple\nWindows\nSamsung", g_state.ble_spam_type,
                         on_ble_type_change);
}

// ============================================================
// API PÚBLICA
// ============================================================
void ui_settings_network_show() {
  if (!scr_network) {
    create_screen();
  }
  is_active = true;
  lv_scr_load(scr_network);
}

void ui_settings_network_hide() {
  is_active = false;
  if (scr_network) {
    lv_obj_del(scr_network);
    scr_network = nullptr;
    lbl_mac = nullptr;
  }
}

bool ui_settings_network_is_active() { return is_active; }
