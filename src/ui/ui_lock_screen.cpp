/**
 * @file ui_lock_screen.cpp
 * @brief Implementação da tela de bloqueio
 */

#include "ui_lock_screen.h"
#include "mascot_faces.h"
#include "ui_themes.h"
#include <time.h>

LockScreen lockScreen;

LockScreen::LockScreen()
    : _screen(nullptr), _clockLabel(nullptr), _dateLabel(nullptr),
      _mascotLabel(nullptr), _statusLabel(nullptr), _unlockHint(nullptr),
      _pinDisplay(nullptr), _locked(true), _visible(false),
      _inputPatternLength(0), _inputPinLength(0), _lastActivityTime(0),
      _onUnlock(nullptr) {

  _config.enabled = false;
  _config.lockType = LOCK_NONE;
  memset(_config.pin, 0, sizeof(_config.pin));
  memset(_config.pattern, 0, sizeof(_config.pattern));
  _config.patternLength = 0;
  _config.showMascot = true;
  _config.showStats = true;
  _config.autoLockSeconds = 60;

  for (int i = 0; i < 9; i++) {
    _patternGrid[i] = nullptr;
  }
  memset(_inputPattern, 0, sizeof(_inputPattern));
  memset(_inputPin, 0, sizeof(_inputPin));
}

void LockScreen::begin() { createUI(); }

