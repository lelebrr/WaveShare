/**
 * @file ui_settings_dragon.cpp
 * @brief Submenu de configurações de Personalidade do Dragão (6 opções)
 *
 * Opções:
 * 1. Nome do dragão (editável)
 * 2. Personalidade (Aggro/Chill/Psycho/Cute/Hacker/Sleepy)
 * 3. Fala do dragão (On/Off + idioma)
 * 4. Frequência de falas
 * 5. Reações exageradas
 * 6. Modo Pet Only
 */

#include "ui_settings_dragon.h"
#include "../core/config_manager.h"
#include "../core/globals.h"
#include "../hardware/audio_driver.h"
#include "ui_main.h"
#include "ui_settings.h"
#include "ui_themes.h"
#include "widgets/ui_settings_widgets.h"

static lv_obj_t *scr_dragon = nullptr;
static lv_obj_t *ta_name = nullptr;
static lv_obj_t *kb = nullptr;
static bool is_active = false;

// ============================================================
// CALLBACKS
// ============================================================
static void on_name_change(lv_event_t *e) {
  lv_obj_t *ta = lv_event_get_target(e);
  const char *txt = lv_textarea_get_text(ta);
  strncpy(g_state.dragon_name, txt, 15);
  g_state.dragon_name[15] = '\0';
  config_manager.saveDragonSettings();
  Serial.printf("[CFG] Dragon Name: %s\n", g_state.dragon_name);
}

static void on_keyboard_ready(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *ta = (lv_obj_t *)lv_event_get_user_data(e);

  if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
    // Esconde teclado
    if (kb) {
      lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_clear_state(ta, LV_STATE_FOCUSED);
  }
}

static void on_name_focus(lv_event_t *e) {
  // Mostra teclado virtual
  if (kb) {
    lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_textarea(kb, ta_name);
  }
}

static void on_personality_change(uint16_t selected) {
  g_state.dragon_personality = selected;
  config_manager.saveDragonSettings();

  const char *personality_names[] = {"Aggro", "Chill",  "Psycho",
                                     "Cute",  "Hacker", "Sleepy"};
  Serial.printf("[CFG] Dragon Personality: %s\n", personality_names[selected]);

  // Som especial para cada personalidade
  audioDriver.playSound(SOUND_SUCCESS);
}

static void on_talk_enabled_change(bool checked) {
  g_state.dragon_talk_enabled = checked;
  config_manager.saveDragonSettings();
  Serial.printf("[CFG] Dragon Talk: %s\n", checked ? "ON" : "OFF");
}

static void on_talk_lang_change(uint16_t selected) {
  g_state.dragon_talk_lang = selected;
  config_manager.saveDragonSettings();
  Serial.printf("[CFG] Dragon Language: %s\n", selected == 0 ? "PT-BR" : "EN");
}

static void on_talk_freq_change(uint16_t selected) {
  g_state.dragon_talk_freq = selected;
  config_manager.saveDragonSettings();

  const char *freq_names[] = {"Nunca", "Raro", "Normal", "Spam"};
  Serial.printf("[CFG] Dragon Talk Freq: %s\n", freq_names[selected]);
}

static void on_exaggerated_change(bool checked) {
  g_state.dragon_exaggerated = checked;
  if (checked) {
    audioDriver.playSound(SOUND_ALERT);
  }
  config_manager.saveDragonSettings();
  Serial.printf("[CFG] Dragon Exaggerated: %s\n", checked ? "ON" : "OFF");
}

static void on_pet_only_change(bool checked) {
  g_state.pet_only_mode = checked;
  config_manager.saveDragonSettings();
  Serial.printf("[CFG] Pet Only Mode: %s\n", checked ? "ON" : "OFF");

  if (checked) {
    // Vai direto para modo pet
    ui_settings_dragon_hide();
    // TODO: Ativar modo pet only
  }
}

static void on_back_click(lv_event_t *e) {
  ui_settings_dragon_hide();
  ui_settings_show();
}

