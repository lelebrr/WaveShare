#pragma once
#include <Arduino.h>

/**
 * @brief Handshake Strength Predictor (Tip 15)
 * Prevê probabilidades de quebrar o handshake baseada em SNR e frames
 * capturados.
 */
class HandshakePredictor {
public:
  static float predictCrackingProbability(int rssi_client, int rssi_ap,
                                          int eapol_frames_captured);
};
