#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#include "../include/config.h"
#include <Arduino.h>
#include <stdint.h>


class CrashHandler {
public:
  void begin();
  void logCrash(const char *reason);
  const char *getLastCrashReason();
  bool hasCrashed();

private:
  const char *getResetReasonString(uint8_t reason);
};

extern CrashHandler crashHandler;

#endif
