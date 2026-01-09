# ⚠️ AVISO IMPORTANTE - CORREÇÕES PENDENTES

**Data:** 2025-12-05  
**Status:** ⚠️ **ERROS DE COMPILAÇÃO - AÇÃO NECESSÁRIA**

---

## ❌ PROBLEMA

Tentei corrigir as configurações de hardware para as especificações oficiais da Waveshare, MAS os arquivos ficaram corrompidos durante as edições.

---

## ✅ SOLUÇÃO TEMPORÁRIA

**REVERTER para os arquivos originais que compilavam:**

1. Usar `git restore` ou `Ctrl+Z` para voltar aos arquivos que compilavam
2. Compilar com sucesso primeiro
3. Depois fazer as correções de hardware manualmente

---

## 🔧 CORREÇÕES QUE PRECISAM SER FEITAS (MANUALMENTE)

### 1. Platform IO - Adicionar bibliotecas
```ini
lib_deps = 
    ...
    lovyan03/LovyanGFX@^1.1.0          # ← ADICIONAR
    lewisxhe/XPowersLib@^0.2.0         # ← ADICIONAR
```

### 2. Display Driver  
**PROBLEMA:** LovyanGFX não tem `Panel_SH8601`

**SOLUÇÃO:** Usar painel genérico:
```cpp
lgfx::Panel_Device _panel_instance;  // Em vez de Panel_SH8601
```

**OU:** Esperar suporte do SH8601 em versão futura da LovyanGFX

### 3. Pinos do Display (Waveshare oficial)
```cpp
// QSPI Data pins
cfg.pin_d0 = 4;   // LCD_SDIO0  
cfg.pin_d1 = 5;   // LCD_SDIO1
cfg.pin_d2 = 6;   // LCD_SDIO2
cfg.pin_d3 = 7;   // LCD_SDIO3

// Control pins
cfg.pin_wr = 11;  // LCD_SCLK
cfg.pin_rs = 10;  // LCD_DC
cfg.pin_cs = 12;  // LCD_CS
cfg.pin_rst = 13; // LCD_RST
cfg.pin_bl = 1;   // LCD_BL
```

### 4. Touch FT3168
```cpp
cfg.i2c_addr = 0x38;  // Não 0x15!
cfg.pin_sda = 8;      // Não 38!
cfg.pin_scl = 9;      // Não 39!
cfg.pin_int = 14;
cfg.pin_rst = 21;
```

---

## 📋 CHECKLIST DE CORREÇÃO

- [ ] Reverter arquivos corrompidos
- [ ] Adicionar `LovyanGFX` ao platformio.ini
- [ ] Adicionar `XPowersLib` ao platformio.ini
- [ ] Corrigir pinos do display
- [ ] Corrigir touch I2C (addr 0x38)
- [ ] Testar compilação
- [ ] Testar no hardware real

---

## 🎯 ARQUIVOS QUE PRECISAM SER CORRIGIDOS

1. `platformio.ini` - Adicionar bibliotecas
2. `src/hardware/display_driver.cpp` - Pinos corretos
3. `src/hardware/rtc_driver.cpp` - Se corrompido, reescrever
4. `src/ble_grid/pwn_grid.h` - Adicionar includes BLE

---

## 📚 REFERÊNCIA

**Documentação oficial:**
https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8

**Pinout validado:**
- Ver `.agent/CORRECOES_HARDWARE_WAVESHARE.md`

---

**RECOMENDAÇÃO:** Compilar PRIMEIRO com código que funciona, DEPOIS fazer correções de hardware incrementalmente.

