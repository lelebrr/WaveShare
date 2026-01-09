/**
 * @file ui_menu_voice.cpp
 * @brief Implementação do submenu de voz
 */

#include "ui_menu_voice.h"
#include "../core/globals.h"
#include "../voice/voice_assistant.h"
#include "ui_home.h"


static lv_obj_t *_screen = nullptr;
static lv_obj_t *_statusLabel = nullptr;
static lv_obj_t *_toggleBtn = nullptr;
static bool _initialized = false;

#define VOICE_COLOR_BG lv_color_hex(0x0a0a1a)
#define VOICE_COLOR_PANEL lv_color_hex(0x12122a)
#define VOICE_COLOR_ACCENT lv_color_hex(0x9933ff)
#define VOICE_COLOR_ON lv_color_hex(0x00ff88)
#define VOICE_COLOR_OFF lv_color_hex(0xff3333)
#define VOICE_COLOR_TEXT lv_color_hex(0xcccccc)

static void btn_back_cb(lv_event_t *e) {
  ui_menu_voice_hide();
  ui_home_show();
}

static void btn_toggle_cb(lv_event_t *e) {
  g_state.voice_enabled = !g_state.voice_enabled;
  voiceAssistant.setListening(g_state.voice_enabled);

  if (g_state.voice_enabled) {
    lv_obj_set_style_bg_color(_toggleBtn, VOICE_COLOR_ON, 0);
    lv_label_set_text(_statusLabel, "🎤 Assistente ATIVO");
  } else {
    lv_obj_set_style_bg_color(_toggleBtn, VOICE_COLOR_OFF, 0);
    lv_label_set_text(_statusLabel, "🔇 Assistente INATIVO");
  }
}

static void btn_test_cb(lv_event_t *e) { voiceAssistant.speak(TTS_HELLO); }

static void btn_commands_cb(lv_event_t *e) {
  Serial.println("[VOICE] Show commands list");
}

static void create_menu_item(lv_obj_t *parent, const char *icon,
                             const char *text, lv_event_cb_t cb) {
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_width(btn, LV_PCT(100)); // Full width
  lv_obj_set_height(btn, 60);
  lv_obj_set_style_bg_color(btn, VOICE_COLOR_PANEL, 0);
  lv_obj_set_style_radius(btn, 12, 0);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_border_color(btn, VOICE_COLOR_ACCENT, 0);
  lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *iconLbl = lv_label_create(btn);
  lv_label_set_text(iconLbl, icon);
  lv_obj_set_style_text_font(iconLbl, &lv_font_montserrat_20, 0);
  lv_obj_align(iconLbl, LV_ALIGN_LEFT_MID, 10, 0);

  lv_obj_t *textLbl = lv_label_create(btn);
  lv_label_set_text(textLbl, text);
  lv_obj_set_style_text_color(textLbl, VOICE_COLOR_TEXT, 0);
  lv_obj_set_style_text_font(textLbl, &lv_font_montserrat_16, 0);
  lv_obj_align(textLbl, LV_ALIGN_LEFT_MID, 50, 0);
}

void ui_menu_voice_init() {
  if (_initialized) return;

  _screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(_screen, VOICE_COLOR_BG, 0);

  // Scrollable
  lv_obj_t* cont = ui_create_scrollable_menu_container(_screen, "🎤 Assistente");

  // Back
  ui_create_back_btn(_screen, btn_back_cb);

  // Status label (In scroll)
  _statusLabel = lv_label_create(cont);
  lv_label_set_text(_statusLabel, g_state.voice_enabled
                                      ? "🎤 Assistente ATIVO"
                                      : "🔇 Assistente INATIVO");
  lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(_statusLabel, lv_color_hex(0xffffff), 0);
  // Center alignment for status/toggle
  lv_obj_set_style_align(_statusLabel, LV_ALIGN_CENTER, 0); 
  // In Flex layout, align works differently. we need to align self.
  lv_obj_set_align(_statusLabel, LV_ALIGN_CENTER);

  // Botão toggle grande
  _toggleBtn = lv_btn_create(cont);
  lv_obj_set_width(_toggleBtn, LV_PCT(80));
  lv_obj_set_height(_toggleBtn, 60);
  lv_obj_set_style_bg_color(
      _toggleBtn, g_state.voice_enabled ? VOICE_COLOR_ON : VOICE_COLOR_OFF, 0);
  lv_obj_set_style_radius(_toggleBtn, 30, 0);
  lv_obj_add_event_cb(_toggleBtn, btn_toggle_cb, LV_EVENT_CLICKED, nullptr);
  // Center in flex
  lv_obj_set_align(_toggleBtn, LV_ALIGN_CENTER);

  lv_obj_t *toggleLabel = lv_label_create(_toggleBtn);
  lv_label_set_text(toggleLabel, "ATIVAR / DESATIVAR");
  lv_obj_set_style_text_font(toggleLabel, &lv_font_montserrat_14, 0);
  lv_obj_center(toggleLabel);

  // Info
  lv_obj_t *info = lv_label_create(cont);
  lv_label_set_text(info, "Wake word: \"Hey Dragon\"");
  lv_obj_set_style_text_color(info, VOICE_COLOR_TEXT, 0);
  lv_obj_set_align(info, LV_ALIGN_CENTER);
  
  // Spacing
  lv_obj_set_style_pad_bottom(info, 20, 0);

  // Outros botões
  create_menu_item(cont, "🔊", "Testar Voz", btn_test_cb);
  create_menu_item(cont, "📋", "Lista de Comandos", btn_commands_cb);

  _initialized = true;
}

void ui_menu_voice_show() {
  if (!_initialized)
    ui_menu_voice_init();
  if (_screen)
    lv_scr_load(_screen);
}

void ui_menu_voice_hide() {}
