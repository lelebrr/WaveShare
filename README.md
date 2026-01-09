# WaveShare Dragon Watch - README

<p align="center">
  <img src="docs/dragon_logo.png" width="200" alt="Dragon Logo">
</p>

## 🐉 O que é?

Um smartwatch de segurança ofensiva baseado em **ESP32-S3** com tela AMOLED touch de 1.8".

### Recursos Principais
- 📶 **WiFi Attacks**: Deauth, Beacon Flood, Handshake Capture, Evil Twin
- 🔵 **BLE Chaos**: Apple/Samsung/Google Spam, AirTag Flood, RickRoll
- ☢️ **IR Blaster**: TV NUKE, Clonador de controles, 500+ códigos
- 🧠 **IA Local**: Detecção de ameaças com TensorFlow Lite
- 🎤 **Assistente de Voz**: Wake word "Hey Dragon", TTS offline
- 🌐 **Web Dashboard**: Interface completa via navegador

---

## ⚡ Quick Start

### 1. Instalar Dependências
```bash
# No VS Code, instalar extensão PlatformIO
# Ou via terminal:
pip install platformio
```

### 2. Configurar API Key (Opcional - para voz online)
Edite `src/core/config.h`:
```cpp
#define OPENAI_API_KEY "sk-sua-chave-aqui"
```

### 3. Compilar e Gravar
```bash
cd c:\Projetos\WaveShare
pio run -t upload
```

### 4. Acessar Web Dashboard
```
http://wavepwn.local
Usuário: admin
Senha: pwned
```

---

## 📱 Interface do Dispositivo

### Tela Home
- Avatar do Dragão com humor
- Status: Bateria, WiFi, Redes, Uptime
- Botão "APPS" para abrir o launcher

### Apps Disponíveis
| App | Função |
|-----|--------|
| **WiFi** | Scan, Deauth, Beacon, Handshake |
| **BLE** | Spam attacks, Scanner |
| **IR** | TV Nuke, Clonador |
| **AI** | Detecção de ameaças |
| **Tools** | Arquivos, Stats |
| **Voice** | Assistente de voz |
| **Settings** | Configurações |
| **Eco** | Modo economia extrema |

### Modo Eco (Relógio)
- Mostra apenas horário
- CPU: 80MHz, radios desligados
- **Para sair**: Segure a tela por 10 segundos

---

## 🛠️ Configuração

### Hardware Suportado
- ESP32-S3 com PSRAM
- Display QSPI 368x448
- Touch FT3168
- PMU AXP2101
- RTC PCF85063
- IMU QMI8658
- Audio ES8311

### Arquivos de Configuração
| Arquivo | Descrição |
|---------|-----------|
| `src/core/config.h` | Chaves API, timeouts |
| `src/core/pin_definitions.h` | Pinos GPIO |
| `platformio.ini` | Build config |

---

## 🎤 Sistema de Voz

### Gerar Áudios TTS
```bash
pip install gtts pydub
python scripts/generate_tts_samples.py
```

### Treinar Wake Word
Siga o guia: `docs/EDGE_IMPULSE_WAKE_WORD_GUIDE.md`

---

## 📄 Licença

Uso educacional. Não use para atividades ilegais.

---

## 🤝 Créditos

- LVGL para interface gráfica
- IRremoteESP8266 para IR
- TensorFlow Lite Micro para IA
- OpenAI Whisper para STT
