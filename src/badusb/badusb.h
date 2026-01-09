#pragma once

#include "USB.h"
#include "USBHIDKeyboard.h"
#include <Arduino.h>


class BadUSB {
public:
  BadUSB();
  void begin();
  void end();

  // Core functions
  void press(uint8_t key);
  void releaseAll();
  void type(String text);
  void write(uint8_t key);

  // Scripting helper
  void runScript(String scriptContent); // Parses simplistic DuckyScript

  // Pre-made Payloads
  void payloadWindowsCMD();
  void payloadNotepadPrank();

private:
  USBHIDKeyboard Keyboard;
  bool active;
};

extern BadUSB bad_usb;
