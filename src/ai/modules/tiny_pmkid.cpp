#include "tiny_pmkid.h"

// Lista de padrões de SSIDs vulneráveis conhecidos (simplificado)
static const char *WEAK_PATTERNS[] = {"Vivo-",  "Claro-",    "NET_",  "TP-Link",
                                      "D-Link", "AndroidAP", "iPhone"};

float TinyPMKIDOfDoom::calculateEntropy(const char *ssid) {
  if (!ssid)
    return 0.0f;
  int len = strlen(ssid);
  if (len == 0)
    return 0.0f;

  // Shannon entropy simples
  int hist[256] = {0};
  for (int i = 0; i < len; i++)
    hist[(unsigned char)ssid[i]]++;

  float entropy = 0.0f;
  for (int i = 0; i < 256; i++) {
    if (hist[i] > 0) {
      float p = (float)hist[i] / len;
      entropy -= p * log2(p);
    }
  }
  return entropy;
}

bool TinyPMKIDOfDoom::isDefaultSSID(const char *ssid) {
  for (int i = 0; i < 7; i++) {
    if (strncmp(ssid, WEAK_PATTERNS[i], strlen(WEAK_PATTERNS[i])) == 0)
      return true;
  }
  return false;
}

bool TinyPMKIDOfDoom::isWorthAttacking(const uint8_t *bssid, const char *ssid,
                                       const uint8_t *pmkid) {
  // 1. Se é padrão de fábrica, sempre vale a pena (quase sempre senha default
  // ou fraca)
  if (isDefaultSSID(ssid))
    return true;

  // 2. Calcular entropia
  // Baixa entropia ("Casa", "Sala") -> Provável senha fraca
  // Alta entropia ("Xy9#mP!2") -> Provável senha forte
  float ent = calculateEntropy(ssid);

  // Threshold experimental
  if (ent < 2.5f)
    return true; // SSIDs simples

  return false; // Muito complexo, PMKID crack vai demorar anos
}
