<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Hardware-ESP32--S3-blue?style=for-the-badge&logo=espressif&logoColor=white" alt="Hardware"/>
  <img src="https://img.shields.io/badge/Status-✅%20Corrigido-brightgreen?style=for-the-badge" alt="Status"/>
  <img src="https://img.shields.io/badge/Docs-Atualizada-green?style=for-the-badge" alt="Docs"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Display-SH8601-purple?style=flat-square" alt="Display"/>
  <img src="https://img.shields.io/badge/Touch-FT3168%20@%200x38-green?style=flat-square" alt="Touch"/>
  <img src="https://img.shields.io/badge/I2C-GPIO%2014%2F15-orange?style=flat-square" alt="I2C"/>
  <img src="https://img.shields.io/badge/IO%20Expander-TCA9554-blue?style=flat-square" alt="IO Expander"/>
</p>

---

# 🔧 CORREÇÕES DE HARDWARE - ESP32-S3-Touch-AMOLED-1.8

**Data:** 2025-12-07  
**Versão:** 1.0  
**Status:** ✅ Documentação corrigida conforme site oficial Waveshare

---

## 📋 Resumo das Correções

Esta documentação lista TODAS as correções feitas para alinhar o projeto WavePwn com as especificações **REAIS** do hardware Waveshare ESP32-S3-Touch-AMOLED-1.8, conforme o site oficial:
**https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8**

---

## ❌ ERROS CORRIGIDOS

### 1. ✅ Display Driver IC
**ANTES (ERRADO):**
- Documentação mencionava SH8501

**AGORA (CORRETO):**
- **SH8601** (driver correto do AMOLED)

**Arquivos corrigidos:**
- `docs/ESPECIFICACAO_HARDWARE.md` - linha 47
- `src/hardware/display_driver.cpp` - linhas 3, 95
- `src/hardware/display_driver.h` - linha 3

---

### 2. ✅ Touch Controller - CRÍTICO!
**ANTES (ERRADO):**
- Documentação mencionava CST816T (endereço I2C 0x15)
- display_driver.cpp usava CST816T

**AGORA (CORRETO):**
- **FT3168** (endereço I2C 0x38)
- Este é o touch controller REAL deste hardware!

**Arquivos corrigidos:**
- `docs/ESPECIFICACAO_HARDWARE.md`:
  - Título da seção (linha 134)
  - Tabela de especificações (linhas 138-146)
  - Endereço I2C em exemplo (linha 167)
  - Lista de endereços I2C (linha 547)
- `src/hardware/display_driver.cpp` - linhas 4, 14, 80, 194
- `src/hardware/display_driver.h` - linha 4
- `src/hardware/arduinogfx_display.cpp` - **JÁ ESTAVA CORRETO!**

---

### 3. ✅ Pinos do Display QSPI
**ANTES (ERRADO):**
```cpp
#define TFT_CS      6
#define TFT_DC      7
#define TFT_RST     17
#define TFT_SCLK    47
#define TFT_MOSI    18
#define TFT_MISO    8
#define TFT_BL      48
#define TFT_TE      9
```

**AGORA (CORRETO):**
```cpp
#define LCD_SDIO0   4    // Data 0 (QSPI)
#define LCD_SDIO1   5    // Data 1 (QSPI)
#define LCD_SDIO2   6    // Data 2 (QSPI)
#define LCD_SDIO3   7    // Data 3 (QSPI)
#define LCD_SCLK    11   // Clock QSPI
#define LCD_CS      12   // Chip Select
```

**NOTA IMPORTANTE:** Display reset, backlight e touch reset são controlados pelo **IO Expander TCA9554** (I2C 0x20), NÃO por pinos diretos do ESP32!

**Arquivos corrigidos:**
- `docs/ESPECIFICACAO_HARDWARE.md` - linhas 57-69
- `src/pin_config.h` - **JÁ ESTAVA CORRETO!**

---

### 4. ✅ Pinos I2C Principais
**ANTES (ERRADO):**
```cpp
#define I2C_SDA     38
#define I2C_SCL     39
```

**AGORA (CORRETO):**
```cpp
#define IIC_SDA     15
#define IIC_SCL     14
```

