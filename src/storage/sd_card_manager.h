/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - SD Card Manager
 * Gerenciador de cartão SD com inicialização correta
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>

// Pinos SD Card para Waveshare ESP32-S3-Touch-AMOLED-1.8
#define SDMMC_CLK 12
#define SDMMC_CMD 16
#define SDMMC_DATA 14

class SDCardManager {
public:
  SDCardManager();

  // Inicialização
  bool begin();
  void end();

  // Status
  bool isReady() const { return _ready; }
  bool isMounted() const { return _mounted; }

  // Informações
  uint64_t getTotalBytes() const;
  uint64_t getUsedBytes() const;
  uint64_t getFreeBytes() const;

  // Sistema de arquivos
  fs::FS &getFS() { return SD_MMC; }

  // Operações de arquivo
  bool exists(const char *path);
  File open(const char *path, const char *mode = "r");
  bool mkdir(const char *path);
  bool remove(const char *path);
  bool rename(const char *oldPath, const char *newPath);

  // Listagem de diretório
  void listDir(const char *dirname, uint8_t levels = 1);

  // Logs e capturas
  bool appendLog(const char *filename, const char *message);
  bool saveCapture(const char *filename, const uint8_t *data, size_t len);

  // Handshakes
  bool saveHandshake(const char *ssid, const uint8_t *data, size_t len);
  int countHandshakes();

private:
  bool _ready;
  bool _mounted;

  void logError(const char *msg);
};

// Instância global
extern SDCardManager sd_card;

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO
// ═══════════════════════════════════════════════════════════════════════════

SDCardManager sd_card;

SDCardManager::SDCardManager() : _ready(false), _mounted(false) {}

bool SDCardManager::begin() {
  Serial.println("[SD] Inicializando cartão SD...");

  // Configura pinos SD_MMC
  SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);

  // Monta em modo 1-bit (mais compatível)
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("[SD] Erro ao montar SD. Tentando novamente...");
    delay(100);

    // Segunda tentativa
    if (!SD_MMC.begin("/sdcard", true)) {
      logError("Falha ao montar SD_MMC");
      _ready = false;
      _mounted = false;
      return false;
    }
  }

  // Verifica tipo de cartão
  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    logError("Nenhum cartão SD detectado");
    _ready = false;
    _mounted = false;
    return false;
  }

  const char *typeStr = "DESCONHECIDO";
  switch (cardType) {
  case CARD_MMC:
    typeStr = "MMC";
    break;
  case CARD_SD:
    typeStr = "SD";
    break;
  case CARD_SDHC:
    typeStr = "SDHC";
    break;
  }

  _ready = true;
  _mounted = true;

  Serial.printf("[SD] Tipo: %s\n", typeStr);
  Serial.printf("[SD] Total: %.2f MB\n", getTotalBytes() / (1024.0 * 1024.0));
  Serial.printf("[SD] Usado: %.2f MB\n", getUsedBytes() / (1024.0 * 1024.0));
  Serial.printf("[SD] Livre: %.2f MB\n", getFreeBytes() / (1024.0 * 1024.0));

  // Cria diretórios padrão
  mkdir("/handshakes");
  mkdir("/logs");
  mkdir("/wallpapers");
  mkdir("/sounds");
  mkdir("/captures");
  mkdir("/config");

  Serial.println("[SD] Cartão SD inicializado com sucesso!");
  return true;
}

void SDCardManager::end() {
  if (_mounted) {
    SD_MMC.end();
    _mounted = false;
    _ready = false;
    Serial.println("[SD] Cartão SD desmontado");
  }
}

uint64_t SDCardManager::getTotalBytes() const {
  return _mounted ? SD_MMC.totalBytes() : 0;
}

uint64_t SDCardManager::getUsedBytes() const {
  return _mounted ? SD_MMC.usedBytes() : 0;
}

uint64_t SDCardManager::getFreeBytes() const {
  return getTotalBytes() - getUsedBytes();
}

bool SDCardManager::exists(const char *path) {
  return _mounted && SD_MMC.exists(path);
}

