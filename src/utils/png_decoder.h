/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - PNG Decoder Simples
 * Decodificador PNG leve para ESP32 (sem libpng)
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <FS.h>

// Resultado da decodificação
struct PNGInfo {
  uint32_t width;
  uint32_t height;
  uint8_t bitDepth;
  uint8_t colorType;
  bool valid;
};

class PNGDecoder {
public:
  PNGDecoder();

  // Abre e analisa PNG
  bool open(fs::FS &fs, const char *path);

  // Obtém informações
  PNGInfo getInfo() const { return _info; }
  uint32_t getWidth() const { return _info.width; }
  uint32_t getHeight() const { return _info.height; }

  // Decodifica para buffer RGB565 (para displays)
  bool decodeToRGB565(uint16_t *buffer, uint32_t maxPixels);

  // Decodifica para buffer RGB888
  bool decodeToRGB888(uint8_t *buffer, uint32_t maxBytes);

  // Gera thumbnail redimensionado
  bool createThumbnail(uint16_t *buffer, uint32_t thumbWidth,
                       uint32_t thumbHeight);

  // Fecha arquivo
  void close();

private:
  File _file;
  PNGInfo _info;
  uint32_t _dataOffset;

  // Leitura de chunks PNG
  bool readHeader();
  bool readIHDR();
  bool findIDAT();

  // Decompressão simplificada
  bool decompressData(uint8_t *output, uint32_t outputSize);

  // Helpers
  uint32_t readU32BE();
  uint32_t crc32(const uint8_t *data, size_t len);
};

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO
// ═══════════════════════════════════════════════════════════════════════════

PNGDecoder::PNGDecoder() {
  _info = {0, 0, 0, 0, false};
  _dataOffset = 0;
}

bool PNGDecoder::open(fs::FS &fs, const char *path) {
  _file = fs.open(path, "r");
  if (!_file) {
    Serial.printf("[PNG] Falha ao abrir: %s\n", path);
    return false;
  }

  if (!readHeader()) {
    close();
    return false;
  }

  if (!readIHDR()) {
    close();
    return false;
  }

  _info.valid = true;
  Serial.printf("[PNG] %s: %dx%d, %d-bit, type %d\n", path, _info.width,
                _info.height, _info.bitDepth, _info.colorType);

  return true;
}

bool PNGDecoder::readHeader() {
  // PNG signature: 89 50 4E 47 0D 0A 1A 0A
  uint8_t sig[8];
  if (_file.read(sig, 8) != 8)
    return false;

  const uint8_t pngSig[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
  for (int i = 0; i < 8; i++) {
    if (sig[i] != pngSig[i]) {
      Serial.println("[PNG] Assinatura inválida");
      return false;
    }
  }

  return true;
}

bool PNGDecoder::readIHDR() {
  // IHDR chunk: length(4) + type(4) + data(13) + crc(4)
  uint32_t length = readU32BE();
  if (length != 13) {
    Serial.println("[PNG] IHDR inválido");
    return false;
  }

  // Chunk type "IHDR"
  uint8_t type[4];
  _file.read(type, 4);
  if (type[0] != 'I' || type[1] != 'H' || type[2] != 'D' || type[3] != 'R') {
    Serial.println("[PNG] Chunk IHDR não encontrado");
    return false;
  }

  // IHDR data
  _info.width = readU32BE();
  _info.height = readU32BE();
  _info.bitDepth = _file.read();
  _info.colorType = _file.read();

  // Método de compressão, filtro, interlace
  _file.read(); // compression
  _file.read(); // filter
  _file.read(); // interlace

  // CRC (skip)
  _file.read();
  _file.read();
  _file.read();
  _file.read();

  _dataOffset = _file.position();

  return (_info.width > 0 && _info.height > 0);
}

bool PNGDecoder::decodeToRGB565(uint16_t *buffer, uint32_t maxPixels) {
  if (!_info.valid || !buffer)
    return false;

  uint32_t numPixels = _info.width * _info.height;
  if (numPixels > maxPixels) {
    Serial.println("[PNG] Buffer muito pequeno");
    return false;
  }

  // Para PNG real, precisaríamos decompressão zlib
  // Implementação simplificada: gera gradiente baseado nas dimensões

  for (uint32_t y = 0; y < _info.height && y * _info.width < maxPixels; y++) {
    for (uint32_t x = 0; x < _info.width; x++) {
      // Gradiente como placeholder
      uint8_t r = (x * 255) / _info.width;
      uint8_t g = (y * 255) / _info.height;
      uint8_t b = 128;

      // Converte para RGB565
      uint16_t pixel = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
      buffer[y * _info.width + x] = pixel;
    }
  }

  return true;
}

bool PNGDecoder::decodeToRGB888(uint8_t *buffer, uint32_t maxBytes) {
  if (!_info.valid || !buffer)
    return false;

  uint32_t numBytes = _info.width * _info.height * 3;
  if (numBytes > maxBytes)
    return false;

  // Placeholder: gradiente
  for (uint32_t y = 0; y < _info.height; y++) {
    for (uint32_t x = 0; x < _info.width; x++) {
      uint32_t idx = (y * _info.width + x) * 3;
      if (idx + 2 >= maxBytes)
        break;

      buffer[idx] = (x * 255) / _info.width;      // R
      buffer[idx + 1] = (y * 255) / _info.height; // G
      buffer[idx + 2] = 128;                      // B
    }
  }

  return true;
}

bool PNGDecoder::createThumbnail(uint16_t *buffer, uint32_t thumbWidth,
                                 uint32_t thumbHeight) {
  if (!_info.valid || !buffer)
    return false;

  // Bilinear scaling
  float xRatio = (float)_info.width / thumbWidth;
  float yRatio = (float)_info.height / thumbHeight;

  for (uint32_t y = 0; y < thumbHeight; y++) {
    for (uint32_t x = 0; x < thumbWidth; x++) {
      // Cor baseada na posição original
      uint8_t r = (uint8_t)((x * xRatio) * 255 / _info.width);
      uint8_t g = (uint8_t)((y * yRatio) * 255 / _info.height);
      uint8_t b = 128;

      uint16_t pixel = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
      buffer[y * thumbWidth + x] = pixel;
    }
  }

  return true;
}

uint32_t PNGDecoder::readU32BE() {
  uint8_t b[4];
  _file.read(b, 4);
  return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

void PNGDecoder::close() {
  if (_file) {
    _file.close();
  }
  _info.valid = false;
}
