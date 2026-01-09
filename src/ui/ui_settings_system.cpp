/**
 * @file ui_settings_system.cpp
 * @brief Submenu de configurações de Sistema & Energia (12 opções)
 */

#include "ui_settings_system.h"
#include "../core/config_manager.h"
#include "../core/globals.h"
#include "../hardware/audio_driver.h"
#include "../hardware/system_hardware.h"
#include "ui_settings.h"
#include "ui_themes.h"
#include "widgets/ui_settings_widgets.h"
#include <LittleFS.h>
#include <SD_MMC.h>
#include <esp_ota_ops.h>
#include <esp_system.h>

static lv_obj_t *scr_system = nullptr;
static lv_obj_t *lbl_firmware = nullptr;
static lv_obj_t *lbl_storage = nullptr;
static lv_obj_t *lbl_temp = nullptr;
static bool is_active = false;
static int reset_confirm_count = 0;

// ============================================================
// CALLBACKS
// ============================================================
static void on_deep_sleep_change(int32_t value) {
  g_state.deep_sleep_minutes = value;
  config_manager.saveSystemSettings();
  Serial.printf("[CFG] Deep Sleep: %ld min\n", value);
}

static void on_wake_touch_change(bool checked) {
  g_state.wake_on_touch = checked;
  config_manager.saveSystemSettings();
  Serial.printf("[CFG] Wake on Touch: %s\n", checked ? "ON" : "OFF");
}

static void on_wake_shake_change(bool checked) {
  g_state.wake_on_shake = checked;
  config_manager.saveSystemSettings();
  Serial.printf("[CFG] Wake on Shake: %s\n", checked ? "ON" : "OFF");
}

static void on_big_battery_change(bool checked) {
  g_state.big_battery_display = checked;
  config_manager.saveSystemSettings();
  Serial.printf("[CFG] Big Battery Display: %s\n", checked ? "ON" : "OFF");
}

static void on_stealth_boot_change(bool checked) {
  g_state.stealth_boot = checked;
  config_manager.saveSystemSettings();
  Serial.printf("[CFG] Stealth Boot: %s\n", checked ? "ON" : "OFF");
}

static void on_timezone_change(uint16_t selected) {
  // selected 0 = -12, selected 26 = +14
  g_state.timezone_offset = (int8_t)(selected - 12);
  config_manager.saveSystemSettings();
  Serial.printf("[CFG] Timezone: UTC%+d\n", g_state.timezone_offset);
}

static void on_restart_click() {
  Serial.println("[SYS] Reiniciando...");
  audioDriver.playSound(SOUND_ALERT);
  delay(500);
  esp_restart();
}

static void on_factory_reset_click() {
  reset_confirm_count++;

  if (reset_confirm_count < 3) {
    audioDriver.playSound(SOUND_CLICK);
    Serial.printf("[SYS] Reset confirmação %d/3\n", reset_confirm_count);
    // TODO: Mostrar alerta de confirmação
  } else {
    Serial.println("[SYS] FACTORY RESET!");
    audioDriver.playSound(SOUND_ALERT);
    config_manager.reset();
    delay(500);
    esp_restart();
  }
}

static void on_calibrate_battery_click() {
  Serial.println("[SYS] Iniciando calibração de bateria...");
  audioDriver.playSound(SOUND_SUCCESS);
  // TODO: Implementar wizard de calibração
}

static void on_back_click(lv_event_t *e) {
  reset_confirm_count = 0;
  ui_settings_system_hide();
  ui_settings_show();
}