File SDCardManager::open(const char *path, const char *mode) {
  if (!_mounted)
    return File();
  return SD_MMC.open(path, mode);
}

bool SDCardManager::mkdir(const char *path) {
  if (!_mounted)
    return false;
  if (SD_MMC.exists(path))
    return true;
  return SD_MMC.mkdir(path);
}

bool SDCardManager::remove(const char *path) {
  return _mounted && SD_MMC.remove(path);
}

bool SDCardManager::rename(const char *oldPath, const char *newPath) {
  return _mounted && SD_MMC.rename(oldPath, newPath);
}

void SDCardManager::listDir(const char *dirname, uint8_t levels) {
  if (!_mounted) {
    Serial.println("[SD] Cartão não montado");
    return;
  }

  Serial.printf("[SD] Listando: %s\n", dirname);

  File root = SD_MMC.open(dirname);
  if (!root || !root.isDirectory()) {
    Serial.println("[SD] Falha ao abrir diretório");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.printf("  📁 %s/\n", file.name());
      if (levels > 0) {
        listDir(file.path(), levels - 1);
      }
    } else {
      Serial.printf("  📄 %s (%d bytes)\n", file.name(), file.size());
    }
    file = root.openNextFile();
  }
}

bool SDCardManager::appendLog(const char *filename, const char *message) {
  if (!_mounted)
    return false;

  char path[64];
  snprintf(path, sizeof(path), "/logs/%s", filename);

  File file = SD_MMC.open(path, FILE_APPEND);
  if (!file)
    return false;

  // Timestamp
  char timestamp[32];
  snprintf(timestamp, sizeof(timestamp), "[%lu] ", millis() / 1000);
  file.print(timestamp);
  file.println(message);
  file.close();

  return true;
}

bool SDCardManager::saveCapture(const char *filename, const uint8_t *data,
                                size_t len) {
  if (!_mounted || !data || len == 0)
    return false;

  char path[64];
  snprintf(path, sizeof(path), "/captures/%s", filename);

  File file = SD_MMC.open(path, FILE_WRITE);
  if (!file)
    return false;

  size_t written = file.write(data, len);
  file.close();

  Serial.printf("[SD] Captura salva: %s (%d bytes)\n", filename, written);
  return written == len;
}

bool SDCardManager::saveHandshake(const char *ssid, const uint8_t *data,
                                  size_t len) {
  if (!_mounted || !data || len == 0)
    return false;

  char filename[64];
  snprintf(filename, sizeof(filename), "/handshakes/%s_%lu.pcap", ssid,
           millis());

  // Limpa caracteres inválidos
  for (int i = 0; filename[i]; i++) {
    if (filename[i] == ' ' || filename[i] == '/' || filename[i] == '\\') {
      filename[i] = '_';
    }
  }

  File file = SD_MMC.open(filename, FILE_WRITE);
  if (!file)
    return false;

  // Escreve PCAP header
  uint8_t pcapHeader[] = {
      0xd4, 0xc3, 0xb2, 0xa1, // Magic number
      0x02, 0x00, 0x04, 0x00, // Version
      0x00, 0x00, 0x00, 0x00, // Timezone
      0x00, 0x00, 0x00, 0x00, // Sigfigs
      0xff, 0xff, 0x00, 0x00, // Snaplen
      0x69, 0x00, 0x00, 0x00  // Link type (802.11)
  };
  file.write(pcapHeader, sizeof(pcapHeader));

  // Escreve dados
  file.write(data, len);
  file.close();

  Serial.printf("[SD] Handshake salvo: %s\n", filename);
  return true;
}

int SDCardManager::countHandshakes() {
  if (!_mounted)
    return 0;

  int count = 0;
  File dir = SD_MMC.open("/handshakes");
  if (!dir || !dir.isDirectory())
    return 0;

  File file = dir.openNextFile();
  while (file) {
    if (!file.isDirectory() && strstr(file.name(), ".pcap")) {
      count++;
    }
    file = dir.openNextFile();
  }

  return count;
}

void SDCardManager::logError(const char *msg) {
  Serial.printf("[SD] ERRO: %s\n", msg);
}
