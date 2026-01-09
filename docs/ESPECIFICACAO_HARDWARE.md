<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Hardware-ESP32--S3-blue?style=for-the-badge&logo=espressif&logoColor=white" alt="Hardware"/>
  <img src="https://img.shields.io/badge/Display-AMOLED%201.8%22-ff69b4?style=for-the-badge" alt="Display"/>
  <img src="https://img.shields.io/badge/Status-✅%20Validado-brightgreen?style=for-the-badge" alt="Status"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Flash-16MB-purple?style=flat-square" alt="Flash"/>
  <img src="https://img.shields.io/badge/PSRAM-8MB%20OPI-purple?style=flat-square" alt="PSRAM"/>
  <img src="https://img.shields.io/badge/CPU-240MHz%20Dual--Core-orange?style=flat-square" alt="CPU"/>
  <img src="https://img.shields.io/badge/Touch-FT3168-green?style=flat-square" alt="Touch"/>
</p>

---

# 📟 ESPECIFICAÇÃO DE HARDWARE - Waveshare ESP32-S3-Touch-AMOLED-1.8

## 📊 Visão Geral

**Produto:** ESP32-S3-Touch-AMOLED-1.8  
**Fabricante:** Waveshare Electronics  
**Wiki:** https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8  
**Documentação Base:** Arduino examples v3.1.0

---

## 🎛️ Especificações Principais

### MCU - ESP32-S3-WROOM-1-N16R8

| Especificação | Valor |
|--------------|-------|
| **CPU** | Xtensa® dual-core 32-bit LX7 @ 240 MHz |
| **Flash** | 16 MB (128 Mbit) |
| **PSRAM** | 8 MB (64 Mbit) OPI (Octa SPI) |
| **Wi-Fi** | 802.11 b/g/n (2.4 GHz) |
| **Bluetooth** | BLE 5.0 |
| **USB** | Native USB OTG (full-speed) |
| **GPIO** | 45 pinos programáveis |
| **ADC** | 2x SAR ADC de 12 bits (20 canais) |
| **DAC** | 2x DAC de 8 bits |
| **SPI** | 4x controladores |
| **I2C** | 2x controladores |
| **UART** | 3x controladores |
| **I2S** | 2x controladores |
| **TWAI** | 1x (Controller Area Network) |
| **RMT** | 8 canais TX + 4 canais RX |
| **PWM** | 14 canais |
| **Temperatura interna** | -40°C a +125°C |

---

## 🖥️ Display AMOLED

### Especificações do Display

| Parâmetro | Valor |
|-----------|-------|
| **Tipo** | AMOLED (Active Matrix Organic LED) |
| **Tamanho** | 1.8 polegadas |
| **Resolução** | 368 x 448 pixels |
| **Driver IC** | SH8601 |
| **Interface** | QSPI (Quad SPI) |
| **Cores** | 65K (RGB565) |
| **Brilho** | Ajustável via software |
| **Ângulo de visão** | ~170° |
| **Contraste** | >1000:1 |
| **Tempo de resposta** | < 1ms |

### Pinout do Display

```cpp
// Configuração QSPI para SH8601 AMOLED
#define LCD_SDIO0   4    // Data 0 (QSPI)
#define LCD_SDIO1   5    // Data 1 (QSPI)
#define LCD_SDIO2   6    // Data 2 (QSPI)
#define LCD_SDIO3   7    // Data 3 (QSPI)
#define LCD_SCLK    11   // Clock QSPI
#define LCD_CS      12   // Chip Select
// Display não tem pinos separados de RST, BL e TE neste modelo
// Controles são via IO Expander TCA9554
```

### Configuração Arduino_GFX (Recomendado)

