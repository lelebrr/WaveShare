/**
 * @file ui_menu_ble.cpp
 * @brief Implementação do submenu BLE
 */

#include "ui_menu_ble.h"
#include "../core/globals.h"
#include "../hardware/ble_driver.h"
#include "ui_home.h"

static lv_obj_t *_screen = nullptr;
static bool _initialized = false;

#define BLE_COLOR_BG lv_color_hex(0x0a0a1a)
#define BLE_COLOR_PANEL lv_color_hex(0x12122a)
#define BLE_COLOR_ACCENT lv_color_hex(0x00ccff)
#define BLE_COLOR_TEXT lv_color_hex(0xcccccc)

static void btn_back_cb(lv_event_t *e) {
  ui_menu_ble_hide();
  ui_home_show();
}

static void btn_apple_spam_cb(lv_event_t *e) {
  Serial.println("[BLE] Apple Spam");
  ble_driver.startSpam(BLE_SPAM_APPLE);
  g_state.ble_enabled = true;
}

static void btn_windows_spam_cb(lv_event_t *e) {
  Serial.println("[BLE] Windows Swift Pair");
  ble_driver.startSpam(BLE_SPAM_WINDOWS);
  g_state.ble_enabled = true;
}

static void btn_google_spam_cb(lv_event_t *e) {
  Serial.println("[BLE] Google Fast Pair");
  ble_driver.startSpam(BLE_SPAM_GOOGLE);
  g_state.ble_enabled = true;
}

static void btn_samsung_spam_cb(lv_event_t *e) {
  Serial.println("[BLE] Samsung Buds");
  ble_driver.startSpam(BLE_SPAM_SAMSUNG);
  g_state.ble_enabled = true;
}

static void btn_rickroll_cb(lv_event_t *e) {
  Serial.println("[BLE] RickRoll Start");
  ble_driver.startSpam(BLE_SPAM_RICKROLL);
  g_state.ble_enabled = true;
}

static void stop_nuke_timer_cb(lv_timer_t *t) {
  Serial.println("[BLE] NUKE COMPLETE - Stopping");
  ble_driver.stopSpam();
  g_state.ble_enabled = false;
  // Feedback visual/sonoro extra?
}

static void btn_nuke_cb(lv_event_t *e) {
  Serial.println("[BLE] NUKE STARTED (30s)");
  ble_driver.startSpam(BLE_SPAM_KITCHEN_SINK);
  g_state.ble_enabled = true;
  lv_timer_create(stop_nuke_timer_cb, 30000, nullptr); // 30s auto-stop
  // Show chaos screen to visualize the nuke
  extern void ui_ble_chaos_show();
  ui_ble_chaos_show();
}

static void btn_chaos_cb(lv_event_t *e) {
  Serial.println("[BLE] Chaos Mode UI");
  ble_driver.startSpam(BLE_SPAM_KITCHEN_SINK);
  g_state.ble_enabled = true;
  // Show dedicated chaos screen
  extern void ui_ble_chaos_show();
  ui_ble_chaos_show();
}

static void btn_stop_spam_cb(lv_event_t *e) {
  Serial.println("[BLE] Stopping spam");
  ble_driver.stopSpam();
  g_state.ble_enabled = false;
}

static void btn_scan_cb(lv_event_t *e) {
  Serial.println("[BLE] Scan devices UI");
  extern void ui_ble_scan_show();
  ui_ble_scan_show();
}

// =========================================================================
// NOVOS CALLBACKS - BLE CHAOS PACK
// =========================================================================

static void btn_airtag_moving_cb(lv_event_t *e) {
  Serial.println("[BLE] AirTag Moving Attack");
  ble_driver.startSpam(BLE_SPAM_AIRTAG_MOVING);
  g_state.ble_enabled = true;
}

static void btn_airtag_flood_cb(lv_event_t *e) {
  Serial.println("[BLE] AirTag Flood (FindMy)");
  ble_driver.startSpam(BLE_SPAM_AIRTAG_FLOOD);
  g_state.ble_enabled = true;
}

static void btn_ibeacon_cb(lv_event_t *e) {
  Serial.println("[BLE] iBeacon Flood");
  ble_driver.startSpam(BLE_SPAM_IBEACON_FLOOD);
  g_state.ble_enabled = true;
}

static void btn_eddystone_cb(lv_event_t *e) {
  Serial.println("[BLE] Eddystone Attack");
  ble_driver.startSpam(BLE_SPAM_EDDYSTONE);
  g_state.ble_enabled = true;
}

