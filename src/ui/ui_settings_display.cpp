/**
 * @file ui_settings_display.cpp
 * @brief Submenu de configurações de Display & Som (12 opções)
 *
 * Opções:
 * 1. Brilho da tela (0-100%)
 * 2. Tempo até apagar tela (5s → never)
 * 3. Modo Always On
 * 4. Tema do dragão (12 temas)
 * 5. Cor dos olhos (32 cores neon)
 * 6. Tamanho do dragão
 * 7. Volume buzzer (0-100%)
 * 8. Som ao capturar handshake
 * 9. Vibração ao ligar
 * 10. Inverter cores (stealth)
 * 11. Mostrar FPS
 * 12. Fonte do sistema
 */

#include "ui_settings_display.h"
#include "../core/config_manager.h"
#include "../core/globals.h"
#include "../hardware/audio_driver.h"
#include "../hardware/system_hardware.h"
#include "modes/minimal_mode.h" // For Item 49: Compact Mode
#include "ui_settings.h"
#include "ui_themes.h"
#include "widgets/ui_settings_widgets.h"


static lv_obj_t *scr_display = nullptr;
static bool is_active = false;

// ============================================================
// CALLBACKS
// ============================================================
static void on_brightness_change(int32_t value) {
  g_state.screen_brightness = value;
  sys_hw.setDisplayBrightness(value);
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Brilho: %ld\n", value);
}

static void on_timeout_change(uint16_t selected) {
  static const uint8_t timeout_values[] = {5,  10,  15, 30,
                                           60, 120, 0}; // 0 = never
  g_state.screen_timeout_s = timeout_values[selected];
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Timeout: %ds\n", g_state.screen_timeout_s);
}

static void on_always_on_change(bool checked) {
  g_state.always_on = checked;
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Always On: %s\n", checked ? "ON" : "OFF");
}

static void on_dragon_theme_change(uint16_t selected) {
  g_state.dragon_theme = selected;
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Dragon Theme: %d\n", selected);
}

static void on_eye_color_change(uint16_t idx) {
  g_state.dragon_eye_color = idx;
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Eye Color: %d (0x%06X)\n", idx, neon_colors[idx]);

  // Item 48: Dragon Color defines Theme
  // Update dynamic theme colors instantly
  ui_update_theme_from_dragon_color(neon_colors[idx]);

  // Refresh Screen to apply new theme if current theme is Custom/Adaptive
  lv_obj_invalidate(lv_scr_act());
}

static void on_dragon_size_change(uint16_t selected) {
  g_state.dragon_size = selected;
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Dragon Size: %d\n", selected);
}

static void on_buzzer_volume_change(int32_t value) {
  g_state.buzzer_volume = value;
  audioDriver.playSound(SOUND_CLICK); // Feedback
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Buzzer Volume: %ld\n", value);
}

static void on_sound_handshake_change(bool checked) {
  g_state.sound_on_handshake = checked;
  if (checked)
    audioDriver.playSound(SOUND_SUCCESS);
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Sound on Handshake: %s\n", checked ? "ON" : "OFF");
}

static void on_vibrate_boot_change(bool checked) {
  g_state.vibrate_on_boot = checked;
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Vibrate on Boot: %s\n", checked ? "ON" : "OFF");
}

static void on_invert_colors_change(bool checked) {
  g_state.invert_colors = checked;
  
  // Nota: Inversão de cores será aplicada no próximo refresh via tema
  // SystemHardware não tem setDisplayInvert, aplicamos via flag
  lv_obj_invalidate(lv_scr_act());
  
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Invert Colors: %s\n", checked ? "ON" : "OFF");
}

static void on_show_fps_change(bool checked) {
  g_state.show_fps = checked;
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] Show FPS: %s\n", checked ? "ON" : "OFF");
}

static void on_font_change(uint16_t selected) {
  g_state.system_font = selected;
  
  // Aplica nova fonte invalidando tela atual
  // Font será aplicada no próximo ciclo de renderização
  lv_obj_invalidate(lv_scr_act());
  
  config_manager.saveDisplaySettings();
  Serial.printf("[CFG] System Font: %d\n", selected);
}

// Item 49: Implement "Compact Mode"
static void on_compact_mode_change(bool checked) {
  if (checked) {
    minimal_mode.enter();
  } else {
    minimal_mode.exit();
  }
}