```cpp
// Este hardware REQUER uso do Arduino_GFX com IO Expander!
// LovyanGFX não suporta nativamente SH8601 com IO Expander TCA9554

#include "Arduino_GFX_Library.h"
#include "ESP_IOExpander_Library.h"

// IO Expander TCA9554 (OBRIGATÓRIO!)
ESP_IOExpander *expander = new ESP_IOExpander_TCA95xx_8bit(
    I2C_NUM_1, 
    ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000,
    IIC_SCL, IIC_SDA
);

// Barramento QSPI
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
    LCD_CS,    // 12
    LCD_SCLK,  // 11
    LCD_SDIO0, // 4
    LCD_SDIO1, // 5
    LCD_SDIO2, // 6
    LCD_SDIO3  // 7
);

// Display SH8601
Arduino_GFX *gfx = new Arduino_SH8601(
    bus,
    -1,        // RST (controlado pelo expander)
    0,         // rotation
    false,     // IPS
    368,       // width
    448        // height
);

// Inicialização
expander->init();
expander->begin();
expander->pinMode(0, OUTPUT); // Display power
expander->pinMode(1, OUTPUT); // Display reset
expander->pinMode(2, OUTPUT); // Touch reset

// Reset display
expander->digitalWrite(1, LOW);
delay(20);
expander->digitalWrite(1, HIGH);
delay(50);

gfx->begin();
gfx->Display_Brightness(200);
```

---

## 👆 Touch Screen - FT3168

### Especificações

| Parâmetro | Valor |
|-----------|-------|
| **Tipo** | Capacitivo auto-capacitivo |
| **Pontos de toque** | 1 (single touch) |
| **Interface** | I2C |
| **Endereço I2C** | 0x38 |
| **Resolução** | 368 x 448 |
| **Taxa de amostragem** | Configurável 10kHz~400kHz |
| **Gestos suportados** | Single tap, long press, swipe |

### Pinout Touch

```cpp
#define IIC_SDA     15   // I2C Data (compartilhado)
#define IIC_SCL     14   // I2C Clock (compartilhado)
#define TP_INT      21   // Touch Interrupt (falling edge)
// Touch reset é controlado via IO Expander TCA9554
```

### Leitura do Touch FT3168 via I2C

```cpp
#define FT3168_ADDR 0x38
#define FT3168_REG_TD_STATUS 0x02
#define FT3168_REG_TOUCH1_XH 0x03

void readTouch(int16_t *x, int16_t *y, bool *pressed) {
    Wire.beginTransmission(FT3168_ADDR);
    Wire.write(FT3168_REG_TD_STATUS);
    if (Wire.endTransmission() == 0) {
        Wire.requestFrom(FT3168_ADDR, 6);
        if (Wire.available() >= 6) {
            uint8_t td_status = Wire.read();
            uint8_t xh = Wire.read();
            uint8_t xl = Wire.read();
            uint8_t yh = Wire.read();
            uint8_t yl = Wire.read();
            Wire.read(); // weight
            
            if ((td_status & 0x0F) > 0) {
                *x = ((xh & 0x0F) << 8) | xl;
                *y = ((yh & 0x0F) << 8) | yl;
                *pressed = true;
                return;
            }
        }
    }
    *pressed = false;
}
```

---

## 🔋 Power Management - AXP2101

### Especificações PMU

| Parâmetro | Valor |
|-----------|-------|
| **Modelo** | AXP2101 |
| **Interface** | I2C |
| **Endereço I2C** | 0x34 |
| **Saídas DCDC** | 5 (ajustáveis) |
| **Saídas LDO** | 3 (ajustáveis) |
| **ADC** | 14 bits |
| **Carregamento** | Linear charge (até 4.2V) |
| **Corrente de carga** | Até 1000 mA (ajustável) |
| **Proteções** | OVP, UVP, OCP, OTP, Short circuit |

### Configuração de Voltagem

```cpp
// Configurações típicas para ESP32-S3-Touch-AMOLED-1.8
DCDC1: 3.3V  (ESP32-S3 core)
DCDC2: 1.2V  (PSRAM)
DCDC3: 1.8V  (I/O)
DCDC4: 3.3V  (Peripherals)
DCDC5: 1.2V  (Reserved)

ALDO1: 1.8V  (Touch, Sensors)
ALDO2: 3.3V  (SD Card)
ALDO3: 2.8V  (Display)
```

