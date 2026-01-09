#pragma once
#include <Arduino.h>

/**
 * @file tiny_classifiers.h
 * @brief Implementação dos classificadores "Tiny" (<2KB)
 *
 * Tips 14 (PMKID) e 15 (Handshake Strength)
 */

class TinyClassifiers {
public:
  // Tip 14: Diz se vale a pena atacar um PMKID capturado
  // Retorna true se "Weak/Crackable", false se Hard
  static bool isPmkidWeak(String ssid, String oui, uint8_t *pmkidData) {
    // Heurística baseada em padrões de roteadores vulneráveis
    // Ex: alguns TP-Links velhos geram PMKID previsível ou usam senhas default

    // 1. Check OUI (Mac Address Prefix)
    // D-Link, Huawei, TP-Link antigos muitas vezes vulneráveis
    if (oui.startsWith("00:1D:60") || oui.startsWith("C8:3A:35")) {
      return true;
    }

    // 2. Check SSID entropy
    // SSID padrão "VIVO-1234" é mais provável de ser fraco que
    // "MinhaCasaSegura"
    if (ssid.length() < 5)
      return true; // Curto demais?

    // Se SSID termina com 4 dígitos hexa/numerais, alta chance de default
    // password Ex: Net_2G_8E4A
    int digitCount = 0;
    for (int i = 0; i < ssid.length(); i++) {
      if (isDigit(ssid.charAt(i)))
        digitCount++;
    }

    if (digitCount >= 4 && ssid.indexOf('_') != -1)
      return true;

    return false;
  }

  // Tip 15: Prediz força do handshake capturado (Quality Score 0-100)
  static int predictHandshakeStrength(int halfHandshakes, int anonces, int snr,
                                      int gapMs) {
    int score = 0;

    // Precisa dos 4-way complete (M1+M2 ou M2+M3 pelo menos)
    // Aqui assumimos que "halfHandshakes" conta pares válidos
    if (halfHandshakes >= 1)
      score += 50;
    if (halfHandshakes >= 2)
      score += 30; // 4-way complete

    if (anonces > 0)
      score += 10;

    // SNR influencia qualidade da captura e chance de corrupção
    if (snr > -60)
      score += 10;
    else if (snr > -80)
      score += 5;

    // Gap muito grande sugere retransmissão ou perda
    if (gapMs < 100)
      score += 0; // Ideal
    else if (gapMs > 500)
      score -= 20;

    if (score > 100)
      score = 100;
    if (score < 0)
      score = 0;

    return score;
  }
};