void LockScreen::createUI() {
  if (_screen)
    return;

  // Tela principal
  _screen = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(_screen, lv_color_hex(0x000000), 0);
  lv_obj_set_style_bg_opa(_screen, LV_OPA_COVER, 0);
  lv_obj_clear_flag(_screen, LV_OBJ_FLAG_SCROLLABLE);

  // Relógio grande
  _clockLabel = lv_label_create(_screen);
  lv_obj_set_style_text_font(_clockLabel, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(_clockLabel, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(_clockLabel, "00:00");
  lv_obj_align(_clockLabel, LV_ALIGN_TOP_MID, 0, 80);

  // Data
  _dateLabel = lv_label_create(_screen);
  lv_obj_set_style_text_font(_dateLabel, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(_dateLabel, lv_color_hex(0x888888), 0);
  lv_label_set_text(_dateLabel, "Domingo, 8 Dez");
  lv_obj_align(_dateLabel, LV_ALIGN_TOP_MID, 0, 140);

  // Mascote
  _mascotLabel = lv_label_create(_screen);
  lv_obj_set_style_text_font(_mascotLabel, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(_mascotLabel, THEME_PRIMARY, 0);
  lv_label_set_text(_mascotLabel, "(-.-)zzZ");
  lv_obj_align(_mascotLabel, LV_ALIGN_CENTER, 0, 0);

  // Status (handshakes, etc)
  _statusLabel = lv_label_create(_screen);
  lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(_statusLabel, THEME_ACCENT, 0);
  lv_label_set_text(_statusLabel, "🤝 12 handshakes | 📡 48 redes");
  lv_obj_align(_statusLabel, LV_ALIGN_CENTER, 0, 60);

  // Hint de desbloqueio
  _unlockHint = lv_label_create(_screen);
  lv_obj_set_style_text_font(_unlockHint, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(_unlockHint, lv_color_hex(0x666666), 0);
  lv_label_set_text(_unlockHint, "Deslize para desbloquear");
  lv_obj_align(_unlockHint, LV_ALIGN_BOTTOM_MID, 0, -40);

  // Grid de padrão (3x3)
  int gridSize = 180;
  int dotSize = 40;
  int spacing = (gridSize - dotSize * 3) / 2;
  int startX = (368 - gridSize) / 2;
  int startY = 250;

  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      int idx = row * 3 + col;
      _patternGrid[idx] = lv_obj_create(_screen);
      lv_obj_set_size(_patternGrid[idx], dotSize, dotSize);
      lv_obj_set_pos(_patternGrid[idx], startX + col * (dotSize + spacing),
                     startY + row * (dotSize + spacing));
      lv_obj_set_style_radius(_patternGrid[idx], LV_RADIUS_CIRCLE, 0);
      lv_obj_set_style_bg_color(_patternGrid[idx], lv_color_hex(0x333333), 0);
      lv_obj_set_style_border_width(_patternGrid[idx], 2, 0);
      lv_obj_set_style_border_color(_patternGrid[idx], THEME_PRIMARY, 0);
      lv_obj_set_style_border_opa(_patternGrid[idx], LV_OPA_50, 0);
      lv_obj_add_flag(_patternGrid[idx], LV_OBJ_FLAG_HIDDEN);
    }
  }

  // Display de PIN (4 círculos)
  _pinDisplay = lv_obj_create(_screen);
  lv_obj_set_size(_pinDisplay, 200, 30);
  lv_obj_align(_pinDisplay, LV_ALIGN_CENTER, 0, 100);
  lv_obj_set_style_bg_opa(_pinDisplay, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(_pinDisplay, 0, 0);
  lv_obj_add_flag(_pinDisplay, LV_OBJ_FLAG_HIDDEN);
}

void LockScreen::show() {
  if (!_screen)
    createUI();

  _visible = true;
  _locked = true;
  _inputPatternLength = 0;
  _inputPinLength = 0;

  // Atualiza elementos conforme tipo de bloqueio
  switch (_config.lockType) {
  case LOCK_PATTERN:
    lv_label_set_text(_unlockHint, "Desenhe o padrao");
    for (int i = 0; i < 9; i++) {
      lv_obj_clear_flag(_patternGrid[i], LV_OBJ_FLAG_HIDDEN);
    }
    break;
  case LOCK_PIN:
    lv_label_set_text(_unlockHint, "Digite o PIN");
    lv_obj_clear_flag(_pinDisplay, LV_OBJ_FLAG_HIDDEN);
    break;
  default:
    lv_label_set_text(_unlockHint, "Deslize para desbloquear");
    break;
  }

  // Mostra/esconde mascote
  if (_config.showMascot) {
    lv_obj_clear_flag(_mascotLabel, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(_mascotLabel, FACE_ASCII[FACE_SLEEP]);
  } else {
    lv_obj_add_flag(_mascotLabel, LV_OBJ_FLAG_HIDDEN);
  }

  updateClock();
  lv_scr_load(_screen);
}

void LockScreen::hide() {
  _visible = false;
  _locked = false;
}

bool LockScreen::tryUnlock() {
  bool success = false;

  switch (_config.lockType) {
  case LOCK_NONE:
    success = true;
    break;
  case LOCK_PATTERN:
    success = validatePattern();
    break;
  case LOCK_PIN:
    success = validatePin();
    break;
  }

  if (success) {
    animateUnlock();
    if (_onUnlock)
      _onUnlock();
  } else {
    showError();
  }

  return success;
}

void LockScreen::update() {
  if (!_visible)
    return;

  updateClock();

  // Animação do mascote dormindo
  if (_config.showMascot && _mascotLabel) {
    static uint32_t lastChange = 0;
    if (millis() - lastChange > 3000) {
      lastChange = millis();
      static bool toggle = false;
      lv_label_set_text(_mascotLabel, toggle ? FACE_ASCII[FACE_SLEEP]
                                             : FACE_ASCII[FACE_SLEEP2]);
      toggle = !toggle;
    }
  }
}

void LockScreen::updateClock() {
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);

  if (timeinfo && _clockLabel) {
    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", timeinfo->tm_hour,
             timeinfo->tm_min);
    lv_label_set_text(_clockLabel, timeStr);
  }

  if (timeinfo && _dateLabel) {
    static const char *dias[] = {"Domingo", "Segunda", "Terca", "Quarta",
                                 "Quinta",  "Sexta",   "Sabado"};
    static const char *meses[] = {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun",
                                  "Jul", "Ago", "Set", "Out", "Nov", "Dez"};
    char dateStr[32];
    snprintf(dateStr, sizeof(dateStr), "%s, %d %s", dias[timeinfo->tm_wday],
             timeinfo->tm_mday, meses[timeinfo->tm_mon]);
    lv_label_set_text(_dateLabel, dateStr);
  }
}

void LockScreen::setConfig(const LockScreenConfig &config) { _config = config; }

void LockScreen::setPin(const char *pin) {
  strncpy(_config.pin, pin, 4);
  _config.pin[4] = '\0';
}

void LockScreen::setPattern(const uint8_t *pattern, uint8_t length) {
  _config.patternLength = (length > 9) ? 9 : length;
  memcpy(_config.pattern, pattern, _config.patternLength);
}

void LockScreen::onUnlock(void (*callback)()) { _onUnlock = callback; }

void LockScreen::handlePatternInput(int pointIndex) {
  if (_inputPatternLength >= 9)
    return;

  _inputPattern[_inputPatternLength++] = pointIndex;

  // Highlight do ponto
  if (_patternGrid[pointIndex]) {
    lv_obj_set_style_bg_color(_patternGrid[pointIndex], THEME_PRIMARY, 0);
  }
}

void LockScreen::handlePinInput(char digit) {
  if (_inputPinLength >= 4)
    return;

  _inputPin[_inputPinLength++] = digit;
  _inputPin[_inputPinLength] = '\0';
}

bool LockScreen::validatePattern() {
  if (_inputPatternLength != _config.patternLength)
    return false;

  for (int i = 0; i < _inputPatternLength; i++) {
    if (_inputPattern[i] != _config.pattern[i])
      return false;
  }
  return true;
}

bool LockScreen::validatePin() { return strcmp(_inputPin, _config.pin) == 0; }

void LockScreen::showError() {
  // Vibração de erro
  if (_mascotLabel) {
    lv_label_set_text(_mascotLabel, FACE_ASCII[FACE_SAD]);
  }

  // Animação de shake
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, _unlockHint);
  lv_anim_set_values(&a, -10, 10);
  lv_anim_set_time(&a, 50);
  lv_anim_set_repeat_count(&a, 4);
  lv_anim_set_exec_cb(
      &a, [](void *var, int32_t val) { lv_obj_set_x((lv_obj_t *)var, val); });
  lv_anim_start(&a);

  // Reset input
  _inputPatternLength = 0;
  _inputPinLength = 0;
  memset(_inputPin, 0, sizeof(_inputPin));

  // Reset pattern grid
  for (int i = 0; i < 9; i++) {
    if (_patternGrid[i]) {
      lv_obj_set_style_bg_color(_patternGrid[i], lv_color_hex(0x333333), 0);
    }
  }
}

void LockScreen::animateUnlock() {
  if (_mascotLabel) {
    lv_label_set_text(_mascotLabel, FACE_ASCII[FACE_AWAKE]);
  }

  // Fade out
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, _screen);
  lv_anim_set_values(&a, 255, 0);
  lv_anim_set_time(&a, 300);
  lv_anim_set_exec_cb(&a, [](void *var, int32_t val) {
    lv_obj_set_style_opa((lv_obj_t *)var, (lv_opa_t)val, 0);
  });
  lv_anim_set_ready_cb(&a, [](lv_anim_t *anim) { lockScreen.hide(); });
  lv_anim_start(&a);
}
