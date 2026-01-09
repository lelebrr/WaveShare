#pragma once
#include <Arduino.h>

/**
 * @brief Tiny PMKID Classifier (Tip 14)
 * Analisa se um PMKID capturado vale a pena atacar baseado na entropia do SSID
 * e MAC. Modelo ultra-leve (<1KB).
 */
class TinyPMKIDOfDoom {
public:
  static bool isWorthAttacking(const uint8_t *bssid, const char *ssid,
                               const uint8_t *pmkid);

private:
  static float calculateEntropy(const char *ssid);
  static bool isDefaultSSID(const char *ssid);
};
