/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Modo Relógio Smartwatch
 * Implementação do sistema de watchfaces
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "watch_mode.h"
#include "../../core/globals.h"
#include "../../mascot/mascot_manager.h"
#include <Preferences.h>
#include <time.h>

// Global instance
WatchMode watch_mode;

// ═══════════════════════════════════════════════════════════════════════════
// STYLES
// ═══════════════════════════════════════════════════════════════════════════

static lv_style_t style_time_big;
static lv_style_t style_time_small;
static lv_style_t style_date;
static lv_style_t style_stats;

static void init_styles() {
  // Big time style
  lv_style_init(&style_time_big);
  lv_style_set_text_font(&style_time_big, &lv_font_montserrat_48);
  lv_style_set_text_color(&style_time_big, lv_color_hex(0x00FFF5));

  // Small time style
  lv_style_init(&style_time_small);
  lv_style_set_text_font(&style_time_small, &lv_font_montserrat_24);
  lv_style_set_text_color(&style_time_small, lv_color_hex(0xFFFFFF));

  // Date style
  lv_style_init(&style_date);
  lv_style_set_text_font(&style_date, &lv_font_montserrat_14);
  lv_style_set_text_color(&style_date, lv_color_hex(0x888888));

  // Stats style
  lv_style_init(&style_stats);
  lv_style_set_text_font(&style_stats, &lv_font_montserrat_12);
  lv_style_set_text_color(&style_stats, lv_color_hex(0x00FF9D));
}

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

WatchMode::WatchMode() : _active(false), _screen(nullptr), _lastUpdate(0) {
  memset(&_config, 0, sizeof(WatchConfig));
  memset(&_stats, 0, sizeof(WatchStats));

  // Defaults
  _config.watchface_id = WATCHFACE_DIGITAL;
  _config.show_seconds = true;
  _config.show_date = true;
  _config.show_battery = true;
  _config.show_wifi_status = true;
  _config.show_handshake_count = true;
  _config.background_sniffing = true;
  _config.auto_capture = false;
  _config.brightness_level = 80;
  _config.screen_timeout = 30;
}

void WatchMode::begin() {
  loadConfig();
  init_styles();
  Serial.println("[Watch] Mode initialized");
}

void WatchMode::enter() {
  if (_active)
    return;

  _active = true;
  _screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(_screen, lv_color_black(), 0);

  // Create base UI elements
  _timeLabel = lv_label_create(_screen);
  lv_obj_add_style(_timeLabel, &style_time_big, 0);
  lv_obj_align(_timeLabel, LV_ALIGN_CENTER, 0, -20);

  _dateLabel = lv_label_create(_screen);
  lv_obj_add_style(_dateLabel, &style_date, 0);
  lv_obj_align(_dateLabel, LV_ALIGN_CENTER, 0, 30);

  _statsLabel = lv_label_create(_screen);
  lv_obj_add_style(_statsLabel, &style_stats, 0);
  lv_obj_align(_statsLabel, LV_ALIGN_BOTTOM_MID, 0, -10);

  lv_scr_load_anim(_screen, LV_SCR_LOAD_ANIM_FADE_IN, 300, 0, false);

  Serial.println("[Watch] Entered watch mode");
}

void WatchMode::exit() {
  if (!_active)
    return;

  _active = false;

  if (_screen) {
    lv_obj_del(_screen);
    _screen = nullptr;
  }

  Serial.println("[Watch] Exited watch mode");
}

void WatchMode::update() {
  if (!_active || !_screen)
    return;

  uint32_t now = millis();
  if (now - _lastUpdate < 1000)
    return; // Update every second
  _lastUpdate = now;

  // Render based on current watchface
  switch ((WatchfaceType)_config.watchface_id) {
  case WATCHFACE_DIGITAL:
    renderDigital();
    break;
  case WATCHFACE_ANALOG:
    renderAnalog();
    break;
  case WATCHFACE_CYBER:
    renderCyber();
    break;
  case WATCHFACE_MASCOT:
    renderMascot();
    break;
  case WATCHFACE_HACKER:
    renderHacker();
    break;
  case WATCHFACE_MINIMAL:
    renderMinimal();
    break;
  default:
    renderDigital();
    break;
  }
}

void WatchMode::setWatchface(WatchfaceType type) {
  if (type >= WATCHFACE_COUNT)
    return;
  _config.watchface_id = type;
  saveConfig();
}

void WatchMode::nextWatchface() {
  _config.watchface_id = (_config.watchface_id + 1) % WATCHFACE_COUNT;
  saveConfig();
  animateTransition();
}

void WatchMode::prevWatchface() {
  _config.watchface_id =
      (_config.watchface_id + WATCHFACE_COUNT - 1) % WATCHFACE_COUNT;
  saveConfig();
  animateTransition();
}

void WatchMode::getTime(uint8_t &hour, uint8_t &minute, uint8_t &second) {
  time_t now = time(nullptr);
  struct tm *tm_info = localtime(&now);
  hour = tm_info->tm_hour;
  minute = tm_info->tm_min;
  second = tm_info->tm_sec;
}