// ============================================================
// CRIAÇÃO DA TELA
// ============================================================
static void create_screen() {
  lv_obj_t *content =
      ui_create_settings_screen(&scr_dragon, "🐉 Dragão", on_back_click);

  // === NOME DO DRAGÃO ===
  ui_create_section_header(content, "Identidade");

  // Container para nome
  lv_obj_t *name_row = lv_obj_create(content);
  lv_obj_set_size(name_row, lv_pct(100), 60);
  lv_obj_set_style_bg_color(name_row, THEME_PANEL, 0);
  lv_obj_set_style_border_width(name_row, 0, 0);
  lv_obj_set_style_radius(name_row, 10, 0);
  lv_obj_set_style_pad_all(name_row, 10, 0);
  lv_obj_clear_flag(name_row, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *name_lbl = lv_label_create(name_row);
  lv_label_set_text(name_lbl, "Nome:");
  lv_obj_set_style_text_color(name_lbl, THEME_TEXT, 0);
  lv_obj_align(name_lbl, LV_ALIGN_LEFT_MID, 0, 0);

  ta_name = lv_textarea_create(name_row);
  lv_textarea_set_one_line(ta_name, true);
  lv_textarea_set_max_length(ta_name, 15);
  lv_textarea_set_text(ta_name, g_state.dragon_name);
  lv_obj_set_size(ta_name, 150, 36);
  lv_obj_align(ta_name, LV_ALIGN_RIGHT_MID, 0, 0);
  lv_obj_set_style_bg_color(ta_name, lv_color_hex(0x1a1a3e), 0);
  lv_obj_set_style_text_color(ta_name, THEME_ACCENT, 0);
  lv_obj_set_style_border_color(ta_name, THEME_PRIMARY, 0);
  lv_obj_add_event_cb(ta_name, on_name_change, LV_EVENT_VALUE_CHANGED, nullptr);
  lv_obj_add_event_cb(ta_name, on_name_focus, LV_EVENT_FOCUSED, nullptr);

  // === PERSONALIDADE ===
  ui_create_dropdown_row(content, "Personalidade", LV_SYMBOL_EDIT,
                         "Aggro\nChill\nPsycho\nCute\nHacker\nSleepy",
                         g_state.dragon_personality, on_personality_change);

  // === SEÇÃO FALAS ===
  ui_create_section_header(content, "Comunicação");

  ui_create_switch_row(content, "Falas Ativas", LV_SYMBOL_AUDIO,
                       g_state.dragon_talk_enabled, on_talk_enabled_change);

  ui_create_dropdown_row(content, "Idioma", LV_SYMBOL_LIST, "PT-BR\nEnglish",
                         g_state.dragon_talk_lang, on_talk_lang_change);

  ui_create_dropdown_row(content, "Frequência", LV_SYMBOL_REFRESH,
                         "Nunca\nRaro\nNormal\nSpam", g_state.dragon_talk_freq,
                         on_talk_freq_change);

  // === SEÇÃO COMPORTAMENTO ===
  ui_create_section_header(content, "Comportamento");

  ui_create_switch_row(content, "Reações Exageradas", LV_SYMBOL_OK,
                       g_state.dragon_exaggerated, on_exaggerated_change);

  ui_create_switch_row(content, "Modo Pet Only", LV_SYMBOL_IMAGE,
                       g_state.pet_only_mode, on_pet_only_change);

  // === TECLADO VIRTUAL ===
  kb = lv_keyboard_create(scr_dragon);
  lv_obj_set_size(kb, LCD_WIDTH, LCD_HEIGHT / 2);
  lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
  lv_keyboard_set_textarea(kb, ta_name);
  lv_obj_add_event_cb(kb, on_keyboard_ready, LV_EVENT_ALL, ta_name);
}

// ============================================================
// API PÚBLICA
// ============================================================
void ui_settings_dragon_show() {
  if (!scr_dragon) {
    create_screen();
  }
  is_active = true;
  lv_scr_load(scr_dragon);
}

void ui_settings_dragon_hide() {
  is_active = false;
  if (scr_dragon) {
    lv_obj_del(scr_dragon);
    scr_dragon = nullptr;
    ta_name = nullptr;
    kb = nullptr;
  }
}

bool ui_settings_dragon_is_active() { return is_active; }
