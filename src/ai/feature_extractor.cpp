#include "feature_extractor.h"

FeatureExtractor::FeatureExtractor() { resetWindow(); }

void FeatureExtractor::resetWindow() {
  _currentFeatures.reset();
  _frameCount = 0;
  _rssiSum = 0;
  _frameCount = 0;
  _rssiSum = 0;
  _packetSizeSum = 0;
  _lastPacketTime = 0;
  _maxBurst = 0;
  _currentBurst = 0;
  _windowStartTime = millis();
}

// Auxiliar para entropia simples (Shannon approx)
float calculateEntropy(const uint8_t *data, size_t len) {
  if (len == 0)
    return 0;
  float entropy = 0;
  int counts[256] = {0};
  for (size_t i = 0; i < len; i++)
    counts[data[i]]++;
  for (int i = 0; i < 256; i++) {
    if (counts[i] > 0) {
      float p = (float)counts[i] / len;
      entropy -= p * log2(p);
    }
  }
  return entropy;
}

void FeatureExtractor::update(int rssi, int channel, uint8_t frameType,
                              uint8_t frameSubtype, bool isProtected,
                              bool isRetry, uint32_t timestamp) {
  _frameCount++;
  _rssiSum += rssi;

  // Packet Gap & Burst Analysis
  uint32_t dt = timestamp - _lastPacketTime;
  if (dt < 5) { // < 5ms considera burst
    _currentBurst++;
  } else {
    if (_currentBurst > _maxBurst)
      _maxBurst = _currentBurst;
    _currentBurst = 0;
  }
  _lastPacketTime = timestamp;

  // Frame Size Analysis (Simulado pois não passamos len, assumindo média por
  // tipo)
  int estimatedLen = 0;
  if (frameType == 0)
    estimatedLen = 100; // Mgmt
  if (frameType == 1)
    estimatedLen = 24; // Ctrl
  if (frameType == 2)
    estimatedLen = 500; // Data
  _packetSizeSum += estimatedLen;

  // Tipo de Frame (Management = 0, Control = 1, Data = 2)
  if (frameType == 0) {
    _currentFeatures.management_frame_ratio += 1.0f;
    // Subtipos Mgmt
    if (frameSubtype == 8)
      _currentFeatures.beacon_rate += 1.0f;
    if (frameSubtype == 4)
      _currentFeatures.probereq_rate += 1.0f;
    if (frameSubtype == 12) {
      _currentFeatures.deauth_rate += 1.0f;
      _currentFeatures.deauth_reason_code_dist +=
          1.0f; // Simplified: count occurrences
    }
    if (frameSubtype == 0)
      _currentFeatures.assocreq_rate += 1.0f;
  } else if (frameType == 1) {
    _currentFeatures.control_frame_ratio += 1.0f;
  } else if (frameType == 2) {
    _currentFeatures.data_frame_ratio += 1.0f;
    if (frameSubtype == 0)
      _currentFeatures.data_frame_ratio += 1.0f; // Data normal
    if (frameSubtype == 4)
      _currentFeatures.null_probe_ratio += 1.0f; // Null function
  }

  // Flags
  if (isRetry)
    _currentFeatures.retry_flag_ratio += 1.0f;
  if (isProtected)
    _currentFeatures.protected_flag_ratio += 1.0f;

  // EAPOL Check (Subtype não é suficiente, seria necessário olhar EthereType,
  // assumindo que quem chamou sabe filtrar ou isso é aproximado)
  if (isProtected && estimatedLen > 100) {
    _currentFeatures.eapol_key_rate += 1.0f; // Heuristica fraca
  }
}

const Neura9Features &FeatureExtractor::getFeatures() const {
  // Usamos const_cast para atualizar o cache "lazy" sem remover const da
  // assinatura
  Neura9Features &result =
      const_cast<FeatureExtractor *>(this)->_cachedNormalized;
  result = _currentFeatures; // Inicia com somas brutas

  if (_frameCount > 0) {
    float durationSec = (millis() - _windowStartTime) / 1000.0f;
    if (durationSec < 0.1f)
      durationSec = 0.1f;

    // Normaliza RSSI
    float avgRssi = (float)_rssiSum / _frameCount;
    result.rssi_norm = constrain((avgRssi + 100.0f) / 100.0f, 0.0f, 1.0f);

    // Ratas por segundo
    result.beacon_rate = (_currentFeatures.beacon_rate / durationSec) / 100.0f;
    result.probereq_rate =
        (_currentFeatures.probereq_rate / durationSec) / 100.0f;
    result.deauth_rate = (_currentFeatures.deauth_rate / durationSec) / 50.0f;

    // Proporções
    result.management_frame_ratio =
        _currentFeatures.management_frame_ratio / _frameCount;
    result.data_frame_ratio = _currentFeatures.data_frame_ratio / _frameCount;
    result.control_frame_ratio =
        _currentFeatures.control_frame_ratio / _frameCount;
    result.retry_flag_ratio = _currentFeatures.retry_flag_ratio / _frameCount;
    result.protected_flag_ratio =
        _currentFeatures.protected_flag_ratio / _frameCount;

    // New Features Normalization
    result.packet_size_avg =
        (_packetSizeSum / _frameCount) / 1500.0f; // Norm 0-1
    result.max_burst_size = _maxBurst / 50.0f;    // Assumindo burst max 50
    result.eapol_key_rate =
        (_currentFeatures.eapol_key_rate / durationSec) / 10.0f;
    result.null_probe_ratio = _currentFeatures.null_probe_ratio / _frameCount;

    // Entropy / Noise (Random simulation for demo if not driven by real data)
    result.channel_utilization =
        (_frameCount * 10.0f) / 1000.0f; // Crude util estimate

    // Limita 0.0 - 1.0
    result.beacon_rate = constrain(result.beacon_rate, 0.0f, 1.0f);
    result.probereq_rate = constrain(result.probereq_rate, 0.0f, 1.0f);
    result.deauth_rate = constrain(result.deauth_rate, 0.0f, 1.0f);
    result.channel_utilization =
        constrain(result.channel_utilization, 0.0f, 1.0f);
  } else {
    result.reset();
  }

  return result;
}
