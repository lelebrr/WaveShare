#pragma once

#include "Arduino_GFX_Library.h"
#include <Arduino.h>


enum DragonState {
  DRAGON_IDLE,
  DRAGON_SLEEPING,
  DRAGON_HAPPY,
  DRAGON_ANGRY,
  DRAGON_EXCITED,
  DRAGON_SCANNING
};

class DragonMascot {
public:
  DragonMascot(Arduino_GFX *display);

  void setState(DragonState state);
  void update(); // Called in loop to animate
  void draw(int x, int y);

private:
  Arduino_GFX *_gfx;
  DragonState _currentState;
  unsigned long _lastFrameTime;
  int _frameIndex;

  // Internal drawing helpers for "Pixel Art" via code
  void drawHead(int x, int y, uint16_t color, bool eyesOpen);
  void drawBody(int x, int y, uint16_t color);
  void drawWings(int x, int y, uint16_t color, int wingState);
  void drawFire(int x, int y); // When Angry
};
