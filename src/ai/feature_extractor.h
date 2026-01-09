#pragma once
#include <Arduino.h>
#include <vector>

// Estrutura de features otimizada (56 features como sugerido no Tip 2)
struct Neura9Features {
  float rssi_norm;
  float snr_norm;
  float beacon_rate;
  float probereq_rate;
  float deauth_rate;
  float assocreq_rate;
  float eapol_rate;
  float management_frame_ratio;
  float data_frame_ratio;
  float control_frame_ratio;
  float sequence_gap_score;
  float retry_flag_ratio;
  float protected_flag_ratio;
  float duration_field_avg;

  // Novas features para fingerprinting e detecção avançada
  float oui_score;            // Score baseado no fabricante OUI
  float information_elt_hash; // Hash dos elementos de informação
  float transmission_rate;    // Taxa de transmissão estimada
  float channel_switch_rate;  // Frequência de troca de canal

  // Extended features (Tip 2: 56 features total)
  float packet_size_avg;
  float packet_size_std_dev; // Desvio padrão dos tamanhos
  float wps_frame_ratio;
  float null_probe_ratio;
  float broadcast_ratio;
  float multicast_ratio;
  float fragment_rate;

  // Temporal features (Burst analysis)
  float max_burst_size; // Maior rajada de pacotes < 5ms
  float avg_inter_packet_gap;
  float gap_variance;

  // Specific Attack signatures
  float
      deauth_reason_code_dist; // 0 se normal, alto se muitos códigos estranhos
  float sequence_number_delta_avg; // Detecção de injecao (seq number jumping)
  float iv_reuse_detected;         // WEP/WPA attack signature
  float eapol_start_rate;
  float eapol_key_rate;

  // Entropy & Randomness (for potential hidden networks or random MACs)
  float mac_randomness_score;
  float ssid_entropy;

  // Physical Layer
  float channel_utilization;
  float noise_floor_est;

  // Extra placeholders to reach exactly 56 float inputs (56 * 4 = 224 bytes
  // input tensor)
  float reserved_features[20];

  void reset() { memset(this, 0, sizeof(Neura9Features)); }
};

class FeatureExtractor {
public:
  FeatureExtractor();

  // Atualiza features com base em estatísticas acumuladas
  void update(int rssi, int channel, uint8_t frameType, uint8_t frameSubtype,
              bool isProtected, bool isRetry, uint32_t timestamp);

  // Obtém features formatadas para o modelo
  const Neura9Features &getFeatures() const;

  // Reseta acumuladores (usar a cada janela de inferência, ex: 800ms)
  void resetWindow();

private:
  Neura9Features _currentFeatures;

  // Acumuladores temporários
  uint32_t _frameCount;
  int32_t _rssiSum;
  uint32_t _windowStartTime;

  // New accumulators
  uint32_t _packetSizeSum;
  uint32_t _lastPacketTime;
  uint32_t _maxBurst;
  uint32_t _currentBurst;

  // Cache para retorno seguro (necessário para getFeatures ser const e retornar
  // ref)
  Neura9Features _cachedNormalized;
};