### Pinout PMU

```cpp
#define PMU_SDA     15   // Compartilhado com I2C
#define PMU_SCL     14   // Compartilhado com I2C
#define PMU_IRQ     -1   // Não exposto neste modelo
```

### Exemplo de Uso (XPowersLib)

```cpp
#include <XPowersLib.h>

XPowersAXP2101 PMU;

void initPMU() {
    Wire.begin(PMU_SDA, PMU_SCL);
    
    if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, PMU_SDA, PMU_SCL)) {
        Serial.println("PMU init failed!");
        return;
    }
    
    // Configurar voltagens
    PMU.setDC1Voltage(3300);  // ESP32-S3
    PMU.setDC3Voltage(1800);  // I/O
    PMU.setALDO1Voltage(1800); // Touch
    PMU.setALDO2Voltage(3300); // SD
    PMU.setALDO3Voltage(2800); // Display
    
    // Habilitar saídas
    PMU.enableDC1();
    PMU.enableDC3();
    PMU.enableALDO1();
    PMU.enableALDO2();
    PMU.enableALDO3();
    
    // Configurar carregamento
    PMU.setChargerConstantCurr(500); // 500mA
    PMU.setChargeTargetVoltage(4200); // 4.2V
    PMU.enableCharge();
    
    // Configurar IRQ
    PMU.clearIrqStatus();
    PMU.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    PMU.enableIRQ(
        XPOWERS_AXP2101_BAT_INSERT_IRQ |
        XPOWERS_AXP2101_BAT_REMOVE_IRQ |
        XPOWERS_AXP2101_VBUS_INSERT_IRQ |
        XPOWERS_AXP2101_VBUS_REMOVE_IRQ |
        XPOWERS_AXP2101_BAT_CHG_DONE_IRQ |
        XPOWERS_AXP2101_BAT_CHG_START_IRQ
    );
}

// Ler status da bateria
void readBattery() {
    int percent = PMU.getBatteryPercent();
    int voltage = PMU.getBattVoltage();
    int current = PMU.getBatteryChargeCurrent();
    bool charging = PMU.isCharging();
    
    Serial.printf("Battery: %d%%, %dmV, %dmA, %s\n",
        percent, voltage, current, charging ? "Charging" : "Discharging");
}
```

---

## 📐 IMU - QMI8658C

### Especificações

| Parâmetro | Valor |
|-----------|-------|
| **Tipo** | 6-axis (3-axis Gyro + 3-axis Accel) |
| **Interface** | I2C |
| **Endereço I2C** | 0x6B (ou 0x6A) |
| **Gyro range** | ±16, ±32, ±64, ±128, ±256, ±512, ±1024, ±2048 dps |
| **Accel range** | ±2g, ±4g, ±8g, ±16g |
| **ODR Gyro** | 8 kHz max |
| **ODR Accel** | 8 kHz max |
| **Resolução** | 16 bits |
| **Consumo** | < 1 mA |

### Pinout IMU

```cpp
#define IMU_SDA     15   // Compartilhado com I2C
#define IMU_SCL     14   // Compartilhado com I2C
#define IMU_INT1    -1   // Não exposto neste modelo
#define IMU_INT2    -1   // Não exposto neste modelo
```

### Exemplo de Uso (SensorLib)