// ============================================================
// CRIAÇÃO DA TELA
// ============================================================
static void create_screen() {
  lv_obj_t *content = ui_create_settings_screen(
      &scr_system, LV_SYMBOL_SETTINGS " Sistema", on_back_click);

  // === SEÇÃO ENERGIA ===
  ui_create_section_header(content, "Energia");

  ui_create_slider_row(content, "Deep Sleep", LV_SYMBOL_POWER, 0, 60,
                       g_state.deep_sleep_minutes, on_deep_sleep_change);

  ui_create_switch_row(content, "Wake on Touch", LV_SYMBOL_EDIT,
                       g_state.wake_on_touch, on_wake_touch_change);

  ui_create_switch_row(content, "Wake on Shake", LV_SYMBOL_REFRESH,
                       g_state.wake_on_shake, on_wake_shake_change);

  ui_create_switch_row(content, "Bateria Grande", LV_SYMBOL_BATTERY_FULL,
                       g_state.big_battery_display, on_big_battery_change);

  ui_create_button_row(content, "Calibrar Bateria", LV_SYMBOL_SETTINGS,
                       "Calibrar", on_calibrate_battery_click);

  // === SEÇÃO BOOT ===
  ui_create_section_header(content, "Boot & Reset");

  ui_create_switch_row(content, "Stealth Boot", LV_SYMBOL_EYE_CLOSE,
                       g_state.stealth_boot, on_stealth_boot_change);

  ui_create_button_row(content, "Reiniciar", LV_SYMBOL_REFRESH, "Reboot",
                       on_restart_click);

  ui_create_button_row(content, "Reset Fábrica", LV_SYMBOL_TRASH, "RESET (3x)",
                       on_factory_reset_click);

  // === SEÇÃO FUSO HORÁRIO ===
  ui_create_section_header(content, "Horário");

  // Gerar opções de fuso horário
  char tz_options[256] = "";
  for (int i = -12; i <= 14; i++) {
    char opt[10];
    snprintf(opt, sizeof(opt), "UTC%+d", i);
    strcat(tz_options, opt);
    if (i < 14)
      strcat(tz_options, "\n");
  }
  ui_create_dropdown_row(content, "Fuso Horário", LV_SYMBOL_BELL, tz_options,
                         g_state.timezone_offset + 12, on_timezone_change);

  // === SEÇÃO INFO (READ-ONLY) ===
  ui_create_section_header(content, "Informações");

  // Firmware version
  const esp_app_desc_t *app_desc = esp_ota_get_app_description();
  char fw_info[64];
  snprintf(fw_info, sizeof(fw_info), "v%s (%s)", app_desc->version,
           app_desc->date);
  lbl_firmware =
      ui_create_info_row(content, "Firmware", LV_SYMBOL_DOWNLOAD, fw_info);

  // Storage
  char storage_info[64];
  size_t total_lfs = 0, used_lfs = 0;
  if (LittleFS.begin()) {
    total_lfs = LittleFS.totalBytes();
    used_lfs = LittleFS.usedBytes();
  }

  uint64_t total_sd = 0, used_sd = 0;
  if (SD_MMC.begin()) {
    total_sd = SD_MMC.totalBytes();
    used_sd = SD_MMC.usedBytes();
  }

  snprintf(storage_info, sizeof(storage_info), "LFS: %dKB | SD: %lluMB",
           (int)((total_lfs - used_lfs) / 1024),
           (unsigned long long)((total_sd - used_sd) / 1024 / 1024));
  lbl_storage =
      ui_create_info_row(content, "Storage", LV_SYMBOL_DRIVE, storage_info);

  // Temperatura
  char temp_info[32];
  float temp = temperatureRead();
  snprintf(temp_info, sizeof(temp_info), "%.1f°C", temp);
  lbl_temp =
      ui_create_info_row(content, "Temp. Chip", LV_SYMBOL_CHARGE, temp_info);
}

// ============================================================
// API PÚBLICA
// ============================================================
void ui_settings_system_show() {
  if (!scr_system) {
    create_screen();
  }
  is_active = true;
  lv_scr_load(scr_system);
}

void ui_settings_system_hide() {
  is_active = false;
  if (scr_system) {
    lv_obj_del(scr_system);
    scr_system = nullptr;
    lbl_firmware = nullptr;
    lbl_storage = nullptr;
    lbl_temp = nullptr;
  }
}

bool ui_settings_system_is_active() { return is_active; }
