#include "handshake_predictor.h"
#include <math.h>

float HandshakePredictor::predictCrackingProbability(
    int rssi_client, int rssi_ap, int eapol_frames_captured) {
  // Teoria: SNR bom do cliente é mais importante que do AP para pegar o M2 do
  // handshake. 4 frames EAPOL (M1..M4) = 100% capturado completo. Menos que 4 =
  // incompleto (half-handshake), mas PMKID pode estar no M1.

  float prob = 0.0f;

  // 1. Baseado em frames capturados
  if (eapol_frames_captured >= 4)
    prob = 0.95f; // Handshake completo
  else if (eapol_frames_captured >= 2)
    prob = 0.60f; // M1+M2 geralmente basta
  else if (eapol_frames_captured == 1)
    prob = 0.30f; // Só M1 (PMKID check needed)
  else
    prob = 0.0f;

  // 2. Penalidade por sinal fraco (corrupção de pacotes)
  if (rssi_client < -80)
    prob *= 0.5f;
  else if (rssi_client < -70)
    prob *= 0.8f;

  if (rssi_ap < -85)
    prob *= 0.7f;

  return prob > 1.0f ? 1.0f : prob;
}