void WatchMode::getDate(uint8_t &day, uint8_t &month, uint16_t &year) {
  time_t now = time(nullptr);
  struct tm *tm_info = localtime(&now);
  day = tm_info->tm_mday;
  month = tm_info->tm_mon + 1;
  year = tm_info->tm_year + 1900;
}

const char *WatchMode::getDayOfWeek() {
  static const char *days[] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"};
  time_t now = time(nullptr);
  struct tm *tm_info = localtime(&now);
  return days[tm_info->tm_wday];
}

// ═══════════════════════════════════════════════════════════════════════════
// WATCHFACE RENDERERS
// ═══════════════════════════════════════════════════════════════════════════

void WatchMode::renderDigital() {
  uint8_t h, m, s;
  getTime(h, m, s);

  static char time_str[16];
  if (_config.show_seconds) {
    sprintf(time_str, "%02d:%02d:%02d", h, m, s);
  } else {
    sprintf(time_str, "%02d:%02d", h, m);
  }
  lv_label_set_text(_timeLabel, time_str);

  if (_config.show_date) {
    uint8_t d, mo;
    uint16_t y;
    getDate(d, mo, y);
    static char date_str[32];
    sprintf(date_str, "%s, %02d/%02d/%d", getDayOfWeek(), d, mo, y);
    lv_label_set_text(_dateLabel, date_str);
  }

  if (_config.show_handshake_count) {
    static char stats_str[32];
    sprintf(stats_str, "🤝 %lu  📡 %lu", g_state.handshakes_captured,
            g_state.networks_seen);
    lv_label_set_text(_statsLabel, stats_str);
  }
}

void WatchMode::renderAnalog() {
  // For analog, we would draw clock hands
  // Simplified: just show time with analog-style text
  uint8_t h, m, s;
  getTime(h, m, s);

  static char time_str[16];
  sprintf(time_str, "%02d:%02d", h, m);
  lv_label_set_text(_timeLabel, time_str);
  lv_obj_set_style_text_color(_timeLabel, lv_color_hex(0xFFD700), 0);
}

void WatchMode::renderCyber() {
  uint8_t h, m, s;
  getTime(h, m, s);

  static char time_str[16];
  sprintf(time_str, "%02d:%02d:%02d", h, m, s);
  lv_label_set_text(_timeLabel, time_str);
  lv_obj_set_style_text_color(_timeLabel, lv_color_hex(0x00FFF5), 0);

  // Show more stats
  static char stats_str[64];
  sprintf(stats_str, "NET:%lu HSK:%lu DEA:%lu", g_state.networks_seen,
          g_state.handshakes_captured, 0UL);
  lv_label_set_text(_statsLabel, stats_str);
}

void WatchMode::renderMascot() {
  uint8_t h, m, s;
  getTime(h, m, s);

  static char time_str[32];
  sprintf(time_str, "%s  %02d:%02d",
          mascot_manager.getCurrentMascotInfo().emoji, h, m);
  lv_label_set_text(_timeLabel, time_str);

  lv_label_set_text(_dateLabel, mascot_manager.getName());
}

void WatchMode::renderHacker() {
  uint8_t h, m, s;
  getTime(h, m, s);

  static char time_str[32];
  sprintf(time_str, "[%02d:%02d:%02d]", h, m, s);
  lv_label_set_text(_timeLabel, time_str);
  lv_obj_set_style_text_color(_timeLabel, lv_color_hex(0x00FF00), 0);

  static char stats_str[48];
  sprintf(stats_str, "$ captured: %lu | scanned: %lu",
          g_state.handshakes_captured, g_state.networks_seen);
  lv_label_set_text(_statsLabel, stats_str);
  lv_obj_set_style_text_color(_statsLabel, lv_color_hex(0x00FF00), 0);
}

void WatchMode::renderMinimal() {
  uint8_t h, m, s;
  getTime(h, m, s);

  static char time_str[8];
  sprintf(time_str, "%02d:%02d", h, m);
  lv_label_set_text(_timeLabel, time_str);
  lv_obj_set_style_text_color(_timeLabel, lv_color_hex(0xFFFFFF), 0);

  lv_obj_add_flag(_dateLabel, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(_statsLabel, LV_OBJ_FLAG_HIDDEN);
}

void WatchMode::animateTransition() {
  if (_screen) {
    lv_obj_fade_out(_screen, 150, 0);
    lv_obj_fade_in(_screen, 150, 150);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// PERSISTENCE
// ═══════════════════════════════════════════════════════════════════════════

void WatchMode::saveConfig() {
  Preferences prefs;
  prefs.begin("watch", false);
  prefs.putBytes("config", &_config, sizeof(WatchConfig));
  prefs.end();
}

void WatchMode::loadConfig() {
  Preferences prefs;
  prefs.begin("watch", true);
  size_t len = prefs.getBytes("config", &_config, sizeof(WatchConfig));
  prefs.end();

  if (len != sizeof(WatchConfig)) {
    // Reset to defaults
    _config.watchface_id = WATCHFACE_DIGITAL;
    _config.show_seconds = true;
    _config.show_date = true;
    _config.show_battery = true;
    _config.show_wifi_status = true;
    _config.show_handshake_count = true;
    _config.background_sniffing = true;
    _config.auto_capture = false;
    _config.brightness_level = 80;
    _config.screen_timeout = 30;
  }
}
