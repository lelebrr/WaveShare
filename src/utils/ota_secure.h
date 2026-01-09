/**
 * @file ota_secure.h
 * @brief OTA Seguro para ESP32-S3
 *
 * Implementação leve de OTA com verificação básica.
 * Otimizado para hardware limitado.
 */

#pragma once

#include <Arduino.h>
#include <Update.h>
#include <esp_ota_ops.h>

class OTASecure {
public:
  /**
   * @brief Inicia processo OTA
   * @param size Tamanho total do firmware
   * @return true se iniciou com sucesso
   */
  static bool begin(size_t size) {
    if (size == 0) {
      Serial.println("[OTA] Tamanho inválido");
      return false;
    }

    _totalSize = size;
    _written = 0;
    _lastProgress = 0;

    // Verifica espaço disponível
    const esp_partition_t *running = esp_ota_get_running_partition();
    const esp_partition_t *update = esp_ota_get_next_update_partition(running);

    if (!update) {
      Serial.println("[OTA] Partição de update não encontrada");
      return false;
    }

    if (size > update->size) {
      Serial.printf("[OTA] Firmware muito grande: %d > %d\n", size,
                    update->size);
      return false;
    }

    if (!Update.begin(size, U_FLASH)) {
      Serial.printf("[OTA] Falha ao iniciar: %s\n", Update.errorString());
      return false;
    }

    Serial.printf("[OTA] Iniciado. Tamanho: %d bytes\n", size);
    _active = true;
    return true;
  }

  /**
   * @brief Escreve chunk de dados
   * @param data Ponteiro para dados
   * @param len Tamanho do chunk
   * @return true se escreveu com sucesso
   */
  static bool writeChunk(const uint8_t *data, size_t len) {
    if (!_active || !data || len == 0) {
      return false;
    }

    size_t written = Update.write((uint8_t *)data, len);
    if (written != len) {
      Serial.printf("[OTA] Erro ao escrever: %d/%d bytes\n", written, len);
      return false;
    }

    _written += len;

    // Mostra progresso a cada 10%
    uint8_t progress = (_written * 100) / _totalSize;
    if (progress >= _lastProgress + 10) {
      _lastProgress = progress;
      Serial.printf("[OTA] Progresso: %d%%\n", progress);
    }

    return true;
  }

  /**
   * @brief Finaliza OTA
   * @param reboot Se true, reinicia após sucesso
   * @return true se finalizou com sucesso
   */
  static bool finalize(bool reboot = true) {
    if (!_active) {
      return false;
    }

    _active = false;

    if (!Update.end(true)) {
      Serial.printf("[OTA] Falha ao finalizar: %s\n", Update.errorString());
      return false;
    }

    Serial.println("[OTA] ✓ Atualização completa!");

    if (reboot) {
      Serial.println("[OTA] Reiniciando em 2s...");
      delay(2000);
      ESP.restart();
    }

    return true;
  }

  /**
   * @brief Cancela OTA em andamento
   */
  static void abort() {
    if (_active) {
      Update.abort();
      _active = false;
      Serial.println("[OTA] Cancelado");
    }
  }

  /**
   * @brief Verifica se OTA está em andamento
   */
  static bool isActive() { return _active; }

  /**
   * @brief Retorna progresso atual (0-100)
   */
  static uint8_t getProgress() {
    if (_totalSize == 0)
      return 0;
    return (_written * 100) / _totalSize;
  }

private:
  static inline bool _active = false;
  static inline size_t _totalSize = 0;
  static inline size_t _written = 0;
  static inline uint8_t _lastProgress = 0;
};

// Funções de compatibilidade com código antigo
inline bool ota_begin_secure(size_t size) { return OTASecure::begin(size); }
inline bool ota_write_chunk(const uint8_t *data, size_t len) {
  return OTASecure::writeChunk(data, len);
}
inline bool ota_finalize(bool reboot) { return OTASecure::finalize(reboot); }