static void on_back_click(lv_event_t *e) {
  ui_settings_display_hide();
  ui_settings_show();
}

// ============================================================
// CRIAÇÃO DA TELA
// ============================================================
static void create_screen() {
  lv_obj_t *content = ui_create_settings_screen(
      &scr_display, LV_SYMBOL_EYE_OPEN " Display & Som", on_back_click);

  // === BRILHO ===
  ui_create_slider_row(content, "Brilho", LV_SYMBOL_EYE_OPEN, 10, 255,
                       g_state.screen_brightness, on_brightness_change);

  // === TIMEOUT ===
  ui_create_dropdown_row(content, "Timeout Tela", LV_SYMBOL_POWER,
                         "5s\n10s\n15s\n30s\n60s\n2min\nNunca",
                         g_state.screen_timeout_s == 0    ? 6
                         : g_state.screen_timeout_s <= 5  ? 0
                         : g_state.screen_timeout_s <= 10 ? 1
                         : g_state.screen_timeout_s <= 15 ? 2
                         : g_state.screen_timeout_s <= 30 ? 3
                         : g_state.screen_timeout_s <= 60 ? 4
                                                          : 5,
                         on_timeout_change);

  // === COMPACT MODE (Item 49) ===
  ui_create_switch_row(content, "Compact Mode", LV_SYMBOL_MINUS,
                       minimal_mode.isActive(), on_compact_mode_change);

  // === ALWAYS ON ===
  ui_create_switch_row(content, "Always On", LV_SYMBOL_EYE_OPEN,
                       g_state.always_on, on_always_on_change);

  // === TEMA DO DRAGÃO ===
  ui_create_dropdown_row(content, "Tema Dragão", LV_SYMBOL_IMAGE,
                         "Hacker Dark\nCyberPunk\nMatrix\nOcean\nBlood\nDark "
                         "Dragon\nTrue Black\nRetro Miami\nIce "
                         "Blue\nStealth\nHacker Hollywood\nDragon Link",
                         g_state.dragon_theme, on_dragon_theme_change);

  // === COR DOS OLHOS ===
  ui_create_color_picker(content, "Cor dos Olhos", g_state.dragon_eye_color,
                         on_eye_color_change);

  // === TAMANHO DRAGÃO ===
  ui_create_dropdown_row(content, "Tamanho Dragão", LV_SYMBOL_IMAGE,
                         "Pequeno\nMédio\nGrande", g_state.dragon_size,
                         on_dragon_size_change);

  // === VOLUME BUZZER ===
  ui_create_slider_row(content, "Volume Buzzer", LV_SYMBOL_AUDIO, 0, 100,
                       g_state.buzzer_volume, on_buzzer_volume_change);

  // === SOM AO CAPTURAR ===
  ui_create_switch_row(content, "Som Handshake", LV_SYMBOL_AUDIO,
                       g_state.sound_on_handshake, on_sound_handshake_change);

  // === VIBRAÇÃO BOOT ===
  ui_create_switch_row(content, "Vibrar ao Ligar", LV_SYMBOL_BELL,
                       g_state.vibrate_on_boot, on_vibrate_boot_change);

  // === INVERTER CORES ===
  ui_create_switch_row(content, "Inverter Cores", LV_SYMBOL_IMAGE,
                       g_state.invert_colors, on_invert_colors_change);

  // === MOSTRAR FPS ===
  ui_create_switch_row(content, "Mostrar FPS", LV_SYMBOL_CHARGE,
                       g_state.show_fps, on_show_fps_change);

  // === FONTE SISTEMA ===
  ui_create_dropdown_row(content, "Fonte", LV_SYMBOL_KEYBOARD,
                         "Neuropol\nPixel\nOCR-A", g_state.system_font,
                         on_font_change);
}

// ============================================================
// API PÚBLICA
// ============================================================
void ui_settings_display_show() {
  if (!scr_display) {
    create_screen();
  }
  is_active = true;
  lv_scr_load(scr_display);
}

void ui_settings_display_hide() {
  is_active = false;
  if (scr_display) {
    lv_obj_del(scr_display);
    scr_display = nullptr;
  }
}

bool ui_settings_display_is_active() { return is_active; }