**Arquivos corrigidos:**
- `docs/ESPECIFICACAO_HARDWARE.md` - múltiplas seções:
  - Touch (linha 150-152)
  - PMU (linha 212-214)
  - IMU (linha 297-299)
  - RTC (linha 368-370)
  - Audio (linha 429-430)
  - I2C Principal (linha 550-551)
- `src/pin_config.h` - **JÁ ESTAVA CORRETO!**

---

### 5. ✅ Pinos I2S Audio
**ANTES (ERRADO):**
```cpp
#define I2S_MCLK    2
#define I2S_BCLK    3
#define I2S_WS      4
#define I2S_DOUT    5
#define I2S_DIN     6
```

**AGORA (CORRETO):**
```cpp
#define I2S_MCK_IO  16   // Master Clock
#define I2S_BCK_IO  9    // Bit Clock
#define I2S_WS_IO   45   // Word Select
#define I2S_DO_IO   8    // Data Out (speaker)
#define I2S_DI_IO   10   // Data In (mic)
#define PA          46   // Power Amplifier
```

**Arquivos corrigidos:**
- `docs/ESPECIFICACAO_HARDWARE.md` - linhas 429-442
- `src/pin_config.h` - **JÁ ESTAVA CORRETO!**

---

### 6. ✅ Pinos SD Card
**ANTES (ERRADO):**
```cpp
#define SD_CLK      12
#define SD_CMD      11
#define SD_D0       13
#define SD_D1       10
#define SD_D2       14
#define SD_D3       9
```

**AGORA (CORRETO):**
```cpp
#define SDMMC_CLK   2    // Clock  
#define SDMMC_CMD   1    // Command
#define SDMMC_DATA  3    // Data
```

**NOTA:** Este modelo suporta apenas **1-bit SD mode**, não 4-bit!

**Arquivos corrigidos:**
- `docs/ESPECIFICACAO_HARDWARE.md` - linhas 495-504
- `src/pin_config.h` - **JÁ ESTAVA CORRETO!**

---

### 7. ✅ Endereços I2C
**ANTES (ERRADO):**
```
0x15 - CST816T (Touch)
```

**AGORA (CORRETO):**
```
0x38 - FT3168 (Touch)
0x18 - ES8311 (Audio Codec)
0x34 - AXP2101 (PMU)
0x51 - PCF85063 (RTC)
0x6B - QMI8658C (IMU)
0x20 - TCA9554 (IO Expander) ← NOVO!
```

**Arquivos corrigidos:**
- `docs/ESPECIFICACAO_HARDWARE.md` - linhas 550-560

---

### 8. ✅ Pinout Completo
Atualizado tabela de pinout completo removendo pinos inexistentes e adicionando os corretos:

**Pinos principais:**
- **1-3:** SD Card (SDMMC)
- **4-7:** Display QSPI (SDIO0-3)
- **8-10:** Audio I2S (DO, BCK, DI)
- **11-12:** Display QSPI (CLK, CS)
- **14-15:** I2C (SCL, SDA) - **TODOS devices compartilham!**
- **16:** Audio I2S MCLK
- **21:** Touch Interrupt
- **45:** Audio I2S WS
- **46:** Audio PA Enable

**Arquivos corrigidos:**
- `docs/ESPECIFICACAO_HARDWARE.md` - linhas 614-646

---

### 9. ✅ Configuração Arduino_GFX
**ANTES:** Exemplo LovyanGFX com pinos errados

**AGORA:** Exemplo correto Arduino_GFX com:
- IO Expander TCA9554 (OBRIGATÓRIO!)
- Barramento QSPI correto
- Driver SH8601
- Touch FT3168 via I2C direto

**Arquivos corrigidos:**
- `docs/ESPECIFICACAO_HARDWARE.md` - linhas 71-130

---

## ✅ ARQUIVOS QUE JÁ ESTAVAM CORRETOS

Estes arquivos **NÃO precisaram de correção**, pois já estavam com as configurações corretas:

1. ✅ `src/pin_config.h` - **PERFEITO!**
   - Pinos do display QSPI corretos
   - Pinos I2C corretos  
   - Pinos I2S corretos
   - Pinos SD Card corretos

2. ✅ `src/hardware/arduinogfx_display.cpp` - **PERFEITO!**
   - Usa FT3168 (0x38) corretamente
   - Usa IO Expander TCA9554
   - Pinos QSPI corretos
   - Lógica de leitura touch correta

