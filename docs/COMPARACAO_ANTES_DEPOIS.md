<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Documentação-ANTES%20vs%20DEPOIS-orange?style=for-the-badge" alt="Type"/>
  <img src="https://img.shields.io/badge/Correções-40+-brightgreen?style=for-the-badge" alt="Fixes"/>
  <img src="https://img.shields.io/badge/Status-✅%20Validado-brightgreen?style=for-the-badge" alt="Status"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/SH8501-❌-red?style=flat-square" alt="Wrong"/>
  <img src="https://img.shields.io/badge/→-SH8601-brightgreen?style=flat-square" alt="Right"/>
  <img src="https://img.shields.io/badge/CST816T-❌-red?style=flat-square" alt="Wrong"/>
  <img src="https://img.shields.io/badge/→-FT3168-brightgreen?style=flat-square" alt="Right"/>
</p>

---

# 📊 COMPARAÇÃO: ERRADO vs CORRETO

Este documento mostra lado a lado o que estava ERRADO e o que está CORRETO agora.

---

## 1️⃣ DISPLAY

### ❌ ANTES (ERRADO)
```
Driver IC: SH8501
Interface: QSPI (mas pinos errados)
Pinos:
  TFT_CS      = 6
  TFT_DC      = 7
  TFT_RST     = 17
  TFT_SCLK    = 47
  TFT_MOSI    = 18
  TFT_MISO    = 8
  TFT_BL      = 48
  TFT_TE      = 9
```

### ✅ AGORA (CORRETO)
```
Driver IC: SH8601
Interface: QSPI (4 data lines)
Pinos:
  LCD_SDIO0   = 4    (Data 0)
  LCD_SDIO1   = 5    (Data 1)
  LCD_SDIO2   = 6    (Data 2)
  LCD_SDIO3   = 7    (Data 3)
  LCD_SCLK    = 11   (Clock)
  LCD_CS      = 12   (Chip Select)
  
Reset/Backlight: Via IO Expander TCA9554 (I2C 0x20)
```

**Por quê mudou:**
- Este hardware usa **QSPI verdadeiro** com 4 linhas de dados
- **SH8601** é o driver correto (não SH8501)
- Reset e backlight são controlados pelo **IO Expander**, não direto pelo ESP32

---

## 2️⃣ TOUCH

### ❌ ANTES (ERRADO)
```
Controller: CST816T
Endereço I2C: 0x15
Interface: I2C
Pinos:
  TOUCH_SDA   = 38
  TOUCH_SCL   = 39
  TOUCH_INT   = 16
  TOUCH_RST   = 21
```

### ✅ AGORA (CORRETO)
```
Controller: FT3168
Endereço I2C: 0x38 ← CRÍTICO!
Interface: I2C (compartilhado)
Pinos:
  IIC_SDA     = 15
  IIC_SCL     = 14
  TP_INT      = 21
  Touch Reset: Via IO Expander TCA9554
```

**Por quê mudou:**
- Este modelo específico usa **FT3168**, não CST816T
- Endereço I2C é **0x38** (CST816T seria 0x15)
- Todos devices I2C compartilham os mesmos pinos (15/14)

---

## 3️⃣ I2C PRINCIPAL

### ❌ ANTES (ERRADO)
```
SDA = 38
SCL = 39
Devices:
  0x15 - CST816T (Touch)
  0x18 - ES8311 (Audio)
  0x34 - AXP2101 (PMU)
  0x51 - PCF85063 (RTC)
  0x6B - QMI8658C (IMU)
```

### ✅ AGORA (CORRETO)
```
SDA = 15
SCL = 14
Devices:
  0x38 - FT3168 (Touch) ← MUDOU!
  0x18 - ES8311 (Audio)
  0x34 - AXP2101 (PMU)
  0x51 - PCF85063 (RTC)
  0x6B - QMI8658C (IMU)
  0x20 - TCA9554 (IO Expander) ← NOVO!
```

**Por quê mudou:**
- Pinos I2C reais são **GPIO 15 (SDA) e 14 (SCL)**
- Touch mudou para endereço **0x38**
- **IO Expander** adicionado (estava faltando!)

---

## 4️⃣ AUDIO I2S

### ❌ ANTES (ERRADO)
```
I2S_MCLK    = 2
I2S_BCLK    = 3
I2S_WS      = 4
I2S_DOUT    = 5
I2S_DIN     = 6
CODEC_PA    = 46
```

### ✅ AGORA (CORRETO)
```
I2S_MCK_IO  = 16   (Master Clock)
I2S_BCK_IO  = 9    (Bit Clock)
I2S_WS_IO   = 45   (Word Select)
I2S_DO_IO   = 8    (Data Out)
I2S_DI_IO   = 10   (Data In)
PA          = 46   (Power Amp)
```

**Por quê mudou:**
- Pinos I2S estavam completamente errados
- Baseado agora no **schematic oficial** da Waveshare

---

## 5️⃣ SD CARD

### ❌ ANTES (ERRADO)
```
Interface: SD MMC (4-bit)
SD_CLK      = 12
SD_CMD      = 11
SD_D0       = 13
SD_D1       = 10
SD_D2       = 14
SD_D3       = 9
```

