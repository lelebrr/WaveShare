/**
 * @file easter_eggs.cpp
 * @brief Implementação dos Easter Eggs
 */

#include "easter_eggs.h"
#include "mascot_faces.h"
#include "ui_themes.h"

EasterEggs easterEggs;

// Konami Code: UP UP DOWN DOWN LEFT RIGHT LEFT RIGHT B A
const TouchDirection EasterEggs::KONAMI_CODE[KONAMI_CODE_LENGTH] = {
    TOUCH_UP,    TOUCH_UP,   TOUCH_DOWN,  TOUCH_DOWN,  TOUCH_LEFT,
    TOUCH_RIGHT, TOUCH_LEFT, TOUCH_RIGHT, TOUCH_TAP_B, TOUCH_TAP_A};

EasterEggs::EasterEggs()
    : _inputIndex(0), _konamiActive(false), _lastInputTime(0),
      _onEasterEgg(nullptr) {
  for (int i = 0; i < KONAMI_CODE_LENGTH; i++) {
    _inputBuffer[i] = TOUCH_UP;
  }
}

void EasterEggs::begin() {
  _inputIndex = 0;
  _konamiActive = false;
}

bool EasterEggs::processInput(TouchDirection direction) {
  uint32_t now = millis();

  // Reset se passou muito tempo desde o último input
  if (now - _lastInputTime > 3000) {
    _inputIndex = 0;
  }
  _lastInputTime = now;

  // Adiciona ao buffer
  _inputBuffer[_inputIndex] = direction;
  _inputIndex++;

  // Verifica Konami Code
  if (_inputIndex >= KONAMI_CODE_LENGTH) {
    if (checkKonamiCode()) {
      activateKonami();
      _inputIndex = 0;
      return true;
    }
    // Shift buffer left
    for (int i = 0; i < KONAMI_CODE_LENGTH - 1; i++) {
      _inputBuffer[i] = _inputBuffer[i + 1];
    }
    _inputIndex = KONAMI_CODE_LENGTH - 1;
  }

  return false;
}

void EasterEggs::processSwipe(int16_t startX, int16_t startY, int16_t endX,
                              int16_t endY) {
  int16_t dx = endX - startX;
  int16_t dy = endY - startY;

  // Determina direção do swipe
  TouchDirection dir;
  if (abs(dx) > abs(dy)) {
    // Horizontal
    dir = (dx > 0) ? TOUCH_RIGHT : TOUCH_LEFT;
  } else {
    // Vertical
    dir = (dy > 0) ? TOUCH_DOWN : TOUCH_UP;
  }

  processInput(dir);
}

void EasterEggs::processTap(int16_t x, int16_t y, int16_t screenWidth) {
  // Tap esquerdo = B, tap direito = A
  TouchDirection dir = (x < screenWidth / 2) ? TOUCH_TAP_B : TOUCH_TAP_A;
  processInput(dir);
}

void EasterEggs::reset() {
  _inputIndex = 0;
  _konamiActive = false;
}

void EasterEggs::onEasterEgg(void (*callback)(int eggId)) {
  _onEasterEgg = callback;
}

bool EasterEggs::checkKonamiCode() {
  for (int i = 0; i < KONAMI_CODE_LENGTH; i++) {
    if (_inputBuffer[i] != KONAMI_CODE[i]) {
      return false;
    }
  }
  return true;
}

void EasterEggs::activateKonami() {
  _konamiActive = true;
  Serial.println("[EASTER] 🎮 KONAMI CODE ACTIVATED! 🐉");

  showDragonDance();

  if (_onEasterEgg) {
    _onEasterEgg(1); // Easter Egg ID 1 = Konami
  }
}

void EasterEggs::showDragonDance() {
  // Sequência de dança do dragão
  static const MascotFaceType danceSequence[] = {
      FACE_EXCITED, FACE_LOOK_L, FACE_LOOK_R,  FACE_LOOK_L, FACE_LOOK_R,
      FACE_HAPPY,   FACE_WINK,   FACE_EXCITED, FACE_KING,   FACE_VICTORY};

  // Anima a sequência
  static int danceStep = 0;
  static uint32_t lastDance = 0;

  // Cria timer para animação
  lv_timer_t *timer = lv_timer_create(
      [](lv_timer_t *t) {
        static int step = 0;
        if (step < 10) {
          MascotFaceType faces[] = {FACE_EXCITED, FACE_LOOK_L,  FACE_LOOK_R,
                                    FACE_LOOK_L,  FACE_LOOK_R,  FACE_HAPPY,
                                    FACE_WINK,    FACE_EXCITED, FACE_KING,
                                    FACE_VICTORY};
          mascotFaces.setFace(faces[step]);
          step++;
        } else {
          lv_timer_del(t);
          step = 0;
          mascotFaces.setFace(FACE_HAPPY);
        }
      },
      300, nullptr);
}