3. ✅ `platformio.ini` - **PERFEITO!**
   - Especificações de hardware corretas
   - Bibliotecas corretas

---

## 🎯 HARDWARE REAL - RESUMO

### MCU
- **Modelo:** ESP32-S3-WROOM-1-N16R8
- **Flash:** 16MB (QIO mode)
- **PSRAM:** 8MB (OPI mode)

### Display
- **Driver:** SH8601 (NÃO SH8501!)
- **Interface:** QSPI (4 data lines)
- **Resolução:** 368 x 448 pixels
- **Pinos:** 4,5,6,7 (data), 11 (clk), 12 (cs)

### Touch
- **Controller:** FT3168 (NÃO CST816T!)
- **Interface:** I2C
- **Endereço:** 0x38 (NÃO 0x15!)
- **Pinos:** 15 (SDA), 14 (SCL), 21 (INT)

### I2C Bus
- **SDA:** GPIO 15  
- **SCL:** GPIO 14
- **Frequência:** 400 kHz
- **Devices:** Touch (0x38), Audio (0x18), PMU (0x34), RTC (0x51), IMU (0x6B), IO Expander (0x20)

### IO Expander
- **Modelo:** TCA9554
- **Endereço I2C:** 0x20
- **Função:** Controla Display Reset, Display Backlight, Touch Reset

### Audio
- **Codec:** ES8311 (I2C 0x18)
- **Interface:** I2S
- **Pinos:** 16 (MCLK), 9 (BCK), 45 (WS), 8 (DO), 10 (DI), 46 (PA)

### SD Card
- **Interface:** SDMMC (1-bit mode apenas)
- **Pinos:** 2 (CLK), 1 (CMD), 3 (DATA)

---

## 📁 ARQUIVOS MODIFICADOS (RESUMO)

### Documentação
- ✅ `docs/ESPECIFICACAO_HARDWARE.md` - **EXTENSIVAMENTE CORRIGIDO**

### Código
- ✅ `src/hardware/display_driver.cpp` - Comentários corrigidos (SH8601, FT3168)
- ✅ `src/hardware/display_driver.h` - Comentário de header corrigido

### Sem modificações (já corretos)
- ✅ `src/pin_config.h`
- ✅ `src/hardware/arduinogfx_display.cpp`
- ✅ `platformio.ini`

---

## 🔍 FONTE DAS INFORMAÇÕES

Todas as correções foram baseadas em:
1. **Site oficial Waveshare:** https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8
2. **Datasheets oficiais:**
   - SH8601 (Display IC)
   - FT3168 (Touch Controller)
   - AXP2101 (PMU)
   - QMI8658C (IMU)
   - PCF85063 (RTC)
   - ES8311 (Audio Codec)
3. **Exemplos oficiais Arduino v3.1.0** da Waveshare

---

## ⚠️ NOTAS IMPORTANTES

1. **IO Expander é OBRIGATÓRIO:** Este hardware usa TCA9554 para controlar reset e backlight do display. Arduino_GFX requer isso, LovyanGFX puro NÃO funciona!

2. **Touch é FT3168, não CST816T:** Muitos projetos similares usam CST816T, mas este modelo específico usa FT3168. Endereço I2C diferente!

3. **I2C compartilhado:** TODOS os periféricos I2C (Touch, Audio, PMU, RTC, IMU, IO Expander) usam os mesmos pinos (15=SDA, 14=SCL).

4. **Display QSPI:** Não é SPI normal! Usa 4 linhas de dados (SDIO0-3) em modo QSPI.

5. **SD Card 1-bit apenas:** Este modelo não suporta 4-bit SD mode.

---

## ✅ PRÓXIMOS PASSOS RECOMENDADOS

1. **Testar build:** Rodar `pio run` para garantir que tudo compila
2. **Testar display:** Verificar se display inicializa corretamente com SH8601
3. **Testar touch:** Verificar leitura do FT3168 no endereço 0x38
4. **Verificar I2C:** Fazer scan I2C para confirmar todos devices
5. **Testar IO Expander:** Confirmar controle de reset/backlight via TCA9554

---

**Documento gerado em:** 2025-12-07 02:55 BRT  
**Autor:** Antigravity AI  
**Status:** ✅ Completo
