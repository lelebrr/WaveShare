/**
 * @file ui_settings_attack.cpp
 * @brief Submenu de configurações de Ataques & Automatização (14 opções)
 */

#include "ui_settings_attack.h"
#include "../core/config_manager.h"
#include "../core/globals.h"
#include "../hardware/audio_driver.h"
#include "ui_settings.h"
#include "ui_themes.h"
#include "widgets/ui_settings_widgets.h"

static lv_obj_t *scr_attack = nullptr;
static bool is_active = false;

// ============================================================
// CALLBACKS
// ============================================================
static void on_wps_pixie_change(bool checked) {
  g_state.wps_pixie_auto = checked;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] WPS Pixie Auto: %s\n", checked ? "ON" : "OFF");
}

static void on_wps_brute_change(bool checked) {
  g_state.wps_bruteforce = checked;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] WPS Brute: %s\n", checked ? "ON" : "OFF");
}

static void on_wps_flood_change(int32_t value) {
  g_state.wps_flood_intensity = value;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] WPS Flood: %ld\n", value);
}

static void on_rogue_ap_change(int32_t value) {
  g_state.rogue_ap_count = value;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] Rogue AP Count: %ld\n", value);
}

static void on_auto_capture_change(bool checked) {
  g_state.auto_capture_new_only = checked;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] Auto Capture New: %s\n", checked ? "ON" : "OFF");
}

static void on_auto_pcap_change(bool checked) {
  g_state.auto_save_pcap = checked;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] Auto Save PCAP: %s\n", checked ? "ON" : "OFF");
}

static void on_auto_attack_fav_change(bool checked) {
  g_state.auto_attack_favorites = checked;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] Auto Attack Favorites: %s\n", checked ? "ON" : "OFF");
}

static void on_insane_mode_change(bool checked) {
  g_state.insane_mode_enabled = checked;
  if (checked) {
    audioDriver.playSound(SOUND_ALERT);
  }
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] Insane Mode: %s\n", checked ? "ON" : "OFF");
}

static void on_captive_portal_change(bool checked) {
  g_state.captive_portal_auto = checked;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] Captive Portal Auto: %s\n", checked ? "ON" : "OFF");
}

static void on_captive_template_change(uint16_t selected) {
  g_state.captive_template_id = selected;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] Captive Template: %d\n", selected);
}

static void on_badusb_change(bool checked) {
  g_state.badusb_auto = checked;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] BadUSB Auto: %s\n", checked ? "ON" : "OFF");
}

static void on_usb_payload_change(uint16_t selected) {
  g_state.usb_payload_id = selected;
  config_manager.saveAttackSettings();
  Serial.printf("[CFG] USB Payload: %d\n", selected);
}

static void on_back_click(lv_event_t *e) {
  ui_settings_attack_hide();
  ui_settings_show();
}

// ============================================================
// CRIAÇÃO DA TELA
// ============================================================
static void create_screen() {
  lv_obj_t *content = ui_create_settings_screen(
      &scr_attack, LV_SYMBOL_WARNING " Ataques Auto", on_back_click);

  // === SEÇÃO WPS ===
  ui_create_section_header(content, "WPS Attacks");

  ui_create_switch_row(content, "WPS Pixie Dust", LV_SYMBOL_CHARGE,
                       g_state.wps_pixie_auto, on_wps_pixie_change);

  ui_create_switch_row(content, "WPS Brute-Force", LV_SYMBOL_LOOP,
                       g_state.wps_bruteforce, on_wps_brute_change);

  ui_create_slider_row(content, "WPS Flood DoS", LV_SYMBOL_WARNING, 1, 10,
                       g_state.wps_flood_intensity, on_wps_flood_change);

  // === SEÇÃO AUTOMATIZAÇÃO ===
  ui_create_section_header(content, "Automatização");

  ui_create_slider_row(content, "Rogue APs", LV_SYMBOL_WIFI, 1, 50,
                       g_state.rogue_ap_count, on_rogue_ap_change);

  ui_create_switch_row(content, "Auto-Capture Novas", LV_SYMBOL_DOWNLOAD,
                       g_state.auto_capture_new_only, on_auto_capture_change);

  ui_create_switch_row(content, "Auto-Save PCAP", LV_SYMBOL_SAVE,
                       g_state.auto_save_pcap, on_auto_pcap_change);

  ui_create_switch_row(content, "Attack Favoritas", LV_SYMBOL_OK,
                       g_state.auto_attack_favorites,
                       on_auto_attack_fav_change);

  // === SEÇÃO MODOS ESPECIAIS ===
  ui_create_section_header(content, "Modos Especiais");

  ui_create_switch_row(content, "🔥 INSANE MODE", LV_SYMBOL_WARNING,
                       g_state.insane_mode_enabled, on_insane_mode_change);

  // === SEÇÃO PORTAL CATIVO ===
  ui_create_section_header(content, "Portal Cativo");

  ui_create_switch_row(content, "Portal Auto", LV_SYMBOL_KEYBOARD,
                       g_state.captive_portal_auto, on_captive_portal_change);

  ui_create_dropdown_row(
      content, "Template", LV_SYMBOL_FILE,
      "Login Genérico\nGoogle\nFacebook\nWiFi "
      "Grátis\nHotel\nAeroporto\nEmpresa\nBanco\nNetflix\nCustom",
      g_state.captive_template_id, on_captive_template_change);

  // === SEÇÃO BADUSB ===
  ui_create_section_header(content, "BadUSB");

  ui_create_switch_row(content, "BadUSB Auto", LV_SYMBOL_USB,
                       g_state.badusb_auto, on_badusb_change);

  ui_create_dropdown_row(
      content, "Payload USB", LV_SYMBOL_FILE,
      "RevShell Win\nRevShell Linux\nWiFi Stealer\nExfil Docs\nCrypto "
      "Miner\nKeylogger\nBackdoor\nInfo Gather\nPrank\nCustom",
      g_state.usb_payload_id, on_usb_payload_change);
}

// ============================================================
// API PÚBLICA
// ============================================================
void ui_settings_attack_show() {
  if (!scr_attack) {
    create_screen();
  }
  is_active = true;
  lv_scr_load(scr_attack);
}

void ui_settings_attack_hide() {
  is_active = false;
  if (scr_attack) {
    lv_obj_del(scr_attack);
    scr_attack = nullptr;
  }
}

bool ui_settings_attack_is_active() { return is_active; }