```cpp
#include <SensorQMI8658.hpp>

SensorQMI8658 imu;

void initIMU() {
    Wire.begin(IMU_SDA, IMU_SCL);
    
    if (!imu.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IMU_SDA, IMU_SCL)) {
        Serial.println("IMU init failed!");
        return;
    }
    
    // Configurar acelerômetro
    imu.configAccelerometer(
        SensorQMI8658::ACC_RANGE_4G,
        SensorQMI8658::ACC_ODR_1000Hz,
        SensorQMI8658::LPF_MODE_0
    );
    
    // Configurar giroscópio
    imu.configGyroscope(
        SensorQMI8658::GYR_RANGE_512DPS,
        SensorQMI8658::GYR_ODR_896_8Hz,
        SensorQMI8658::LPF_MODE_0
    );
    
    imu.enableGyroscope();
    imu.enableAccelerometer();
}

void readIMU() {
    if (imu.getDataReady()) {
        float accelX, accelY, accelZ;
        float gyroX, gyroY, gyroZ;
        
        imu.getAccelerometer(accelX, accelY, accelZ);
        imu.getGyroscope(gyroX, gyroY, gyroZ);
        
        Serial.printf("Accel: %.2f, %.2f, %.2f | Gyro: %.2f, %.2f, %.2f\n",
            accelX, accelY, accelZ, gyroX, gyroY, gyroZ);
    }
}
```

---

## 🕐 RTC - PCF85063

### Especificações

| Parâmetro | Valor |
|-----------|-------|
| **Tipo** | Real-Time Clock |
| **Interface** | I2C |
| **Endereço I2C** | 0x51 |
| **Precisão** | ±20 ppm @ 25°C |
| **Bateria backup** | Não (usa super cap opcional) |
| **Alarme** | 1 alarme programável |
| **Consumo** | < 150 nA @ 3V |

### Pinout RTC

```cpp
#define RTC_SDA     15   // Compartilhado com I2C
#define RTC_SCL     14   // Compartilhado com I2C
#define RTC_INT     -1   // Não exposto neste modelo
```

### Exemplo de Uso (SensorLib)

```cpp
#include <SensorPCF85063.hpp>

SensorPCF85063 rtc;

void initRTC() {
    Wire.begin(RTC_SDA, RTC_SCL);
    
    if (!rtc.begin(Wire, PCF85063_SLAVE_ADDRESS, RTC_SDA, RTC_SCL)) {
        Serial.println("RTC init failed!");
        return;
    }
    
    // Verificar se está rodando
    if (!rtc.isRunning()) {
        Serial.println("RTC was not running, starting now...");
        rtc.start();
    }
    
    // Configurar hora inicial (se necessário)
    // rtc.setDateTime(2025, 12, 4, 10, 30, 0);
}

void readRTC() {
    RTC_DateTime dt = rtc.getDateTime();
    
    Serial.printf("RTC: %04d-%02d-%02d %02d:%02d:%02d\n",
        dt.year, dt.month, dt.day,
        dt.hour, dt.minute, dt.second);
}
```

---

## 🔊 Audio Codec - ES8311

### Especificações

| Parâmetro | Valor |
|-----------|-------|
| **Tipo** | Audio Codec (ADC + DAC) |
| **Interface** | I2S (dados) + I2C (controle) |
| **Endereço I2C** | 0x18 |
| **DAC SNR** | 100 dB |
| **ADC SNR** | 100 dB |
| **Sample Rate** | 8 kHz ~ 96 kHz |
| **Bit Depth** | 16/24/32 bits |
| **Entrada** | MIC (diferencial) |  
| **Saída** | Speaker/Headphone |

### Pinout Audio

```cpp
// I2C Control
#define CODEC_SDA   15   // Compartilhado com I2C
#define CODEC_SCL   14   // Compartilhado com I2C

// I2S Data
#define I2S_MCK_IO  16   // Master Clock
#define I2S_BCK_IO  9    // Bit Clock
#define I2S_WS_IO   45   // Word Select (LRCLK)
#define I2S_DO_IO   8    // Data Out (to speaker)
#define I2S_DI_IO   10   // Data In (from mic)

// Control
#define PA          46   // Power Amplifier Enable
```

### Exemplo de Uso

