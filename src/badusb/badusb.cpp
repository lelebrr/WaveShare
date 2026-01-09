#include "badusb.h"

BadUSB bad_usb;

BadUSB::BadUSB() : active(false) {}

void BadUSB::begin() {
  Keyboard.begin();
  USB.begin();
  active = true;
}

void BadUSB::end() {
  Keyboard.end();
  active = false;
}

void BadUSB::type(String text) {
  if (!active)
    return;
  Keyboard.print(text);
}

void BadUSB::press(uint8_t key) {
  if (!active)
    return;
  Keyboard.press(key);
}

void BadUSB::releaseAll() {
  if (!active)
    return;
  Keyboard.releaseAll();
}

void BadUSB::write(uint8_t key) {
  if (!active)
    return;
  Keyboard.write(key);
}

// Simple parser for demonstration
void BadUSB::runScript(String script) {
  // In a real implementation, this would parse line by line
  // handling DELAY, STRING, GUI, ETC.
  // For now, let's just type it out if it's plain text
  type(script);
}

void BadUSB::payloadWindowsCMD() {
  if (!active)
    begin();

  // GUI + R
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  delay(500);

  Keyboard.println("cmd");
  delay(1000);

  Keyboard.println("echo Hello from WavePwn Dragon!");
  Keyboard.println("color a");
  Keyboard.println("dir");
}

void BadUSB::payloadNotepadPrank() {
  if (!active)
    begin();

  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(100);
  Keyboard.releaseAll();
  delay(500);

  Keyboard.println("notepad");
  delay(1000);
  Keyboard.println("You have been compromised by a cute dragon...");
  delay(500);
  Keyboard.println("Just kidding! This is a demo.");
}
