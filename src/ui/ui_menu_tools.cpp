/**
 * @file ui_menu_tools.cpp
 * @brief Implementação do submenu de ferramentas
 */

#include "ui_menu_tools.h"
#include "../plugins/age_tracker.h"
#include "../plugins/exp_system.h"
#include "../plugins/memtemp.h"
#include "screens/ui_handshakes_screen.h"
#include "screens/ui_stats_screen.h"
#include "ui_home.h"


static lv_obj_t *_screen = nullptr;
static bool _initialized = false;

#define TOOLS_COLOR_BG lv_color_hex(0x0a0a1a)
#define TOOLS_COLOR_PANEL lv_color_hex(0x12122a)
#define TOOLS_COLOR_ACCENT lv_color_hex(0xff8800)
#define TOOLS_COLOR_TEXT lv_color_hex(0xcccccc)

static void btn_back_cb(lv_event_t *e) {
  ui_menu_tools_hide();
  ui_home_show();
}

static void btn_handshakes_cb(lv_event_t *e) { handshakesScreen.show(); }

static void btn_stats_cb(lv_event_t *e) { statsScreen.show(); }

static void btn_sdcard_cb(lv_event_t *e) {
  Serial.println("[TOOLS] SD Card manager");
}

static void btn_sysinfo_cb(lv_event_t *e) {
  Serial.println("[TOOLS] System info");
  Serial.printf("  Free Heap: %s\n", memtempPlugin.getFormattedHeap().c_str());
  Serial.printf("  Temp: %s\n", memtempPlugin.getFormattedTemp().c_str());
  Serial.printf("  XP: %u Level %d\n", expPlugin.getTotalXP(),
                expPlugin.getLevel());
  Serial.printf("  Uptime: %s\n", agePlugin.getFormattedUptime().c_str());
}

static void create_menu_item(lv_obj_t *parent, const char *icon,
                             const char *text, lv_event_cb_t cb) {
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_width(btn, LV_PCT(100));
  lv_obj_set_height(btn, 60); // 60px target
  lv_obj_set_style_bg_color(btn, TOOLS_COLOR_PANEL, 0);
  lv_obj_set_style_radius(btn, 12, 0);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_border_color(btn, TOOLS_COLOR_ACCENT, 0);
  lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *iconLbl = lv_label_create(btn);
  lv_label_set_text(iconLbl, icon);
  lv_obj_set_style_text_font(iconLbl, &lv_font_montserrat_20, 0);
  lv_obj_align(iconLbl, LV_ALIGN_LEFT_MID, 10, 0);

  lv_obj_t *textLbl = lv_label_create(btn);
  lv_label_set_text(textLbl, text);
  lv_obj_set_style_text_color(textLbl, TOOLS_COLOR_TEXT, 0);
  lv_obj_set_style_text_font(textLbl, &lv_font_montserrat_16, 0);
  lv_obj_align(textLbl, LV_ALIGN_LEFT_MID, 50, 0);
}

void ui_menu_tools_init() {
  if (_initialized) return;

  _screen = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(_screen, TOOLS_COLOR_BG, 0);

  // Scrollable Container
  lv_obj_t* cont = ui_create_scrollable_menu_container(_screen, "Ferramentas");

  // Back Button
  ui_create_back_btn(_screen, btn_back_cb);

  // Menu items
  create_menu_item(cont, "🔐", "Handshakes Capturados", btn_handshakes_cb);
  create_menu_item(cont, "📊", "Estatísticas", btn_stats_cb);
  create_menu_item(cont, "💾", "SD Card", btn_sdcard_cb);
  create_menu_item(cont, "ℹ", "Info do Sistema", btn_sysinfo_cb);

  _initialized = true;
}

void ui_menu_tools_show() {
  if (!_initialized)
    ui_menu_tools_init();
  if (_screen)
    lv_scr_load(_screen);
}

void ui_menu_tools_hide() {}