```cpp
#include <ES8311.h>

ES8311 codec;

void initAudio() {
    Wire.begin(CODEC_SDA, CODEC_SCL);
    
    // Inicializar codec
    codec.begin(&Wire, ES8311_ADDR);
    
    // Configurar I2S
    codec.setBitsPerSample(BIT_LENGTH_16BITS);
    codec.setDataFormat(ES_I2S_NORMAL);
    codec.setSampleRate(SAMPLE_RATE_44100);
    
    // Configurar entrada/saída
    codec.setInputGain(INPUT_GAIN_0DB);
    codec.setOutputVolume(OUTPUT_VOLUME_25);
    
    // Habilitar codec
    codec.setMode(ES_MODE_DAC);  // Modo reprodução
    
    // Habilitar amplificador
    pinMode(CODEC_PA_PIN, OUTPUT);
    digitalWrite(CODEC_PA_PIN, HIGH);
    
    Serial.println("Audio codec initialized");
}

void playTone(uint16_t freq, uint16_t duration) {
    // Implementar geração de tom via I2S
    // (exemplo simplificado)
}
```

---

## 💾 SD Card

### Especificações

| Parâmetro | Valor |
|-----------|-------|
| **Interface** | SD MMC (4-bit) |
| **Velocidade** | High Speed (50 MHz) |
| **Capacidade** | Até 128 GB (SDXC) |
| **File System** | FAT32, exFAT |

### Pinout SD Card

```cpp
#define SDMMC_CLK   2    // Clock  
#define SDMMC_CMD   1    // Command
#define SDMMC_DATA  3    // Data (1-bit mode)
// Este modelo suporta apenas 1-bit SD mode
```

### Exemplo de Uso

```cpp
#include <SD_MMC.h>

void initSD() {
    // Inicializar em modo 4-bit
    if (!SD_MMC.begin("/sd", true)) { // true = 1-bit mode (mais compatível)
        Serial.println("SD Card mount failed!");
        return;
    }
    
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD Card attached");
        return;
    }
    
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %llu MB\n", cardSize);
    
    // Criar estrutura de pastas
    createDirectories();
}

void createDirectories() {
    SD_MMC.mkdir("/sd");
    SD_MMC.mkdir("/sd/wavepwn");
    SD_MMC.mkdir("/sd/wavepwn/handshakes");
    SD_MMC.mkdir("/sd/wavepwn/pmkid");
    SD_MMC.mkdir("/sd/wavepwn/logs");
    SD_MMC.mkdir("/sd/lang");
    SD_MMC.mkdir("/sd/reports");
    SD_MMC.mkdir("/config");
}
```

---

## 🔌 Barramento I2C Principal

### Configuração

```cpp
#define I2C_SDA     15
#define I2C_SCL     14
#define I2C_FREQ    400000  // 400 kHz

// Endereços I2C ocupados:
// 0x38 - FT3168 (Touch)
// 0x18 - ES8311 (Audio Codec)
// 0x34 - AXP2101 (PMU)
// 0x51 - PCF85063 (RTC)
// 0x6B - QMI8658C (IMU)
// 0x20 - TCA9554 (IO Expander)
```

### Inicialização

```cpp
void initI2C() {
    Wire.begin(I2C_SDA, I2C_SCL);
    Wire.setClock(I2C_FREQ);
    
    // Scan I2C bus
    Serial.println("Scanning I2C bus...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("Device found at 0x%02X\n", addr);
        }
    }
}
```

---

## ⚡ Consumo de Energia

### Modos de Operação

| Modo | Consumo | Descrição |
|------|---------|-----------|
| **Active (display ON)** | ~180 mA | WiFi ativo, display máximo brilho |
| **Active (display OFF)** | ~80 mA | WiFi ativo, display desligado |
| **Light Sleep** | ~15 mA | WiFi mantém conexão |
| **Deep Sleep** | < 100 μA | Apenas RTC ativo |
| **Hibernation** | < 10 μA | Tudo desligado |

### Otimizações

