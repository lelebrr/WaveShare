#pragma once
#include <Arduino.h>

/**
 * @brief Evil Twin Detector (Tip 16)
 * Analisa assinaturas de portais cativos e discrepâncias de BSSID/Canal.
 */
class EvilTwinDetector {
public:
  // Retorna probabilidade de ser Evil Twin (0.0 - 1.0)
  static float analyze(const char *ssid, const uint8_t *bssid, int channel,
                       int rssi, String htmlContent);

private:
  static bool hasSuspiciousHTML(String html);
};
