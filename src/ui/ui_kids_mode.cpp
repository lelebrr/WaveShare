/**
 * @file ui_kids_mode.cpp
 * @brief Implementação do Modo Kids (Tamagotchi)
 */

#include "ui_kids_mode.h"
#include "mascot_faces.h"
#include "ui_themes.h"

KidsMode kidsMode;

KidsMode::KidsMode()
    : _screen(nullptr), _dragonLabel(nullptr), _moodBar(nullptr),
      _hungerBar(nullptr), _feedBtn(nullptr), _playBtn(nullptr),
      _sleepBtn(nullptr), _statusLabel(nullptr), _dragonMood(80),
      _dragonHunger(50), _dragonEnergy(100), _lastUpdateTime(0) {

  _config.enabled = false;
  _config.hideAttacks = true;
  _config.hideNetworks = true;
  _config.simpleUI = true;
  _config.bigButtons = true;
  _config.colorfulTheme = true;
  memset(_config.parentPin, 0, sizeof(_config.parentPin));
}

void KidsMode::begin() { _lastUpdateTime = millis(); }

void KidsMode::enable() {
  _config.enabled = true;
  _dragonMood = 80;
  _dragonHunger = 50;
  _dragonEnergy = 100;
  createUI();
}

bool KidsMode::disable(const char *pin) {
  // Verifica PIN
  bool correct = true;
  for (int i = 0; i < 4; i++) {
    if (pin[i] != _config.parentPin[i]) {
      correct = false;
      break;
    }
  }

  if (correct || _config.parentPin[0] == 0) {
    _config.enabled = false;
    return true;
  }
  return false;
}

