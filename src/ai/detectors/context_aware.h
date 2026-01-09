#pragma once
#include <Arduino.h>

/**
 * @file context_aware_ai.h
 * @brief Agrupa Tips 20, 21, 23, 24, 25
 */

class ContextAwareAI {
public:
  // Tip 20: Battery Life Predictor
  static int predictBatteryHours(int batteryPercent, bool attackMode) {
    // Consumo estimado: Idle ~90mA, Attack ~220mA
    float mA = attackMode ? 220.0f : 90.0f;
    float capacitymAh = 800.0f; // Exemplo de bateria
    float remaining = capacitymAh * (batteryPercent / 100.0f);
    return (int)(remaining / mA);
  }

  // Tip 21: Crowd Density (Pessoas por Probe Requests únicos / min)
  static String estimateCrowdDensity(int uniqueProbesPerMin) {
    if (uniqueProbesPerMin < 5)
      return "Empty";
    if (uniqueProbesPerMin < 20)
      return "Low";
    if (uniqueProbesPerMin < 50)
      return "Medium";
    if (uniqueProbesPerMin < 100)
      return "High";
    return "CROWDED";
  }

  // Tip 23: Fake AP Realism Score
  static int calculateFakeApScore(String ssid, bool encrypted, int channel) {
    int score = 0;
    // SSID parece real?
    if (ssid.length() > 3 && ssid.indexOf("X") == -1)
      score += 30;
    // Canais padrão (1, 6, 11) parecem mais reais que 13 ou 14
    if (channel == 1 || channel == 6 || channel == 11)
      score += 20;
    // Redes abertas em lugares públicos são comuns, mas encrypted é mais
    // "residencial"
    if (encrypted)
      score += 40;
    else
      score += 10;

    return score;
  }

  // Tip 25: Micro Sleep Detector (Gyro Activity)
  // Se o gyro ficar COMPLETAMENTE parado por X tempo, usuário dormiu?
  // Retorna true se deve desligar tela
  static bool detectMicroSleep(float gyroVar, uint32_t lastTouch) {
    if (millis() - lastTouch > 60000) { // 1 min sem toque
      if (gyroVar < 0.05f) {            // Quase zero movimento
        return true;
      }
    }
    return false;
  }
};
