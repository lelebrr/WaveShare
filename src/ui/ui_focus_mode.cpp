/**
 * @file ui_focus_mode.cpp
 * @brief Implementação dos modos Focus e Minimal
 */

#include "ui_focus_mode.h"
#include "../hardware/system_hardware.h"
#include "mascot_faces.h"
#include "ui_themes.h"
#include <time.h>


FocusMode focusMode;
StealthMode stealthMode;

// ═══════════════════════════════════════════════════════════════════════════
// FOCUS MODE
// ═══════════════════════════════════════════════════════════════════════════

FocusMode::FocusMode()
    : _screen(nullptr), _clockLabel(nullptr), _mascotLabel(nullptr),
      _batteryLabel(nullptr), _attackLabel(nullptr), _attackIndicator(nullptr),
      _enabled(false), _originalBrightness(100), _lastUpdate(0) {

  _config.showClock = true;
  _config.showMascot = true;
  _config.showBattery = true;
  _config.showAttackStatus = true;
  _config.hideNotifications = true;
  _config.reducedAnimations = true;
  _config.autoDimScreen = true;
  _config.dimBrightness = 30;
}

void FocusMode::begin() {
  // Salva brilho original
  _originalBrightness = 100; // TODO: ler do sistema
}

void FocusMode::enable() {
  if (_enabled)
    return;

  _enabled = true;
  createUI();
  applyDimming();
}

void FocusMode::disable() {
  if (!_enabled)
    return;

  _enabled = false;
  restoreBrightness();
}

void FocusMode::toggle() {
  if (_enabled) {
    disable();
  } else {
    enable();
  }
}

