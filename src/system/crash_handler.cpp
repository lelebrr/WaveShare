#include "crash_handler.h"
#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>
#include <esp_attr.h>
#include <rom/rtc.h>

CrashHandler crashHandler;

RTC_NOINIT_ATTR char last_crash_reason[128];
RTC_NOINIT_ATTR bool was_crash;

void CrashHandler::begin() {
  RESET_REASON reason_cpu0 = rtc_get_reset_reason(0);

  Serial.printf("[SYSTEM] Boot Reason: %s\n",
                getResetReasonString((uint8_t)reason_cpu0));

  // Tip 25: Check if restart was due to crash
  // Use numeric values compatible with ESP32-S3 reset reasons
  if (reason_cpu0 == 12 || // SW_CPU_RESET
      reason_cpu0 == 7 ||  // TG0WDT_SYS_RESET
      reason_cpu0 == 8 ||  // TG1WDT_SYS_RESET
      reason_cpu0 == 15) { // RTCWDT_BROWN_OUT_RESET

    Serial.println("[SYSTEM] ⚠ DETECTADO REINÍCIO ANORMAL/CRASH!");

    if (was_crash) {
      Serial.printf("[SYSTEM] Motivo salvo: %s\n", last_crash_reason);
    }
  } else {
    was_crash = false;
    last_crash_reason[0] = '\0';
  }
}

void CrashHandler::logCrash(const char *reason) {
  strncpy(last_crash_reason, reason, sizeof(last_crash_reason) - 1);
  was_crash = true;
}

const char *CrashHandler::getResetReasonString(uint8_t reason) {
  switch (reason) {
  case 1:
    return "POWERON_RESET";
  case 3:
    return "SW_RESET";
  case 4:
    return "OWDT_RESET";
  case 5:
    return "DEEPSLEEP_RESET";
  case 6:
    return "SDIO_RESET";
  case 7:
    return "TG0WDT_SYS_RESET";
  case 8:
    return "TG1WDT_SYS_RESET";
  case 9:
    return "RTCWDT_SYS_RESET";
  case 10:
    return "INTRUSION_RESET";
  case 11:
    return "TGWDT_CPU_RESET";
  case 12:
    return "SW_CPU_RESET";
  case 13:
    return "RTCWDT_CPU_RESET";
  case 14:
    return "EXT_CPU_RESET";
  case 15:
    return "RTCWDT_BROWN_OUT_RESET";
  case 16:
    return "RTCWDT_RTC_RESET";
  default:
    return "NO_MEAN";
  }
}

// Global helper
void reportCrashToSD() {
  if (!was_crash)
    return;

  // Check FS - Assumes SD_MMC is already initialized by main system
  // We do NOT init SD here to avoid conflicts.
  if (SD_MMC.totalBytes() > 0) {
    if (!SD_MMC.exists("/crash"))
      SD_MMC.mkdir("/crash");

    File f = SD_MMC.open("/crash/last_crash.txt", FILE_APPEND);
    if (f) {
      f.printf("[%lu] Crash detected. Reason: %s\n", millis(),
               last_crash_reason);
      f.close();
      Serial.println("[SYSTEM] Crash log salvo no SD.");

      // Clear flag after saving to avoid spamming next boot if not cleared?
      // Actually better to keep it in case we wan't to show on UI.
      // But for now, we leave it.
      was_crash = false;
    }
  }
}