static void btn_easter_egg_cb(lv_event_t *e) {
  Serial.println("[BLE] Easter Egg Mode");
  ble_driver.startSpam(BLE_SPAM_EASTER_EGG);
  g_state.ble_enabled = true;
}

static void btn_silent_cb(lv_event_t *e) {
  Serial.println("[BLE] Silent Chaos Mode");
  ble_driver.startSpam(BLE_SPAM_SILENT);
  g_state.ble_enabled = true;
}

static void btn_dynamic_name_cb(lv_event_t *e) {
  Serial.println("[BLE] Dynamic Name Spam");
  ble_driver.startSpam(BLE_SPAM_DYNAMIC_NAME);
  g_state.ble_enabled = true;
}

static void create_menu_item(lv_obj_t *parent, const char *icon,
                             const char *text, lv_event_cb_t cb,
                             uint32_t accentColor = 0x00ccff) {
  lv_obj_t *btn = lv_btn_create(parent);
  lv_obj_set_width(btn, LV_PCT(100));
  lv_obj_set_height(btn, 60);
  lv_obj_set_style_bg_color(btn, BLE_COLOR_PANEL, 0);
  lv_obj_set_style_radius(btn, 12, 0);
  lv_obj_set_style_border_width(btn, 1, 0);
  lv_obj_set_style_border_color(btn, lv_color_hex(accentColor), 0);
  lv_obj_set_style_border_opa(btn, LV_OPA_50, 0);
  lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *iconLbl = lv_label_create(btn);
  lv_label_set_text(iconLbl, icon);
  lv_obj_set_style_text_font(iconLbl, &lv_font_montserrat_20, 0);
  lv_obj_align(iconLbl, LV_ALIGN_LEFT_MID, 10, 0);

  lv_obj_t *textLbl = lv_label_create(btn);
  lv_label_set_text(textLbl, text);
  lv_obj_set_style_text_color(textLbl, BLE_COLOR_TEXT, 0);
  lv_obj_set_style_text_font(textLbl, &lv_font_montserrat_16, 0);
  lv_obj_align(textLbl, LV_ALIGN_LEFT_MID, 50, 0); // Need more space for icon?
}

void ui_menu_ble_init() {
  if (_initialized) return;

  _screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(_screen, BLE_COLOR_BG, 0);

  // Scrollable Container
  lv_obj_t* cont = ui_create_scrollable_menu_container(_screen, "Bluetooth");

  // Back Button
  ui_create_back_btn(_screen, btn_back_cb);

  // Ataques Clássicos
  create_menu_item(cont, "A", "Apple Popup Spam", btn_apple_spam_cb);
  create_menu_item(cont, "W", "Windows Swift Pair", btn_windows_spam_cb);
  create_menu_item(cont, "G", "Google Fast Pair", btn_google_spam_cb);
  create_menu_item(cont, "S", "Samsung Buds Spam", btn_samsung_spam_cb);

  // Novos Ataques - BLE Chaos Pack
  create_menu_item(cont, "T", "AirTag Moving", btn_airtag_moving_cb, 0x00ff88);
  create_menu_item(cont, "F", "FindMy Flood", btn_airtag_flood_cb, 0x00ff88);
  create_menu_item(cont, "B", "iBeacon Flood", btn_ibeacon_cb, 0x88ff00);
  create_menu_item(cont, "E", "Eddystone Spam", btn_eddystone_cb, 0x88ff00);

  // Fun & Special
  create_menu_item(cont, "R", "RickRoll BLE", btn_rickroll_cb, 0xffaa00);
  create_menu_item(cont, "L", "Easter Egg Mode", btn_easter_egg_cb, 0xff88ff);
  create_menu_item(cont, "D", "Dynamic Name", btn_dynamic_name_cb, 0xffff00);

  // Modes
  create_menu_item(cont, "!!!", "NUKE (30s)", btn_nuke_cb, 0xff0000);
  create_menu_item(cont, "!", "CHAOS MODE", btn_chaos_cb, 0xff00ff);
  create_menu_item(cont, "-", "Silent Chaos", btn_silent_cb, 0x666666);

  // Tools
  create_menu_item(cont, "?", "Scan Dispositivos", btn_scan_cb);
  create_menu_item(cont, "X", "Parar Spam", btn_stop_spam_cb, 0xff3333);

  _initialized = true;
}

void ui_menu_ble_show() {
  if (!_initialized)
    ui_menu_ble_init();
  if (_screen)
    lv_scr_load(_screen);
}

void ui_menu_ble_hide() {}
