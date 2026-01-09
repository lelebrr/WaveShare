<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Tipo-FAQ-blue?style=for-the-badge" alt="FAQ"/>
  <img src="https://img.shields.io/badge/Perguntas-20+-green?style=for-the-badge" alt="Questions"/>
  <img src="https://img.shields.io/badge/Idioma-Português-yellow?style=for-the-badge" alt="Language"/>
</p>

---

# ❓ FAQ - Perguntas Frequentes

## 📋 Índice

1. [Geral](#-geral)
2. [Hardware](#-hardware)
3. [Compilação](#-compilação)
4. [Ataques WiFi](#-ataques-wifi)
5. [NEURA9 AI](#-neura9-ai)
6. [Interface](#-interface)
7. [Problemas Comuns](#-problemas-comuns)

---

## 🎯 Geral

### O que é o WavePwn?
WavePwn é um framework de segurança wireless ofensiva para o hardware Waveshare ESP32-S3-Touch-AMOLED-1.8, inspirado no Pwnagotchi. Combina ataques WiFi/BLE com IA embarcada para detecção de ameaças.

### É legal usar o WavePwn?
⚠️ **Apenas em redes que você possui ou tem autorização explícita por escrito.** Uso não autorizado é ilegal e pode resultar em penalidades criminais.

### Preciso de conhecimento técnico?
Conhecimento básico de Linux/PlatformIO é recomendado. A documentação é extensa para ajudar iniciantes.

---

## 🔧 Hardware

### Qual hardware é suportado?
Apenas o **Waveshare ESP32-S3-Touch-AMOLED-1.8**:
- ESP32-S3 N16R8 (16MB Flash, 8MB PSRAM)
- Display AMOLED 1.8" 368x448
- Touch FT3168

### Posso usar outro ESP32-S3?
O código é específico para este hardware. Adaptar para outras placas requer modificações significativas nos drivers de display e touch.

### Preciso de cartão SD?
Recomendado mas não obrigatório. SD card é usado para:
- Salvar capturas (handshakes, PMKID)
- Wallpapers customizados
- Sons customizados
- Logs

### Qual tamanho de SD usar?
4GB a 128GB, formatado em **FAT32**.

---

## 🔨 Compilação

### Erro: "DRAM overflow"
**Solução:** O tensor_arena foi movido para PSRAM na v2.1.0. Atualize para a versão mais recente.

### Erro: "WiFiClientSecure.h not found"
**Solução:** Adicione estas libs no `platformio.ini`:
```ini
lib_deps = 
    WiFiClientSecure
```

### Erro: "configAccelerometer - too many arguments"
**Solução:** Use 3 argumentos ao invés de 4. Veja `CORRECOES_HARDWARE.md`.

### Compilação demora muito
Normal. A compilação completa leva ~2-3 minutos na primeira vez. Builds incrementais são mais rápidos.

---

## 📡 Ataques WiFi

### Por que o deauth não funciona?
1. Verifique se está no canal correto
2. Alguns APs modernos têm proteção contra deauth
3. Distância muito grande do alvo

### Handshake não é capturado
1. Cliente deve se reconectar durante o ataque
2. Use deauth para forçar reconexão
3. Verifique se o AP usa WPA/WPA2

### Beacon flood trava o dispositivo?
Reduza o número de SSIDs fake ou aumente o intervalo entre beacons.

### Evil Twin não funciona
1. Verifique se o SSID está correto
2. O alvo deve se conectar manualmente
3. Captive portal requer DNS spoofing ativo

---

## 🧠 NEURA9 AI

### O que a NEURA9 detecta?
10 categorias:
- SAFE, CROWDED, OPEN_NETWORK
- EVIL_TWIN_RISK, DEAUTH_DETECTED, ROGUE_AP
- HIGH_RISK, BATTERY_CRITICAL
- GESTURE_COMMAND, LEARNING_MODE

### Como treinar meu próprio modelo?
Veja `docs/TUTORIAL_TREINAMENTO_NEURA9.md`. Resumo:
1. Colete dados com o script
2. Prepare o dataset
3. Treine com TensorFlow
4. Converta para TFLite
5. Gere array C++

### Confiança sempre baixa?
O modelo stub atual precisa ser substituído por um modelo treinado. Execute os scripts de treinamento.

---

## 🎨 Interface

### Display não liga
1. Verifique IO Expander TCA9554 (I2C 0x20)
2. Confirme driver SH8601 (não SH8501)
3. Veja `GUIA_VALIDACAO_HARDWARE.md`

### Touch não responde
1. Touch é FT3168 @ 0x38 (não CST816T)
2. Verifique I2C SDA=15, SCL=14
3. Execute scan I2C

### Como mudar tema de boot?
Acesse Configurações → Sistema → Tema de Boot. 10 temas disponíveis.

### Como personalizar mascote?
Acesse Configurações → Dragão → Personalidade. Opções incluem nome, velocidade, expressões.

---

## 🔧 Problemas Comuns

### "AsyncTCP failed to start task"
**Causa:** Memória insuficiente  
**Solução:** 
- Atualize PSRAM para v2.1.0+
- Reduza buffers LVGL

### "Wire.cpp:513 requestFrom -1"
**Causa:** Erro de comunicação I2C  
**Solução:**
- Verifique pinos I2C (14/15)
- Reduza velocidade para 100kHz
- Verifique conexões físicas

### Web interface não abre
1. Conecte ao WiFi "WavePwn" (senha: wavepwn)
2. Acesse http://192.168.4.1
3. Se não funcionar, verifique logs serial

### Bateria não carrega
1. Verifique PMU AXP2101 @ 0x34
2. Veja configuração de corrente de carga
3. Bateria pode estar com defeito

### Device reinicia sozinho
**Causas comuns:**
- Watchdog timeout (aumentar WDT)
- Memória insuficiente
- Erro em ataque pesado

---

## 📞 Ainda com dúvidas?

1. 📖 Leia a documentação completa em `/docs`
2. 🔍 Procure issues existentes no GitHub
3. 💬 Abra uma nova issue com detalhes

---

**Última Atualização:** 2025-12-08  
**Versão:** 1.0