### ✅ AGORA (CORRETO)
```
Interface: SDMMC (1-bit apenas!)
SDMMC_CLK   = 2
SDMMC_CMD   = 1
SDMMC_DATA  = 3
```

**Por quê mudou:**
- Este modelo suporta apenas **1-bit mode**
- Pinos estavam errados

---

## 6️⃣ CONFIGURAÇÃO ARDUINO_GFX

### ❌ ANTES (ERRADO)
```cpp
// Exemplo LovyanGFX com pinos errados
class LGFX_WaveshareAMOLED : public lgfx::LGFX_Device {
  lgfx::Panel_SH8501 _panel_instance;  // DRIVER ERRADO!
  lgfx::Touch_CST816T _touch_instance; // TOUCH ERRADO!
  
  cfg.pin_sclk = 47;  // PINOS ERRADOS!
  cfg.pin_mosi = 18;
  cfg.pin_dc = 7;
  // ...
};
```

### ✅ AGORA (CORRETO)
```cpp
// Arduino_GFX com IO Expander
ESP_IOExpander *expander = new ESP_IOExpander_TCA95xx_8bit(
    I2C_NUM_1, 
    ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000,
    IIC_SCL, IIC_SDA  // 14, 15
);

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS,    // 12
    LCD_SCLK,  // 11
    LCD_SDIO0, // 4
    LCD_SDIO1, // 5
    LCD_SDIO2, // 6
    LCD_SDIO3  // 7
);

Arduino_GFX *gfx = new Arduino_SH8601(  // DRIVER CORRETO!
    bus, -1, 0, false, 368, 448
);
```

**Por quê mudou:**
- **SH8601** é o driver correto (não SH8501)
- **IO Expander TCA9554 é OBRIGATÓRIO** para este hardware
- Pinos QSPI corretos
- LovyanGFX sozinho **NÃO funciona** neste hardware!

---

## 7️⃣ LEITURA DO TOUCH

### ❌ ANTES (ERRADO)
```cpp
// LovyanGFX Touch (não funciona!)
auto tcfg = _touch_instance.config();
tcfg.i2c_addr = 0x15; // CST816T (ERRADO!)
```

### ✅ AGORA (CORRETO)
```cpp
// Leitura I2C direta do FT3168
#define FT3168_ADDR 0x38
#define FT3168_REG_TD_STATUS 0x02

Wire.beginTransmission(FT3168_ADDR);
Wire.write(FT3168_REG_TD_STATUS);
Wire.endTransmission();
Wire.requestFrom(FT3168_ADDR, 6);

uint8_t td_status = Wire.read();
uint8_t xh = Wire.read();
uint8_t xl = Wire.read();
uint8_t yh = Wire.read();
uint8_t yl = Wire.read();
Wire.read(); // weight

if ((td_status & 0x0F) > 0) {
    uint16_t x = ((xh & 0x0F) << 8) | xl;
    uint16_t y = ((yh & 0x0F) << 8) | yl;
    // Use x, y
}
```

**Por quê mudou:**
- Touch é **FT3168** (endereço 0x38), não CST816T
- Leitura direta via I2C é mais confiável

---

## 📋 RESUMO DAS MUDANÇAS

| Componente | ANTES (Errado) | AGORA (Correto) |
|------------|----------------|-----------------|
| **Display IC** | SH8501 | **SH8601** |
| **Touch Controller** | CST816T @ 0x15 | **FT3168 @ 0x38** |
| **I2C SDA** | 38 | **15** |
| **I2C SCL** | 39 | **14** |
| **Display SCLK** | 47 | **11** |
| **Display CS** | 6 | **12** |
| **Display SDIO** | N/A | **4,5,6,7 (QSPI)** |
| **I2S MCLK** | 2 | **16** |
| **I2S BCK** | 3 | **9** |
| **I2S WS** | 4 | **45** |
| **I2S DO** | 5 | **8** |
| **I2S DI** | 6 | **10** |
| **SD CLK** | 12 | **2** |
| **SD CMD** | 11 | **1** |
| **SD DATA** | 13,10,14,9 (4-bit) | **3 (1-bit)** |
| **IO Expander** | ❌ Ausente | **✅ TCA9554 @ 0x20** |

---

## 🎯 CONCLUSÃO

**A documentação tinha 40+ erros de hardware!**

Os principais problemas eram:
1. ❌ Driver de display errado (SH8501 vs SH8601)
2. ❌ Touch controller completamente errado (CST816T vs FT3168)
3. ❌ Endereço I2C do touch errado (0x15 vs 0x38)
4. ❌ Pinos I2C errados (38/39 vs 15/14)
5. ❌ Pinos do display todos errados
6. ❌ Pinos I2S todos errados
7. ❌ Pinos SD Card errados
8. ❌ IO Expander estava faltando completamente!

**Agora tudo está corrigido** conforme o site oficial da Waveshare! ✅

---

**Documento gerado em:** 2025-12-07  
**Baseado em:** https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8
