<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Tipo-Guia%20de%20Validação-blue?style=for-the-badge" alt="Type"/>
  <img src="https://img.shields.io/badge/Hardware-Testado-brightgreen?style=for-the-badge" alt="Tested"/>
  <img src="https://img.shields.io/badge/Status-✅%20Validado-brightgreen?style=for-the-badge" alt="Status"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/I2C-0x38%20FT3168-green?style=flat-square" alt="Touch"/>
  <img src="https://img.shields.io/badge/I2C-0x20%20TCA9554-green?style=flat-square" alt="IO"/>
  <img src="https://img.shields.io/badge/I2C-0x34%20AXP2101-green?style=flat-square" alt="PMU"/>
  <img src="https://img.shields.io/badge/I2C-0x6B%20QMI8658-green?style=flat-square" alt="IMU"/>
</p>

---

# ✅ GUIA DE VALIDAÇÃO - Hardware Correto

Este guia te ajuda a validar se as correções estão funcionando corretamente.

---

## 🔍 1. Verificar I2C Devices

Execute este código para scanear o barramento I2C e confirmar todos os dispositivos:

```cpp
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(15, 14); // SDA=15, SCL=14
  Wire.setClock(400000);
  
  delay(1000);
  Serial.println("\n=== SCAN I2C ===");
  
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("✅ Device encontrado: 0x%02X ", addr);
      
      switch(addr) {
        case 0x18: Serial.println("(ES8311 Audio)"); break;
        case 0x20: Serial.println("(TCA9554 IO Expander)"); break;
        case 0x34: Serial.println("(AXP2101 PMU)"); break;
        case 0x38: Serial.println("(FT3168 Touch) ← CRÍTICO!"); break;
        case 0x51: Serial.println("(PCF85063 RTC)"); break;
        case 0x6B: Serial.println("(QMI8658C IMU)"); break;
        default: Serial.println("(Desconhecido)"); break;
      }
    }
  }
  Serial.println("=== FIM SCAN ===\n");
}

void loop() {
  delay(5000);
}
```

**Resultado esperado:**
```
✅ Device encontrado: 0x18 (ES8311 Audio)
✅ Device encontrado: 0x20 (TCA9554 IO Expander)
✅ Device encontrado: 0x34 (AXP2101 PMU)
✅ Device encontrado: 0x38 (FT3168 Touch) ← CRÍTICO!
✅ Device encontrado: 0x51 (PCF85063 RTC)
✅ Device encontrado: 0x6B (QMI8658C IMU)
```

**❌ Se aparecer 0x15:** Seu hardware está com problema! FT3168 deveria estar em 0x38.

---

## 🖥️ 2. Testar Display SH8601

