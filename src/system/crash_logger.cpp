#include "crash_logger.h"
#include "esp_rom_sys.h"
#include <LittleFS.h>
#include <SD_MMC.h>


CrashLogger crashLogger;

// Tip 52: Global Crash Handler
// We use RTC_NOINIT to store some info if possible, but esp_reset_reason is
// safer for now.

void CrashLogger::begin() {
  // Check reset reason
  esp_reset_reason_t reason = esp_reset_reason();

  // Only log if it was a crash (Panic, WDT, etc)
  if (reason == ESP_RST_PANIC || reason == ESP_RST_INT_WDT ||
      reason == ESP_RST_TASK_WDT || reason == ESP_RST_WDT ||
      reason == ESP_RST_BROWNOUT) {
    Serial.printf("[CRASH] System recovered from crash! Reason: %d\n", reason);
    dumpCrashLog();
  } else {
    Serial.printf("[SYSTEM] Boot Reason: %s\n",
                  getResetReasonName(reason).c_str());
  }
}

void CrashLogger::dumpCrashLog() {
  String log = "========== CRASH DETECTED ==========\n";
  log += "Timestamp: " + String(millis()) +
         "ms (Post-Boot)\n"; // Need Real Time if NTP synced
  log += "Reset Reason: " + getResetReasonName(esp_reset_reason()) + "\n";
  log += "Core: " + String(xPortGetCoreID()) + "\n";

  // Tip 25: Backtrace needed.
  // Without full IDF Panic Hook, we can't easily get the register dump here
  // *after* reboot unless we read the Core Dump partition. For now we log the
  // event.
  log += "====================================\n";

  saveToSD(log);
}

void CrashLogger::saveToSD(String &log) {
  // Ensure SD is mounted (usually checked in main)
  // If not, try to append to LittleFS internal logs

  // Try SD first (Tip 25 says "saved no SD")
  /* We assume SD_MMC is initialized in main or pwn_controller */
  /* Check if we can write */

  bool saved = false;

  // Create directory if not exists
  if (!SD_MMC.exists("/crash")) {
    SD_MMC.mkdir("/crash");
  }

  File f = SD_MMC.open("/crash/last_crash.txt", FILE_APPEND);
  if (f) {
    f.print(log);
    f.close();
    saved = true;
    Serial.println("[CRASH] Log saved to SD Card.");
  }

  if (!saved) {
    // Fallback to internal LittleFS
    if (!LittleFS.exists("/crash"))
      LittleFS.mkdir("/crash");
    File f2 = LittleFS.open("/crash/last_crash.txt", FILE_APPEND);
    if (f2) {
      f2.print(log);
      f2.close();
      Serial.println("[CRASH] Log saved to LittleFS (SD fail).");
    }
  }
}

void CrashLogger::enableWatchdog(uint32_t timeout_ms) {
  // Tip 24: Independent Task WDT
  // Add current task to WDT
  esp_task_wdt_init(timeout_ms / 1000, true); // Initialize if not already
  esp_task_wdt_add(NULL);                     // Add current task
}

void CrashLogger::feedWatchdog() { esp_task_wdt_reset(); }

String CrashLogger::getResetReasonName(esp_reset_reason_t reason) {
  switch (reason) {
  case ESP_RST_UNKNOWN:
    return "UNKNOWN";
  case ESP_RST_POWERON:
    return "POWER ON";
  case ESP_RST_EXT:
    return "EXTERNAL PIN";
  case ESP_RST_SW:
    return "SOFTWARE RESET";
  case ESP_RST_PANIC:
    return "EXCEPTION/PANIC";
  case ESP_RST_INT_WDT:
    return "INTERRUPT WDT";
  case ESP_RST_TASK_WDT:
    return "TASK WDT";
  case ESP_RST_WDT:
    return "WDT CHIP RESET";
  case ESP_RST_DEEPSLEEP:
    return "DEEP SLEEP WAKEUP";
  case ESP_RST_BROWNOUT:
    return "BROWNOUT";
  case ESP_RST_SDIO:
    return "SDIO RESET";
  default:
    return "OTHER";
  }
}
