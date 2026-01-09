/**
 * @file ui_avatar.cpp
 * @brief Implementação do avatar visual do assistente
 */

#include "ui_avatar.h"
#include "../hardware/audio_driver.h"    // For sound
#include "../hardware/system_hardware.h" // For haptics
#include "../wifi/wifi_attacks.h"
#include "ui_debug_screen.h"

VoiceAvatar voiceAvatar;

// Cores do avatar
#define AVATAR_BG_COLOR lv_color_hex(0x1a1a3e)
#define AVATAR_GLOW_CYAN lv_color_hex(0x00ffff)
#define AVATAR_GLOW_GREEN lv_color_hex(0x00ff88)
#define AVATAR_GLOW_PURPLE lv_color_hex(0x9933ff)
#define AVATAR_TEXT_COLOR lv_color_hex(0xcccccc)

// LVGL Event Callback
static void avatar_touch_cb(lv_event_t *e) {
  VoiceAvatar *avatar = (VoiceAvatar *)lv_event_get_user_data(e);
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    lv_point_t p;
    lv_indev_get_point(lv_indev_get_act(), &p);
    avatar->checkNoseTap(p.x, p.y);
    avatar->reactToTouch();
  } else if (code == LV_EVENT_LONG_PRESSED) { // Tip 79: Quick Nuke
    Serial.println("!!! DRAGON LONG PRESS - QUICK NUKE !!!");

    // Visual Feedback
    avatar->setFace(FACE_ANGRY);
    avatar->setGlowColor(0xFF0000);
    sys_hw.vibratorOn(200);

    // Execute Attack
    wifi_attacks.startOneTapNuke();
  } else if (code == LV_EVENT_PRESSING) {
    lv_indev_t *indev = lv_indev_get_act();
    if (indev) {
      lv_point_t p;
      lv_indev_get_point(indev, &p);
      avatar->setFocus(p.x, p.y);
    }
  } else if (code == LV_EVENT_RELEASED) {
    avatar->setFocus(LCD_WIDTH / 2, 110); // Center back
  }
}

VoiceAvatar::VoiceAvatar()
    : _container(nullptr), _avatarCircle(nullptr), _faceLabel(nullptr),
      _nameLabel(nullptr), _textLabel(nullptr), _subtextLabel(nullptr),
      _auraRing(nullptr), _state(AVATAR_IDLE), _currentFace(FACE_HAPPY),
      _glowColor(0x00ffff), _glowEnabled(true), _frameCount(0), _blinkTimer(0),
      _dotAnimFrame(0), _noseTapCount(0), _lastTapTime(0) {

  for (int i = 0; i < 3; i++) {
    _listeningDots[i] = nullptr;
  }
}