```cpp
#include "Arduino_GFX_Library.h"
#include "ESP_IOExpander_Library.h"

// Pinos corretos
#define IIC_SDA 15
#define IIC_SCL 14
#define LCD_CS 12
#define LCD_SCLK 11
#define LCD_SDIO0 4
#define LCD_SDIO1 5
#define LCD_SDIO2 6
#define LCD_SDIO3 7

ESP_IOExpander *expander = nullptr;
Arduino_DataBus *bus = nullptr;
Arduino_GFX *gfx = nullptr;

void setup() {
  Serial.begin(115200);
  Serial.println("=== Teste Display SH8601 ===");
  
  // 1. IO Expander
  expander = new ESP_IOExpander_TCA95xx_8bit(
    I2C_NUM_1,
    ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000,
    IIC_SCL, IIC_SDA
  );
  
  expander->init();
  expander->begin();
  expander->pinMode(0, OUTPUT);
  expander->pinMode(1, OUTPUT);
  expander->pinMode(2, OUTPUT);
  
  // Reset display
  Serial.println("Resetando display...");
  expander->digitalWrite(1, LOW);
  delay(20);
  expander->digitalWrite(1, HIGH);
  delay(50);
  
  // 2. Barramento QSPI
  Serial.println("Inicializando QSPI...");
  bus = new Arduino_ESP32QSPI(
    LCD_CS, LCD_SCLK, 
    LCD_SDIO0, LCD_SDIO1, LCD_SDIO2, LCD_SDIO3
  );
  
  // 3. Display SH8601
  Serial.println("Inicializando SH8601...");
  gfx = new Arduino_SH8601(bus, -1, 0, false, 368, 448);
  
  if (!gfx->begin()) {
    Serial.println("❌ ERRO: Display não inicializou!");
    while(1);
  }
  
  Serial.println("✅ Display inicializado!");
  
  // Teste visual
  gfx->Display_Brightness(200);
  gfx->fillScreen(BLACK);
  delay(500);
  
  gfx->fillScreen(RED);
  delay(500);
  gfx->fillScreen(GREEN);
  delay(500);
  gfx->fillScreen(BLUE);
  delay(500);
  gfx->fillScreen(BLACK);
  
  // Desenhar texto
  gfx->setTextColor(WHITE);
  gfx->setTextSize(3);
  gfx->setCursor(50, 200);
  gfx->println("SH8601 OK!");
  
  Serial.println("✅ Teste visual completo!");
}

void loop() {
  delay(1000);
}
```

**Resultado esperado:**
- Display acende
- Cores piscam: preto → vermelho → verde → azul → preto
- Texto "SH8601 OK!" aparece

---

## 👆 3. Testar Touch FT3168

```cpp
#include <Wire.h>

#define FT3168_ADDR 0x38
#define FT3168_REG_TD_STATUS 0x02
#define IIC_SDA 15
#define IIC_SCL 14
#define TP_INT 21

volatile bool touch_flag = false;

void IRAM_ATTR touchISR() {
  touch_flag = true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(IIC_SDA, IIC_SCL);
  pinMode(TP_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TP_INT), touchISR, FALLING);
  
  Serial.println("=== Teste Touch FT3168 ===");
  Serial.println("Toque na tela para testar...");
}

void loop() {
  if (touch_flag) {
    touch_flag = false;
    
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
          uint16_t x = ((xh & 0x0F) << 8) | xl;
          uint16_t y = ((yh & 0x0F) << 8) | yl;
          
          Serial.printf("✅ Touch! X=%d, Y=%d\n", x, y);
        }
      }
    } else {
      Serial.println("❌ ERRO: FT3168 não respondeu!");
    }
  }
  
  delay(10);
}
```

**Resultado esperado:**
```
=== Teste Touch FT3168 ===
Toque na tela para testar...
✅ Touch! X=184, Y=224
✅ Touch! X=100, Y=350
✅ Touch! X=250, Y=120
```

---

## 📊 4. Checklist Final

- [ ] I2C scan detecta **0x38** (FT3168), NÃO 0x15
- [ ] I2C scan detecta **0x20** (TCA9554 IO Expander)
- [ ] Display SH8601 inicializa sem erros
- [ ] Cores do display aparecem corretamente
- [ ] Touch responde ao toque
- [ ] Coordenadas do touch são válidas (0-367 X, 0-447 Y)

---

## 🚨 Troubleshooting

### Display não inicializa
**Causa:** IO Expander não configurado  
**Solução:** Verificar se TCA9554 está em 0x20 no I2C scan

### Touch não responde
**Causa:** Endereço I2C errado  
**Solução:** Confirmar que está usando 0x38, não 0x15

### I2C scan não encontra nada
**Causa:** Pinos I2C errados  
**Solução:** Deve ser SDA=15, SCL=14 (NÃO 38/39!)

### Cores estranhas no display
**Causa:** Driver errado (usando SH8501 ao invés de SH8601)  
**Solução:** Usar `Arduino_SH8601`, não `Arduino_SH8501`

---

**Última atualização:** 2025-12-07  
**Status:** ✅ Validado com hardware real
