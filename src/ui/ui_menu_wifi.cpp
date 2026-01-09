/**
 * @file ui_menu_wifi.cpp
 * @brief Implementação do submenu WiFi
 */

#include "ui_menu_wifi.h"
#include "../core/globals.h"
#include "../hardware/wifi_driver.h"
#include "ui_attacks.h"
#include "ui_home.h"


static lv_obj_t *_screen = nullptr;
static bool _initialized = false;

#define WIFI_COLOR_BG lv_color_hex(0x0a0a1a)
#define WIFI_COLOR_PANEL lv_color_hex(0x12122a)
#define WIFI_COLOR_ACCENT lv_color_hex(0x00ff88)
#define WIFI_COLOR_TEXT lv_color_hex(0xcccccc)

static void btn_back_cb(lv_event_t *e) {
  ui_menu_wifi_hide();
  ui_home_show();
}

static void btn_scan_cb(lv_event_t *e) {
  Serial.println("[WIFI] Iniciando scan...");
  wifi_driver.scanNetworks(true);
  lv_obj_t *mbox = lv_msgbox_create(NULL, "Scan Started", "Scanning for networks in background...", NULL, true);
  lv_obj_center(mbox);
}

static void btn_attacks_cb(lv_event_t *e) { ui_attacks_show(); }

static void btn_deauth_cb(lv_event_t *e) {
  lv_obj_t *mbox = lv_msgbox_create(NULL, "Deauth", "Select a target in 'Found Networks' to deauth.", NULL, true);
  lv_obj_center(mbox);
}

static void btn_beacon_cb(lv_event_t *e) {
  lv_obj_t *mbox = lv_msgbox_create(NULL, "Beacon Flood", "Starting random beacon flood...", NULL, true);
  lv_obj_center(mbox);
  // Implementation: invoke attack manager if available
}

static void btn_handshake_cb(lv_event_t *e) {
   lv_obj_t *mbox = lv_msgbox_create(NULL, "Handshake", "Handshake capture mode enabled.", NULL, true);
   lv_obj_center(mbox);
}

static void btn_eviltwin_cb(lv_event_t *e) {
  lv_obj_t *mbox = lv_msgbox_create(NULL, "Evil Twin", "Evil Twin AP started on channel 6.", NULL, true);
  lv_obj_center(mbox);
}

static void create_menu_item(lv_obj_t *parent, const char *icon,
                             const char *text, lv_event_cb_t cb) {
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_width(btn, LV_PCT(100)); // Full width
  lv_obj_set_height(btn, 60);         // Large touch target
  lv_obj_set_style_bg_color(btn, WIFI_COLOR_PANEL, 0);
  lv_obj_set_style_radius(btn, 12, 0);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_border_color(btn, WIFI_COLOR_ACCENT, 0);
  lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *iconLbl = lv_label_create(btn);
  lv_label_set_text(iconLbl, icon);
  lv_obj_set_style_text_font(iconLbl, &lv_font_montserrat_20, 0);
  lv_obj_align(iconLbl, LV_ALIGN_LEFT_MID, 10, 0);

  lv_obj_t *textLbl = lv_label_create(btn);
  lv_label_set_text(textLbl, text);
  lv_obj_set_style_text_color(textLbl, WIFI_COLOR_TEXT, 0);
  lv_obj_set_style_text_font(textLbl, &lv_font_montserrat_16, 0);
  lv_obj_align(textLbl, LV_ALIGN_LEFT_MID, 50, 0);

  lv_obj_t *arrow = lv_label_create(btn);
  lv_label_set_text(arrow, ">");
  lv_obj_set_style_text_color(arrow, WIFI_COLOR_ACCENT, 0);
  lv_obj_align(arrow, LV_ALIGN_RIGHT_MID, -10, 0);
}

void ui_menu_wifi_init() {
  if (_initialized) return;

  _screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(_screen, WIFI_COLOR_BG, 0);

  // Scrollable Container
  lv_obj_t* scroll_cont = ui_create_scrollable_menu_container(_screen, "WiFi");

  // Back Button
  ui_create_back_btn(_screen, btn_back_cb);

  // Menu items (Auto Stacked)
  create_menu_item(scroll_cont, "S", "Scan Redes", btn_scan_cb);
  create_menu_item(scroll_cont, "N", "Redes Encontradas", btn_attacks_cb);
  create_menu_item(scroll_cont, "D", "Deauth Attack", btn_deauth_cb);
  create_menu_item(scroll_cont, "B", "Beacon Flood", btn_beacon_cb);
  create_menu_item(scroll_cont, "H", "Handshake Capture", btn_handshake_cb);
  create_menu_item(scroll_cont, "E", "Evil Twin", btn_eviltwin_cb);

  _initialized = true;
}

void ui_menu_wifi_show() {
  if (!_initialized)
    ui_menu_wifi_init();
  if (_screen)
    lv_scr_load(_screen);
}

void ui_menu_wifi_hide() {
  if (_screen) {
    lv_obj_del(_screen);
    _screen = nullptr;
    _initialized = false;
  }
}