void KidsMode::createUI() {
  if (_screen)
    return;

  _screen = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(_screen, lv_color_hex(0x4488FF),
                            0); // Azul amigável
  lv_obj_clear_flag(_screen, LV_OBJ_FLAG_SCROLLABLE);

  // Título
  lv_obj_t *title = lv_label_create(_screen);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(title, "🐉 Meu Dragão!");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

  // Dragão grande
  _dragonLabel = lv_label_create(_screen);
  lv_obj_set_style_text_font(_dragonLabel, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(_dragonLabel, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(_dragonLabel, "(•‿•)");
  lv_obj_align(_dragonLabel, LV_ALIGN_CENTER, 0, -30);

  // Barra de humor
  lv_obj_t *moodLabel = lv_label_create(_screen);
  lv_label_set_text(moodLabel, "😊 Humor");
  lv_obj_set_style_text_color(moodLabel, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(moodLabel, LV_ALIGN_CENTER, -60, 40);

  _moodBar = lv_bar_create(_screen);
  lv_obj_set_size(_moodBar, 120, 15);
  lv_obj_align(_moodBar, LV_ALIGN_CENTER, 30, 40);
  lv_bar_set_value(_moodBar, _dragonMood, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(_moodBar, lv_color_hex(0x333333), LV_PART_MAIN);
  lv_obj_set_style_bg_color(_moodBar, lv_color_hex(0x00FF88),
                            LV_PART_INDICATOR);

  // Barra de fome
  lv_obj_t *hungerLabel = lv_label_create(_screen);
  lv_label_set_text(hungerLabel, "🍖 Fome");
  lv_obj_set_style_text_color(hungerLabel, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(hungerLabel, LV_ALIGN_CENTER, -60, 70);

  _hungerBar = lv_bar_create(_screen);
  lv_obj_set_size(_hungerBar, 120, 15);
  lv_obj_align(_hungerBar, LV_ALIGN_CENTER, 30, 70);
  lv_bar_set_value(_hungerBar, _dragonHunger, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(_hungerBar, lv_color_hex(0x333333), LV_PART_MAIN);
  lv_obj_set_style_bg_color(_hungerBar, lv_color_hex(0xFFAA00),
                            LV_PART_INDICATOR);

  // Botões grandes
  int btnWidth = 100;
  int btnHeight = 50;
  int btnY = 130;

  // Alimentar
  _feedBtn = lv_btn_create(_screen);
  lv_obj_set_size(_feedBtn, btnWidth, btnHeight);
  lv_obj_align(_feedBtn, LV_ALIGN_CENTER, -110, btnY);
  lv_obj_set_style_bg_color(_feedBtn, lv_color_hex(0xFF6B35), 0);
  lv_obj_set_style_radius(_feedBtn, 15, 0);
  lv_obj_t *feedLabel = lv_label_create(_feedBtn);
  lv_label_set_text(feedLabel, "🍖");
  lv_obj_set_style_text_font(feedLabel, &lv_font_montserrat_24, 0);
  lv_obj_center(feedLabel);
  lv_obj_add_event_cb(
      _feedBtn, [](lv_event_t *e) { kidsMode.feedDragon(); }, LV_EVENT_CLICKED,
      nullptr);

  // Brincar
  _playBtn = lv_btn_create(_screen);
  lv_obj_set_size(_playBtn, btnWidth, btnHeight);
  lv_obj_align(_playBtn, LV_ALIGN_CENTER, 0, btnY);
  lv_obj_set_style_bg_color(_playBtn, lv_color_hex(0x4CAF50), 0);
  lv_obj_set_style_radius(_playBtn, 15, 0);
  lv_obj_t *playLabel = lv_label_create(_playBtn);
  lv_label_set_text(playLabel, "⚽");
  lv_obj_set_style_text_font(playLabel, &lv_font_montserrat_24, 0);
  lv_obj_center(playLabel);
  lv_obj_add_event_cb(
      _playBtn, [](lv_event_t *e) { kidsMode.playWithDragon(); },
      LV_EVENT_CLICKED, nullptr);

  // Dormir
  _sleepBtn = lv_btn_create(_screen);
  lv_obj_set_size(_sleepBtn, btnWidth, btnHeight);
  lv_obj_align(_sleepBtn, LV_ALIGN_CENTER, 110, btnY);
  lv_obj_set_style_bg_color(_sleepBtn, lv_color_hex(0x9C27B0), 0);
  lv_obj_set_style_radius(_sleepBtn, 15, 0);
  lv_obj_t *sleepLabel = lv_label_create(_sleepBtn);
  lv_label_set_text(sleepLabel, "💤");
  lv_obj_set_style_text_font(sleepLabel, &lv_font_montserrat_24, 0);
  lv_obj_center(sleepLabel);
  lv_obj_add_event_cb(
      _sleepBtn, [](lv_event_t *e) { kidsMode.sleepDragon(); },
      LV_EVENT_CLICKED, nullptr);

  // Status
  _statusLabel = lv_label_create(_screen);
  lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(_statusLabel, lv_color_hex(0xFFFFFF), 0);
  lv_label_set_text(_statusLabel, "Seu dragão está feliz!");
  lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -20);
}

void KidsMode::update() {
  if (!_config.enabled)
    return;

  uint32_t now = millis();
  if (now - _lastUpdateTime >= 30000) { // A cada 30 segundos
    _lastUpdateTime = now;
    updateStats();
  }

  updateDragonFace();
}

void KidsMode::updateStats() {
  // Fome aumenta com o tempo
  if (_dragonHunger < 100) {
    _dragonHunger += 2;
  }

  // Humor diminui se com fome
  if (_dragonHunger > 70 && _dragonMood > 0) {
    _dragonMood -= 5;
  }

  // Atualiza barras
  if (_moodBar)
    lv_bar_set_value(_moodBar, _dragonMood, LV_ANIM_ON);
  if (_hungerBar)
    lv_bar_set_value(_hungerBar, _dragonHunger, LV_ANIM_ON);

  // Atualiza status
  if (_statusLabel) {
    if (_dragonMood < 30) {
      lv_label_set_text(_statusLabel, "Seu dragão está triste... 😢");
    } else if (_dragonHunger > 80) {
      lv_label_set_text(_statusLabel, "Seu dragão está com fome! 🍖");
    } else if (_dragonMood > 80) {
      lv_label_set_text(_statusLabel, "Seu dragão está muito feliz! 🎉");
    } else {
      lv_label_set_text(_statusLabel, "Seu dragão está bem! 😊");
    }
  }
}

void KidsMode::updateDragonFace() {
  if (!_dragonLabel)
    return;

  if (_dragonMood < 30) {
    lv_label_set_text(_dragonLabel, FACE_ASCII[FACE_SAD]);
  } else if (_dragonHunger > 80) {
    lv_label_set_text(_dragonLabel, FACE_ASCII[FACE_BORED]);
  } else if (_dragonMood > 80) {
    lv_label_set_text(_dragonLabel, FACE_ASCII[FACE_EXCITED]);
  } else {
    lv_label_set_text(_dragonLabel, FACE_ASCII[FACE_HAPPY]);
  }
}

void KidsMode::feedDragon() {
  if (_dragonHunger > 20) {
    _dragonHunger -= 30;
    if (_dragonHunger < 0)
      _dragonHunger = 0;
  }
  _dragonMood += 10;
  if (_dragonMood > 100)
    _dragonMood = 100;

  showHeart();
  updateStats();
}

void KidsMode::playWithDragon() {
  _dragonMood += 20;
  if (_dragonMood > 100)
    _dragonMood = 100;

  _dragonHunger += 5; // Brincar dá fome

  showStar();
  updateStats();
}

void KidsMode::sleepDragon() {
  if (_dragonLabel) {
    lv_label_set_text(_dragonLabel, FACE_ASCII[FACE_SLEEP]);
  }
  if (_statusLabel) {
    lv_label_set_text(_statusLabel, "Zzz... Seu dragão está dormindo 💤");
  }
}

void KidsMode::showHeart() {
  // Animação de coração
  lv_obj_t *heart = lv_label_create(_screen);
  lv_label_set_text(heart, "❤️");
  lv_obj_set_style_text_font(heart, &lv_font_montserrat_24, 0);
  lv_obj_align(heart, LV_ALIGN_CENTER, 0, -80);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, heart);
  lv_anim_set_values(&a, 0, -50);
  lv_anim_set_time(&a, 1000);
  lv_anim_set_exec_cb(&a, [](void *var, int32_t val) {
    lv_obj_set_y((lv_obj_t *)var, -80 + val);
    lv_obj_set_style_opa((lv_obj_t *)var, 255 - (val * 5), 0);
  });
  lv_anim_set_ready_cb(
      &a, [](lv_anim_t *anim) { lv_obj_del((lv_obj_t *)anim->var); });
  lv_anim_start(&a);
}

void KidsMode::showStar() {
  // Animação de estrela
  lv_obj_t *star = lv_label_create(_screen);
  lv_label_set_text(star, "⭐");
  lv_obj_set_style_text_font(star, &lv_font_montserrat_24, 0);
  lv_obj_align(star, LV_ALIGN_CENTER, 30, -80);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, star);
  lv_anim_set_values(&a, 0, -50);
  lv_anim_set_time(&a, 1000);
  lv_anim_set_exec_cb(&a, [](void *var, int32_t val) {
    lv_obj_set_y((lv_obj_t *)var, -80 + val);
    lv_obj_set_style_opa((lv_obj_t *)var, 255 - (val * 5), 0);
  });
  lv_anim_set_ready_cb(
      &a, [](lv_anim_t *anim) { lv_obj_del((lv_obj_t *)anim->var); });
  lv_anim_start(&a);
}

void KidsMode::setConfig(const KidsModeConfig &config) { _config = config; }

void KidsMode::setParentPin(const char *pin) {
  for (int i = 0; i < 4 && pin[i]; i++) {
    _config.parentPin[i] = pin[i];
  }
}
