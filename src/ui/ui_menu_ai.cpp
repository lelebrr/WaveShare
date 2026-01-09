/**
 * @file ui_menu_ai.cpp
 * @brief Implementação do submenu de IA
 */

#include "ui_menu_ai.h"
#include "../ai/neura9_inference.h"
#include "../core/globals.h"
#include "ui_home.h"


static lv_obj_t *_screen = nullptr;
static lv_obj_t *_statusLabel = nullptr;
static lv_obj_t *_modeLabel = nullptr;
static bool _initialized = false;

#define AI_COLOR_BG lv_color_hex(0x0a0a1a)
#define AI_COLOR_PANEL lv_color_hex(0x12122a)
#define AI_COLOR_ACCENT lv_color_hex(0xff00ff)
#define AI_COLOR_TEXT lv_color_hex(0xcccccc)

static void btn_back_cb(lv_event_t *e) {
  ui_menu_ai_hide();
  ui_home_show();
}

static void btn_mode_toggle_cb(lv_event_t *e) {
  static bool autoMode = true;
  autoMode = !autoMode;
  if (_modeLabel) {
    lv_label_set_text(_modeLabel, autoMode ? "Modo: AUTO" : "Modo: MANUAL");
  }
}

static void btn_history_cb(lv_event_t *e) {
  static const char *history_mock[] = {"1. Handshake (WPA2)", "2. Voice Cmd: Status", "3. Anomaly: Deauth", NULL};
  
  lv_obj_t *mbox = lv_msgbox_create(NULL, "Detection History", "", NULL, true);
  lv_obj_set_size(mbox, 200, 160);
  lv_obj_center(mbox);
  
  lv_obj_t *txt = lv_label_create(mbox);
  lv_label_set_text(txt, "Last 3 detections:\n\n1. Handshake (WPA2)\n2. Voice Cmd: Status\n3. Anomaly: Deauth");
  lv_obj_align(txt, LV_ALIGN_TOP_LEFT, 0, 0);
}

static void btn_info_cb(lv_event_t *e) {
  lv_obj_t *mbox = lv_msgbox_create(NULL, "Model Info", "NEURA9 v1.2\nType: TFLite (Quantized)\nInput: 128x1 features\nClasses: 4\nAccuracy: 94.2%", NULL, true);
  lv_obj_center(mbox);
}

static void create_menu_item(lv_obj_t *parent, const char *icon,
                             const char *text, lv_event_cb_t cb) {
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_width(btn, LV_PCT(100));
  lv_obj_set_height(btn, 60); // Tip 7: >44px for touch
  lv_obj_set_style_bg_color(btn, AI_COLOR_PANEL, 0);
  lv_obj_set_style_radius(btn, 12, 0);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_border_color(btn, AI_COLOR_ACCENT, 0);
  lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *iconLbl = lv_label_create(btn);
  lv_label_set_text(iconLbl, icon);
  lv_obj_set_style_text_font(iconLbl, &lv_font_montserrat_20, 0); // Larger
  lv_obj_align(iconLbl, LV_ALIGN_LEFT_MID, 10, 0);

  lv_obj_t *textLbl = lv_label_create(btn);
  lv_label_set_text(textLbl, text);
  lv_obj_set_style_text_color(textLbl, AI_COLOR_TEXT, 0);
  lv_obj_set_style_text_font(textLbl, &lv_font_montserrat_16, 0); // Readable
  lv_obj_align(textLbl, LV_ALIGN_LEFT_MID, 40, 0);
}

void ui_menu_ai_init() {
  if (_initialized) return;

  // Use new helper for standard container
  lv_obj_t* container = ui_create_scrollable_menu_container(NULL, "NEURA9 AI");
  _screen = lv_obj_get_parent(lv_obj_get_parent(container)); // Parent of container is the screen background? No, helper returns container.
  // Wait, helper returns container which is child of "parent". 
  // If we pass NULL to helper, we need to handle screen creation?
  // Let's adjust usage: Helper creates container ON parent.
  
  _screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(_screen, AI_COLOR_BG, 0);
  
  lv_obj_t* scroll_cont = ui_create_scrollable_menu_container(_screen, "NEURA9 AI");

  // Back Button (Fixed)
  ui_create_back_btn(_screen, btn_back_cb);

  // Status box (First item in scroll)
  lv_obj_t *statusBox = lv_obj_create(scroll_cont);
  lv_obj_set_size(statusBox, LV_PCT(100), 80);
  lv_obj_set_style_bg_color(statusBox, AI_COLOR_PANEL, 0);
  lv_obj_set_style_radius(statusBox, 12, 0);
  lv_obj_set_style_border_width(statusBox, 2, 0);
  lv_obj_set_style_border_color(statusBox, AI_COLOR_ACCENT, 0);
  lv_obj_clear_flag(statusBox, LV_OBJ_FLAG_SCROLLABLE);

  _statusLabel = lv_label_create(statusBox);
  lv_label_set_text(_statusLabel, "IA Carregada");
  lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(_statusLabel, lv_color_hex(0x00ff88), 0);
  lv_obj_align(_statusLabel, LV_ALIGN_TOP_MID, 0, 10);

  _modeLabel = lv_label_create(statusBox);
  lv_label_set_text(_modeLabel, "Modo: AUTO");
  lv_obj_set_style_text_color(_modeLabel, AI_COLOR_TEXT, 0);
  lv_obj_align(_modeLabel, LV_ALIGN_BOTTOM_MID, 0, -10);

  // Menu items (Auto stacked by Flex)
  create_menu_item(scroll_cont, "M", "Alternar AUTO/MANUAL", btn_mode_toggle_cb);
  create_menu_item(scroll_cont, "H", "Historico", btn_history_cb);
  create_menu_item(scroll_cont, "I", "Info do Modelo", btn_info_cb);

  _initialized = true;
}

void ui_menu_ai_show() {
  if (!_initialized)
    ui_menu_ai_init();
  if (_screen)
    lv_scr_load(_screen);
}

void ui_menu_ai_hide() {
  if (_screen) {
    lv_obj_del(_screen);
    _screen = nullptr;
    _statusLabel = nullptr;
    _modeLabel = nullptr;
    _initialized = false;
  }
}
