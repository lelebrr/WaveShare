#include "evil_twin.h"

bool EvilTwinDetector::hasSuspiciousHTML(String html) {
  // Palavras chaves de portais fakes comuns (ex: Wifiphisher, EvilPortal)
  if (html.indexOf("firmware update") != -1)
    return true;
  if (html.indexOf("insira sua senha") != -1)
    return true;
  if (html.indexOf("password verification") != -1)
    return true;

  // Checar inputs de senha sem HTTPS (simplificado, já que vemos HTML puro)
  if (html.indexOf("type=\"password\"") != -1 &&
      html.indexOf("action=\"http://") != -1)
    return true;

  return false;
}

float EvilTwinDetector::analyze(const char *ssid, const uint8_t *bssid,
                                int channel, int rssi, String htmlContent) {
  float score = 0.0f;

  // 1. Análise de conteúdo (Se tivermos capturado HTML de um portal aberto)
  if (htmlContent.length() > 0) {
    if (hasSuspiciousHTML(htmlContent))
      score += 0.9f;
  }

  // 2. Heurística de Canal/RSSI (Se tivermos histórico)
  // Placeholder: Se RSSI é muito alto para um AP que deveria estar longe (ex:
  // enterprise) Isso exigiria contexto do "AP Real".

  // 3. OUI check (Se OUI do MAC não bater com OUI de APs comuns tipo
  // Cisco/Aruba para SSIDs Enterprise)
  // TODO: Implementar lookup básico de OUI

  return min(score, 1.0f);
}
