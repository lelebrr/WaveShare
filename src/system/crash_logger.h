#pragma once

#include "esp_system.h"
#include "esp_task_wdt.h"
#include <Arduino.h>


class CrashLogger {
public:
  void begin();
  void dumpCrashLog();

  // Call this from critical tasks to feed the watchdog
  // Tip 24: Independent watchdog per task
  void enableWatchdog(uint32_t timeout_ms);
  void feedWatchdog();

private:
  String getResetReasonName(esp_reset_reason_t reason);
  void saveToSD(String &log);
};

extern CrashLogger crashLogger;
