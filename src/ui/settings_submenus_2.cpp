#include "../core/globals.h"
#include "settings_menu_v2.h"

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 5: REDE & WIRELESS (16 items)
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createNetworkSubmenu(lv_obj_t *parent) {
  createSlider(parent, "Potência TX (dBm)", 8, 20, g_state.wifi_tx_power_dbm,
               nullptr);
  createDropdown(parent, "Canal WiFi",
                 "Auto\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13",
                 g_state.wifi_channel + 1, nullptr);
  createSwitch(parent, "MAC Spoof no Boot", g_state.mac_spoof_on_boot, nullptr);
  createSwitch(parent, "Usar MAC Custom", g_state.use_custom_mac, nullptr);
  createButton(parent, "MAC Customizado", "Editar", nullptr);
  createSwitch(parent, "2.4 GHz Ativo", g_state.radio_2ghz_enabled, nullptr);

  createSwitch(parent, "Beacon Spam", g_state.beacon_spam_enabled, nullptr);
  createSlider(parent, "Velocidade Beacon", 1, 10, g_state.beacon_spam_speed,
               nullptr);
  createSlider(parent, "Qtd SSIDs", 5, 50, g_state.beacon_ssid_count, nullptr);
  createSwitch(parent, "Evil Twin Auto", g_state.evil_twin_auto, nullptr);
  createSwitch(parent, "Karma/Mana Attack", g_state.karma_mana_enabled,
               nullptr);
  createSwitch(parent, "Modo Monitor Only", g_state.monitor_mode_only, nullptr);
  createSwitch(parent, "BLE Spam", g_state.ble_spam_enabled, nullptr);
  createDropdown(parent, "Tipo BLE Spam", "Apple\nWindows\nSamsung\nTodos",
                 g_state.ble_spam_type, nullptr);
  createSwitch(parent, "Modo Promíscuo", g_state.promiscuous_mode, nullptr);
  return parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 6: ATAQUES & AUTOMAÇÃO (14 items)
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createAttacksSubmenu(lv_obj_t *parent) {
  createSwitch(parent, "WPS Pixie Auto", g_state.wps_pixie_auto, nullptr);
  createSwitch(parent, "WPS Brute-force", g_state.wps_bruteforce, nullptr);
  createSlider(parent, "WPS Flood (1-10)", 1, 10, g_state.wps_flood_intensity,
               nullptr);
  createSlider(parent, "Rogue APs (1-50)", 1, 50, g_state.rogue_ap_count,
               nullptr);
  createSwitch(parent, "Auto-Capture Novas", g_state.auto_capture_new_only,
               nullptr);
  createSwitch(parent, "Auto-Salvar PCAP", g_state.auto_save_pcap, nullptr);
  createSwitch(parent, "Ataque Favoritos", g_state.auto_attack_favorites,
               nullptr);
  createSwitch(parent, "Modo Insano (60s)", g_state.insane_mode_enabled,
               nullptr);

  createSwitch(parent, "Portal Cativo Auto", g_state.captive_portal_auto,
               nullptr);
  createDropdown(parent, "Template Portal",
                 "Login\nFacebook\nGoogle\nWiFi Grátis\nHotel\nAeroporto",
                 g_state.captive_template_id, nullptr);
  createSwitch(parent, "BadUSB Auto", g_state.badusb_auto, nullptr);
  return parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 7: SISTEMA & ENERGIA (8 items)
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createSystemSubmenu(lv_obj_t *parent) {
  createSlider(parent, "Deep Sleep (min)", 0, 255, g_state.deep_sleep_minutes,
               nullptr);
  createSwitch(parent, "Acordar com Toque", g_state.wake_on_touch, nullptr);
  createSwitch(parent, "Acordar com Shake", g_state.wake_on_shake, nullptr);
  createSwitch(parent, "Boot Stealth", g_state.stealth_boot, nullptr);
  createSlider(parent, "Fuso Horário", -12, 14, g_state.timezone_offset,
               nullptr);
  createButton(parent, "Backup Config", "Salvar", nullptr);
  createButton(parent, "Restaurar Config", "Carregar", nullptr);
  createButton(parent, "OTA Update", "Atualizar", nullptr);
  createButton(parent, "Factory Reset", "⚠️ RESET", nullptr);
  createButton(parent, "Reiniciar", "Reboot", nullptr);
  return parent;
}