void VoiceAvatar::create(lv_obj_t *parent) {
  // Container principal do avatar
  _container = lv_obj_create(parent);
  lv_obj_set_size(_container, lv_pct(100), 160);
  lv_obj_align(_container, LV_ALIGN_TOP_MID, 0, 30);
  lv_obj_set_style_bg_opa(_container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(_container, 0, 0);
  lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

  // Anel de aura (efeito glow pulsante)
  _auraRing = lv_obj_create(_container);
  lv_obj_set_size(_auraRing, 110, 110);
  lv_obj_align(_auraRing, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_radius(_auraRing, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_opa(_auraRing, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(_auraRing, 3, 0);
  lv_obj_set_style_border_color(_auraRing, AVATAR_GLOW_CYAN, 0);
  lv_obj_set_style_border_opa(_auraRing, LV_OPA_50, 0);
  lv_obj_clear_flag(_auraRing, LV_OBJ_FLAG_CLICKABLE);

  // Círculo de fundo do avatar
  _avatarCircle = lv_obj_create(_container);
  lv_obj_set_size(_avatarCircle, 90, 90);
  lv_obj_align(_avatarCircle, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_radius(_avatarCircle, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(_avatarCircle, AVATAR_BG_COLOR, 0);
  lv_obj_set_style_border_width(_avatarCircle, 2, 0);
  lv_obj_set_style_border_color(_avatarCircle, AVATAR_GLOW_CYAN, 0);
  lv_obj_set_style_shadow_width(_avatarCircle, 20, 0);
  lv_obj_set_style_shadow_color(_avatarCircle, AVATAR_GLOW_CYAN, 0);
  lv_obj_set_style_shadow_opa(_avatarCircle, LV_OPA_50, 0);
  lv_obj_clear_flag(_avatarCircle, LV_OBJ_FLAG_SCROLLABLE);

  // Face (emoji grande)
  _faceLabel = lv_label_create(_avatarCircle);
  lv_label_set_text(_faceLabel, "(•‿•)");
  lv_obj_set_style_text_font(_faceLabel, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(_faceLabel, AVATAR_GLOW_GREEN, 0);
  lv_obj_center(_faceLabel);

  // Nome do assistente
  _nameLabel = lv_label_create(_container);
  lv_label_set_text(_nameLabel, "DRAGON");
  lv_obj_set_style_text_font(_nameLabel, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(_nameLabel, AVATAR_GLOW_CYAN, 0);
  lv_obj_align(_nameLabel, LV_ALIGN_TOP_MID, 0, 105);

  // Texto principal (fala do assistente)
  _textLabel = lv_label_create(_container);
  lv_label_set_text(_textLabel, "Pronto para ajudar!");
  lv_obj_set_style_text_font(_textLabel, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(_textLabel, lv_color_hex(0xffffff), 0);
  lv_obj_align(_textLabel, LV_ALIGN_TOP_MID, 0, 125);

  // Subtexto (status)
  _subtextLabel = lv_label_create(_container);
  lv_label_set_text(_subtextLabel, "Diga 'Hey Dragon'");
  lv_obj_set_style_text_font(_subtextLabel, &lv_font_montserrat_10, 0);
  lv_obj_set_style_text_color(_subtextLabel, AVATAR_TEXT_COLOR, 0);
  lv_obj_align(_subtextLabel, LV_ALIGN_TOP_MID, 0, 145);

  // Pontos de "ouvindo" (animados)
  for (int i = 0; i < 3; i++) {
    _listeningDots[i] = lv_obj_create(_container);
    lv_obj_set_size(_listeningDots[i], 8, 8);
    lv_obj_set_pos(_listeningDots[i], 140 + i * 15, 130);
    lv_obj_set_style_radius(_listeningDots[i], LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(_listeningDots[i], AVATAR_GLOW_CYAN, 0);
    lv_obj_set_style_border_width(_listeningDots[i], 0, 0);
    lv_obj_add_flag(_listeningDots[i], LV_OBJ_FLAG_HIDDEN);
  }
}

#include "../mascot/mascot_manager.h"

void VoiceAvatar::update() {
  if (!_container)
    return;

  _frameCount++;

  // Sincroniza estado com MascotManager (Tip 19)
  if (mascot_manager.isTalking()) {
    _state = AVATAR_SPEAKING;
  } else {
    // Mapeia mood para estado do avatar
    uint8_t mood = mascot_manager.getMood();
    if (mood == 3)
      _state = AVATAR_LISTENING; // Excited -> Listening
    else if (mood == 2)
      _state = AVATAR_THINKING; // Sleepy -> Thinking
    else
      _state = AVATAR_IDLE;
  }

  switch (_state) {
  case AVATAR_IDLE:
    updateIdleAnimation();
    break;
  case AVATAR_LISTENING:
    updateListeningAnimation();
    break;
  case AVATAR_SPEAKING:
    updateSpeakingAnimation();
    break;
  case AVATAR_THINKING:
    updateThinkingAnimation();
    break;
  default:
    break;
  }
}

#include "mascot_faces.h" // Adicionado include

void VoiceAvatar::updateIdleAnimation() {
  // Atualiza face (usando singleton centralizado)
  if (_faceLabel) {
    lv_label_set_text(_faceLabel, mascotFaces.getCurrentFaceASCII());
  }

  // Efeito de respiração (movimento vertical)
  if (_avatarCircle) {
    lv_obj_set_y(_avatarCircle, 10 + mascotFaces.getBreathingOffset());
  }

  // Pulso suave na aura
  float pulse = (sin(_frameCount * 0.05) + 1) / 2;
  lv_obj_set_style_border_opa(_auraRing, 30 + (uint8_t)(pulse * 40), 0);
  lv_obj_set_style_shadow_opa(_avatarCircle, 30 + (uint8_t)(pulse * 30), 0);
}

void VoiceAvatar::updateListeningAnimation() {
  // Aura pulsante mais intensa
  float pulse = (sin(_frameCount * 0.15) + 1) / 2;
  lv_obj_set_style_border_opa(_auraRing, 100 + (uint8_t)(pulse * 155), 0);
  lv_obj_set_style_border_color(_auraRing, AVATAR_GLOW_PURPLE, 0);

  // Anima pontos
  _dotAnimFrame++;
  for (int i = 0; i < 3; i++) {
    lv_obj_clear_flag(_listeningDots[i], LV_OBJ_FLAG_HIDDEN);

    int phase = (_dotAnimFrame / 10 + i) % 3;
    uint8_t opa = (phase == 0) ? 255 : (phase == 1) ? 150 : 80;
    lv_obj_set_style_bg_opa(_listeningDots[i], opa, 0);
  }

  // Face atenta
  lv_label_set_text(_faceLabel, "(⊙_⊙)");
}

void VoiceAvatar::updateSpeakingAnimation() {
  // Alterna boca
  if ((_frameCount / 8) % 2 == 0) {
    lv_label_set_text(_faceLabel, "(•ᴗ•)");
  } else {
    lv_label_set_text(_faceLabel, "(•o•)");
  }

  // Glow verde pulsante
  float pulse = (sin(_frameCount * 0.2) + 1) / 2;
  lv_obj_set_style_shadow_color(_avatarCircle, AVATAR_GLOW_GREEN, 0);
  lv_obj_set_style_shadow_opa(_avatarCircle, 80 + (uint8_t)(pulse * 100), 0);
  lv_obj_set_style_border_color(_auraRing, AVATAR_GLOW_GREEN, 0);
}

void VoiceAvatar::updateThinkingAnimation() {
  // Rotação de pontos
  const char *thinkFaces[] = {"(•_•)", "(•_◦)", "(◦_•)", "(◦_◦)"};
  int idx = (_frameCount / 15) % 4;
  lv_label_set_text(_faceLabel, thinkFaces[idx]);
}

void VoiceAvatar::setState(AvatarState state) {
  _state = state;

  // Esconde pontos de listening se não está ouvindo
  if (state != AVATAR_LISTENING) {
    for (int i = 0; i < 3; i++) {
      lv_obj_add_flag(_listeningDots[i], LV_OBJ_FLAG_HIDDEN);
    }
  }

  // Restaura cores padrão
  lv_obj_set_style_border_color(_auraRing, AVATAR_GLOW_CYAN, 0);
  lv_obj_set_style_shadow_color(_avatarCircle, AVATAR_GLOW_CYAN, 0);
}

void VoiceAvatar::setFace(MascotFaceType face) {
  _currentFace = face;
  if (_faceLabel && _state == AVATAR_IDLE) {
    lv_label_set_text(_faceLabel, FACE_ASCII[face]);
  }
}

void VoiceAvatar::setText(const char *text) {
  if (_textLabel) {
    lv_label_set_text(_textLabel, text);
  }
}

void VoiceAvatar::setSubtext(const char *text) {
  if (_subtextLabel) {
    lv_label_set_text(_subtextLabel, text);
  }
}

void VoiceAvatar::setName(const char *name) {
  if (_nameLabel) {
    lv_label_set_text(_nameLabel, name);
  }
}

void VoiceAvatar::setGlowColor(uint32_t color) {
  _glowColor = color;
  if (_avatarCircle) {
    lv_obj_set_style_border_color(_avatarCircle, lv_color_hex(color), 0);
    lv_obj_set_style_shadow_color(_avatarCircle, lv_color_hex(color), 0);
  }
  if (_auraRing) {
    lv_obj_set_style_border_color(_auraRing, lv_color_hex(color), 0);
  }
}

void VoiceAvatar::setGlowEnabled(bool enable) {
  _glowEnabled = enable;
  if (_avatarCircle) {
    lv_obj_set_style_shadow_opa(_avatarCircle, enable ? LV_OPA_50 : LV_OPA_0,
                                0);
  }
}

void VoiceAvatar::playWaveAnimation() {
  setFace(FACE_HAPPY);
  setText("👋 Olá!");
}

void VoiceAvatar::playSuccessAnimation() {
  setFace(FACE_EXCITED);
  setGlowColor(0x00ff88);
}

void VoiceAvatar::playErrorAnimation() {
  setFace(FACE_SAD);
  setGlowColor(0xff3333);
}

void VoiceAvatar::setFocus(int x, int y) {
  if (!_avatarCircle || !_faceLabel)
    return;

  // Parallax Effect: Move face label slightly towards touch
  // Center of avatar is roughly (LCD_WIDTH/2, 110)
  int cx = LCD_WIDTH / 2;
  int cy = 110; // Approx

  int dx = (x - cx) / 10; // Dampen movement
  int dy = (y - cy) / 10;

  // Clamp
  if (dx > 10)
    dx = 10;
  if (dx < -10)
    dx = -10;
  if (dy > 10)
    dy = 10;
  if (dy < -10)
    dy = -10;

  lv_obj_set_pos(_faceLabel, dx, dy);
}

void VoiceAvatar::reactToTouch() {
  playSuccessAnimation(); // Happy jump

  // Item 47: Haptic Feedback + Instant Color Change
  sys_hw.vibratorOn(50);

  // Cycle colors instantly
  static int colorIdx = 0;
  static const uint32_t colors[] = {0x00FF88, 0x00FFFF, 0xFF00FF, 0xFF0044,
                                    0xFFFF00};
  setGlowColor(colors[colorIdx]);
  colorIdx = (colorIdx + 1) % 5;
}

// Item 50: Easter Egg 5 Taps on Nose
void VoiceAvatar::checkNoseTap(int x, int y) {
  // Nose/Face center approx (LCD_WIDTH/2, 110 screen coordinates)
  // Simple check: just count taps for now, assuming click on container is close
  // enough

  uint32_t now = millis();
  if (now - _lastTapTime > 2000) {
    _noseTapCount = 0;
  }
  _lastTapTime = now;

  _noseTapCount++;
  if (_noseTapCount >= 5) {
    _noseTapCount = 0;
    Serial.println("!!! DRAGON NOSE BOOPED 5 TIMES - DEBUG MODE !!!");
    audioDriver.playSound(SOUND_SUCCESS);
    ui_debug_screen_show(); // Tip 76
  }
}

void VoiceAvatar::show() {
  if (_container) {
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_HIDDEN);

    // Enable Touch Interactivity
    lv_obj_add_flag(_container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(_container, avatar_touch_cb, LV_EVENT_ALL, this);
  }
}

void VoiceAvatar::hide() {
  if (_container) {
    lv_obj_add_flag(_container, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_event_cb(_container, avatar_touch_cb);
  }
}
