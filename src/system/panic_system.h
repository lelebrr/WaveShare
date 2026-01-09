#pragma once
#include <Arduino.h>

class PanicSystem {
public:
  void begin();
  void update();
  void trigger(); // Manually trigger panic

private:
  void nukeData();
  bool _isPressed = false;
  uint32_t _pressStartTime = 0;
};

extern PanicSystem panicSystem;