void FocusMode::createUI() {
  if (_screen)
    return;

  _screen = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(_screen, lv_color_hex(0x000000), 0);
  lv_obj_clear_flag(_screen, LV_OBJ_FLAG_SCROLLABLE);

  // Relógio grande e centralizado
  if (_config.showClock) {
    _clockLabel = lv_label_create(_screen);
    lv_obj_set_style_text_font(_clockLabel, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(_clockLabel, lv_color_hex(0xFFFFFF), 0);
    lv_label_set_text(_clockLabel, "00:00");
    lv_obj_align(_clockLabel, LV_ALIGN_CENTER, 0, -60);
  }

  // Mascote pequeno
  if (_config.showMascot) {
    _mascotLabel = lv_label_create(_screen);
    lv_obj_set_style_text_font(_mascotLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(_mascotLabel, THEME_PRIMARY, 0);
    lv_label_set_text(_mascotLabel, "(•‿•)");
    lv_obj_align(_mascotLabel, LV_ALIGN_CENTER, 0, 10);
  }

  // Bateria
  if (_config.showBattery) {
    _batteryLabel = lv_label_create(_screen);
    lv_obj_set_style_text_font(_batteryLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(_batteryLabel, lv_color_hex(0x888888), 0);
    lv_label_set_text(_batteryLabel, "🔋 100%");
    lv_obj_align(_batteryLabel, LV_ALIGN_TOP_RIGHT, -10, 10);
  }

  // Status de ataque
  if (_config.showAttackStatus) {
    _attackIndicator = lv_obj_create(_screen);
    lv_obj_set_size(_attackIndicator, 10, 10);
    lv_obj_set_style_radius(_attackIndicator, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_attackIndicator, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_border_width(_attackIndicator, 0, 0);
    lv_obj_align(_attackIndicator, LV_ALIGN_BOTTOM_LEFT, 15, -15);
    lv_obj_add_flag(_attackIndicator, LV_OBJ_FLAG_HIDDEN);

    _attackLabel = lv_label_create(_screen);
    lv_obj_set_style_text_font(_attackLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(_attackLabel, lv_color_hex(0x666666), 0);
    lv_label_set_text(_attackLabel, "Idle");
    lv_obj_align(_attackLabel, LV_ALIGN_BOTTOM_LEFT, 30, -12);
  }

  lv_scr_load(_screen);
  updateClock();
  updateBattery();
}

void FocusMode::update() {
  if (!_enabled)
    return;

  uint32_t now = millis();
  if (now - _lastUpdate < 1000)
    return;
  _lastUpdate = now;

  updateClock();

  // Atualiza bateria a cada 30s
  static uint8_t batteryCounter = 0;
  if (++batteryCounter >= 30) {
    batteryCounter = 0;
    updateBattery();
  }
}

void FocusMode::updateClock() {
  if (!_clockLabel)
    return;

  time_t now = time(nullptr);
  struct tm *t = localtime(&now);
  if (t) {
    char buf[6];
    snprintf(buf, sizeof(buf), "%02d:%02d", t->tm_hour, t->tm_min);
    lv_label_set_text(_clockLabel, buf);
  }
}

void FocusMode::updateBattery() {
  if (!_batteryLabel)
    return;

  int pct = sys_hw.getBatteryPercent();
  char buf[16];
  snprintf(buf, sizeof(buf), "🔋 %d%%", pct);
  lv_label_set_text(_batteryLabel, buf);

  // Cor baseada no nível
  uint32_t color = (pct > 50) ? 0x00FF00 : (pct > 20) ? 0xFFAA00 : 0xFF0000;
  lv_obj_set_style_text_color(_batteryLabel, lv_color_hex(color), 0);
}

void FocusMode::setAttackStatus(const char *status, bool active) {
  if (_attackLabel) {
    lv_label_set_text(_attackLabel, status);
    lv_obj_set_style_text_color(
        _attackLabel, active ? lv_color_hex(0xFF4444) : lv_color_hex(0x666666),
        0);
  }
  if (_attackIndicator) {
    if (active) {
      lv_obj_clear_flag(_attackIndicator, LV_OBJ_FLAG_HIDDEN);
      lv_obj_set_style_bg_color(_attackIndicator, lv_color_hex(0xFF0000), 0);
    } else {
      lv_obj_add_flag(_attackIndicator, LV_OBJ_FLAG_HIDDEN);
    }
  }
}

void FocusMode::setConfig(const FocusModeConfig &config) { _config = config; }

void FocusMode::applyDimming() {
  if (_config.autoDimScreen) {
    sys_hw.setDisplayBrightness(_config.dimBrightness * 255 / 100);
  }
}

void FocusMode::restoreBrightness() {
  sys_hw.setDisplayBrightness(_originalBrightness * 255 / 100);
}

// ═══════════════════════════════════════════════════════════════════════════
// STEALTH MODE (Stealth Extremo)
// ═══════════════════════════════════════════════════════════════════════════

StealthMode::StealthMode()
    : _screen(nullptr), _messageLabel(nullptr), _activityDot(nullptr),
      _enabled(false), _lastPulse(0) {}

void StealthMode::begin() {}

void StealthMode::enable() {
  if (_enabled)
    return;
  _enabled = true;

  _screen = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(_screen, lv_color_hex(0x000000), 0);
  lv_obj_clear_flag(_screen, LV_OBJ_FLAG_SCROLLABLE);

  // Apenas um ponto vermelho e texto mínimo
  _activityDot = lv_obj_create(_screen);
  lv_obj_set_size(_activityDot, 8, 8);
  lv_obj_set_style_radius(_activityDot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(_activityDot, lv_color_hex(0x660000), 0);
  lv_obj_set_style_border_width(_activityDot, 0, 0);
  lv_obj_align(_activityDot, LV_ALIGN_CENTER, 0, -10);

  _messageLabel = lv_label_create(_screen);
  lv_obj_set_style_text_font(_messageLabel, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(_messageLabel, lv_color_hex(0x440000), 0);
  lv_label_set_text(_messageLabel, "...");
  lv_obj_align(_messageLabel, LV_ALIGN_CENTER, 0, 15);

  lv_scr_load(_screen);

  // Brilho mínimo
  sys_hw.setDisplayBrightness(10);
}

void StealthMode::disable() {
  if (!_enabled)
    return;
  _enabled = false;
  sys_hw.setDisplayBrightness(100);
}

void StealthMode::setMessage(const char *msg) {
  if (_messageLabel) {
    lv_label_set_text(_messageLabel, msg);
  }
}

void StealthMode::showActivity(bool active) {
  if (_activityDot) {
    lv_obj_set_style_bg_color(
        _activityDot, active ? lv_color_hex(0xFF0000) : lv_color_hex(0x330000),
        0);
  }
}

void StealthMode::update() {
  if (!_enabled)
    return;

  // Pulso do ponto
  uint32_t now = millis();
  if (now - _lastPulse > 1000) {
    _lastPulse = now;
    static bool on = false;
    on = !on;
    showActivity(on);
  }
}
