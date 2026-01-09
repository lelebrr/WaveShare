#include "dragon_mascot.h"
#include "../core/config.h"

DragonMascot::DragonMascot(Arduino_GFX *display)
    : _gfx(display), _currentState(DRAGON_IDLE), _frameIndex(0) {}

void DragonMascot::setState(DragonState state) {
  if (_currentState != state) {
    _currentState = state;
    _frameIndex = 0;
  }
}

void DragonMascot::update() {
  unsigned long now = millis();
  if (now - _lastFrameTime > 200) { // 5 FPS animation
    _lastFrameTime = now;
    _frameIndex = (_frameIndex + 1) % 4; // 4 frame loop
  }
}

// Draw a procedural pixel-art style dragon
void DragonMascot::draw(int x, int y) {
  uint16_t mainColor = GREEN;
  bool eyesOpen = true;
  int wingState = _frameIndex % 2; // Flapping wings

  switch (_currentState) {
  case DRAGON_IDLE:
    mainColor = GREEN;
    break;
  case DRAGON_SLEEPING:
    mainColor = 0x0400; // Dark Green
    eyesOpen = false;
    wingState = 0;
    break;
  case DRAGON_HAPPY:
    mainColor = YELLOW;
    wingState = 1; // Wings up
    break;
  case DRAGON_ANGRY:
    mainColor = RED;
    break;
  case DRAGON_SCANNING:
    mainColor = CYAN;
    break;
  }

  // Draw parts
  drawBody(x, y + 20, mainColor);
  drawWings(x + 10, y + 15, WHITE, wingState);
  drawHead(x + 10, y, mainColor, eyesOpen);

  if (_currentState == DRAGON_ANGRY) {
    drawFire(x + 35, y + 10);
  }

  if (_currentState == DRAGON_SLEEPING) {
    _gfx->setCursor(x + 30, y - 10);
    _gfx->setTextColor(WHITE);
    _gfx->setTextSize(2);
    _gfx->print("Zzz");
  }
}

void DragonMascot::drawHead(int x, int y, uint16_t color, bool eyesOpen) {
  // Head shape
  _gfx->fillRoundRect(x, y, 30, 25, 5, color);

  // Snout
  _gfx->fillRoundRect(x + 25, y + 10, 15, 15, 4, color);

  // Eyes
  if (eyesOpen) {
    _gfx->fillCircle(x + 10, y + 10, 4, WHITE); // Left Eye
    _gfx->fillCircle(x + 10, y + 10, 1, BLACK); // Pupil

    _gfx->fillCircle(x + 20, y + 10, 4, WHITE); // Right Eye
    _gfx->fillCircle(x + 20, y + 10, 1, BLACK); // Pupil
  } else {
    // Closed eyes (lines)
    _gfx->drawLine(x + 6, y + 10, x + 14, y + 10, BLACK);
    _gfx->drawLine(x + 16, y + 10, x + 24, y + 10, BLACK);
  }

  // Horns
  _gfx->fillTriangle(x + 2, y, x + 8, y, x + 5, y - 10, WHITE);
  _gfx->fillTriangle(x + 22, y, x + 28, y, x + 25, y - 10, WHITE);
}

void DragonMascot::drawBody(int x, int y, uint16_t color) {
  _gfx->fillEllipse(x + 15, y + 20, 20, 25, color);
  // Belly
  _gfx->fillEllipse(x + 15, y + 20, 12, 18, 0xFFE0); // Light yellow
}

void DragonMascot::drawWings(int x, int y, uint16_t color, int state) {
  if (state == 0) {
    // Wings Down
    _gfx->fillTriangle(x - 15, y + 10, x, y + 5, x - 5, y + 25, color); // Left
    _gfx->fillTriangle(x + 30, y + 5, x + 45, y + 10, x + 35, y + 25,
                       color); // Right
  } else {
    // Wings Up
    _gfx->fillTriangle(x - 15, y - 10, x, y + 5, x - 5, y - 25, color); // Left
    _gfx->fillTriangle(x + 30, y + 5, x + 45, y - 10, x + 35, y - 25,
                       color); // Right
  }
}

void DragonMascot::drawFire(int x, int y) {
  uint16_t fireColor = (millis() / 100) % 2 == 0 ? RED : ORANGE;
  _gfx->fillTriangle(x, y + 5, x + 20, y, x + 5, y + 15, fireColor);
}