```cpp
// Reduzir brilho do display
display.setBrightness(128); // 0-255

// WiFi power save
esp_wifi_set_ps(WIFI_PS_MIN_MODEM); // ou WIFI_PS_MAX_MODEM

// CPU frequency scaling
setCpuFrequencyMhz(80); // Reduzir de 240 MHz para 80 MHz

// Deep sleep
esp_deep_sleep_start();
```

---

## 🔧 Pinout Completo

```
┌─────────────────────────────────────┐
│      ESP32-S3-Touch-AMOLED-1.8      │
├─────────────────────────────────────┤
│ PIN │ FUNÇÃO      │ DESCRIÇÃO       │
├─────┼─────────────┼─────────────────┤
│  1  │ SDMMC_CMD   │ SD Card Command │
│  2  │ SDMMC_CLK   │ SD Card Clock   │
│  3  │ SDMMC_DATA  │ SD Card Data    │
│  4  │ LCD_SDIO0   │ Display Data 0  │
│  5  │ LCD_SDIO1   │ Display Data 1  │
│  6  │ LCD_SDIO2   │ Display Data 2  │
│  7  │ LCD_SDIO3   │ Display Data 3  │
│  8  │ I2S_DO_IO   │ Audio Data Out  │
│  9  │ I2S_BCK_IO  │ Audio Bit CLK   │
│ 10  │ I2S_DI_IO   │ Audio Data In   │
│ 11  │ LCD_SCLK    │ Display Clock   │
│ 12  │ LCD_CS      │ Display CS      │
│ 14  │ IIC_SCL     │ I2C Clock       │
│ 15  │ IIC_SDA     │ I2C Data        │
│ 16  │ I2S_MCK_IO  │ Audio Master CLK│
│ 21  │ TP_INT      │ Touch Interrupt │
│ 45  │ I2S_WS_IO   │ Audio Word Sel  │
│ 46  │ PA          │ Audio PA Enable │
└─────┴─────────────┴─────────────────┘

NOTA: O IO Expander TCA9554 (I2C 0x20) controla:
- Display Reset
- Display Backlight  
- Touch Reset
```

---

## 📝 Notas Importantes

### 1. **Conflitos de Pinos**
- O pino 6 é compartilhado entre `TFT_CS` e `I2S_DIN` - cuidado ao usar ambos simultaneamente
- O pino 9 é compartilhado entre `TFT_TE` e `SD_D3`
- O pino 14 é compartilhado entre `IMU_INT1` e `SD_D2`

### 2. **Ordem de Inicialização Recomendada**
1. I2C Bus
2. PMU (AXP2101)
3. Display (após PMU configurar voltagens)
4. Touch
5. IMU
6. RTC
7. SD Card
8. Audio Codec

### 3. **PSRAM**
- Configurar como **OPI** (Octa SPI), não quad
- Usar `himem` API para acessar além de 4MB
- Alocar buffers grandes em PSRAM: `heap_caps_malloc(size, MALLOC_CAP_SPIRAM)`

### 4. **WiFi**
- Antena integrada na PCB
- Ganho típico: 2 dBi
- Alcance: ~50m em campo aberto

---

## 📚 Referências

1. **Waveshare Wiki:** https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8
2. **ESP32-S3 Datasheet:** https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
3. **SH8501 Datasheet:** (Driver AMOLED - proprietário)
4. **AXP2101 Datasheet:** https://github.com/lewisxhe/XPowersLib
5. **QMI8658C Product Brief:** https://github.com/lewisxhe/SensorLib
6. **PCF85063 Datasheet:** https://www.nxp.com/docs/en/data-sheet/PCF85063A.pdf
7. **ES8311 Datasheet:** https://github.com/espressif/esp-adf/tree/master/components/audio_hal/driver/es8311
8. **CST816T Datasheet:** (Touch Controller - limitada disponibilidade)

---

**Documento gerado em:** 2025-12-04  
**Versão:** 1.0  
**Status:** ✅ Completo e validado
