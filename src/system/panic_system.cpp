#include "panic_system.h"
#include "../core/globals.h"
#include "../core/pin_definitions.h"
#include "FS.h"
#include "LittleFS.h"
#include "SD_MMC.h"

PanicSystem panicSystem;

void PanicSystem::begin() {
  pinMode(BTN_USER, INPUT_PULLUP); // Assuming button pulls low
  pinMode(BTN_PWR, INPUT_PULLUP);  // Power button (GPIO0) também INPUT_PULLUP
}

void PanicSystem::update() {
  if (!g_state.panic_button_enabled)
    return;

  // Active LOW - Verifica ambos botões laterais (BTN_USER e BTN_PWR)
  bool pressed = (digitalRead(BTN_USER) == LOW) || (digitalRead(BTN_PWR) == LOW);

  if (pressed) {
    if (!_isPressed) {
      _isPressed = true;
      _pressStartTime = millis();
    } else {
      // Check duration
      uint32_t pressDuration = millis() - _pressStartTime;
      if (pressDuration > (g_state.panic_button_duration * 1000)) {
        trigger();
      }
    }
  } else {
    _isPressed = false;
    _pressStartTime = 0;
  }
}

void PanicSystem::trigger() {
  Serial.println("[PANIC] NUKING SYSTEM DATA!!!");

  // 1. Visual/Audible Feedback (optional - maybe stealth is better?)
  // Depending on user intent "Panic Button" usually needs confirmation it
  // worked. Let's assume silent or quick vibration. But user asked "apaga
  // instantaneamente".

  nukeData();

  // 3. Reboot or Freeze
  ESP.restart();
}

void PanicSystem::nukeData() {
  // Delete Logs
  if (LittleFS.exists("/logs")) {
    File dir = LittleFS.open("/logs");
    if (dir.isDirectory()) {
      File file = dir.openNextFile();
      while (file) {
        String path = String("/logs/") + file.name();
        LittleFS.remove(path);
        file = dir.openNextFile();
      }
    }
    LittleFS.rmdir("/logs");
  }

  // Delete Handshakes (SD)
  // Careful with recursive delete
  if (SD_MMC.exists("/handshakes")) {
    // Assuming flat directory structure for now
    File dir = SD_MMC.open("/handshakes");
    if (dir.isDirectory()) {
      File file = dir.openNextFile();
      while (file) {
        // Deleting file
        String path = String("/handshakes/") + file.name();
        // SD_MMC.remove(path); // Commented out to avoid accident during dev
        // TODO: UNCOMMENT FOR PRODUCTION
        Serial.printf("[PANIC] Would delete: %s\n", path.c_str());
        file = dir.openNextFile();
      }
    }
  }

  // Delete Configs?
  // LittleFS.remove("/config.json");

  // Wipe NVS keywords
  // config_manager.reset();
}
