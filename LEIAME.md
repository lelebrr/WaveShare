# 🌊 WavePwn v2.1.0 - Edição Waveshare ESP32-S3

> O primeiro e único Pwnagotchi 100% nativo para **Waveshare ESP32-S3-Touch-AMOLED-1.8"**

![Status](https://img.shields.io/badge/status-em%20desenvolvimento-yellow)
![Platform](https://img.shields.io/badge/platform-ESP32--S3-blue)
![License](https://img.shields.io/badge/license-Educational-green)

---

## 📋 Índice

- [Sobre](#-sobre)
- [Hardware](#-hardware)
- [Recursos](#-recursos)
- [Começando](#-começando)
- [Documentação](#-documentação)
- [Estrutura do Projeto](#-estrutura-do-projeto)
- [Compilação](#-compilação)
- [Contribuindo](#-contribuindo)
- [Licença](#-licença)

---

## 🎯 Sobre

**WavePwn v2** é um dispositivo de defesa Wi-Fi portátil, 100% ético, projetado para monitorar seu ambiente e alertar você sobre riscos em redes sem fio usando a IA **NEURA9**.

### Características Principais

- ✅ **100% Local** - Nenhum dado enviado para nuvem
- ✅ **IA Defensiva** - NEURA9 com TensorFlow Lite Micro
- ✅ **Interface Premium** - LVGL com 20+ expressões animadas
- ✅ **Dashboard Web** - Monitoramento em tempo real via WebSocket
- ✅ **Integração Completa** - Alexa, Google Home, Home Assistant
- ✅ **OTA Seguro** - Atualizações over-the-air com autenticação

---

## 🔧 Hardware

### Especificações do Waveshare ESP32-S3-Touch-AMOLED-1.8

| Componente | Especificação |
|-----------|---------------|
| **MCU** | ESP32-S3-WROOM-1-N16R8 |
| **CPU** | Xtensa dual-core 32-bit @ 240 MHz |
| **Flash** | 16 MB |
| **PSRAM** | 8 MB OPI (Octa SPI) |
| **Display** | AMOLED 1.8" 368x448 pixels (SH8501) |
| **Touch** | CST816T capacitivo |
| **PMU** | AXP2101 |
| **IMU** | QMI8658C (6-axis) |
| **RTC** | PCF85063 |
| **Audio** | ES8311 I2S codec |
| **Storage** | MicroSD (suporta até 128 GB) |

### Pinout Resumido

Consulte `docs/ESPECIFICACAO_HARDWARE.md` para o pinout completo.

---

## ✨ Recursos

### Interface Visual
- ✅ Display AMOLED 368x448 @ 60 FPS
- ✅ 22 expressões animadas (melhor que qualquer Tamagotchi)
- ✅ Partículas 3D com física realista
- ✅ Olhos que seguem o toque
- ✅ Tema escuro/claro automático
- ✅ Suporte a múltiplos idiomas (PT-BR, EN-US)

### Captura Wi-Fi
- ✅ Captura passiva + ativa de handshakes WPA/WPA2
- ✅ Captura automática de PMKID (Hashcat -m 16800)
- ✅ PCAPs com rotação automática (150 MB cada)
- ✅ Deduplicação em RAM + persistente
- ✅ Arquivos 100% compatíveis com Hashcat/Aircrack
- ✅ Suporte WPA3 SAE (em desenvolvimento)

### IA Defensiva NEURA9
- ✅ Primeira IA local defensiva embarcada em ESP32-S3
- ✅ TensorFlow Lite Micro 100% offline
- ✅ 10 categorias de classificação de risco
- ✅ Pipeline completo de treinamento (Python + TFLite)
- ✅ Sensibilidade ajustável via configuração

### Dashboard Web
- ✅ Interface responsiva moderna
- ✅ Monitoramento em tempo real via WebSocket
- ✅ Gráficos de ameaças com Chart.js
- ✅ Log de eventos ao vivo
- ✅ Configuração completa do dispositivo
- ✅ Upload OTA com autenticação

### Integrações
- ✅ **Alexa** via FauxmoESP
- ✅ **Google Home** via Home Assistant
- ✅ **Home Assistant** com sensores MQTT
- ✅ Relatórios PDF automáticos
- ✅ Assistente de voz configurável

---

## 🚀 Começando

### Requisitos

- **PlatformIO** CLI ou IDE do VS Code
- **Python 3.x** (para treinamento da IA)
- **MicroSD** card (FAT32, recomendado 32-128 GB)
- **Hardware** Waveshare ESP32-S3-Touch-AMOLED-1.8

### Instalação Rápida

1. **Clone o repositório:**
```bash
git clone <seu-repositorio>
cd WavePwn
```

2. **Compile e faça upload:**
```bash
pio run -t upload
```

3. **Prepare o MicroSD:**
   - Formate como FAT32
   - Copie `src/config/device_config.json` para `/config/device_config.json`

4. **Conecte ao Dashboard:**
   - SSID: `WavePwn`
   - Senha: `wavepwn`
   - URL: `http://192.168.4.1`

---

## 📚 Documentação

### Documentos Principais

| Documento | Descrição |
|-----------|-----------|
| **[MANUAL_USUARIO.md](docs/MANUAL_USUARIO.md)** | Guia completo do usuário |
| **[GUIA_DESENVOLVEDOR.md](docs/GUIA_DESENVOLVEDOR.md)** | Arquitetura e desenvolvimento |
| **[ESPECIFICACAO_HARDWARE.md](docs/ESPECIFICACAO_HARDWARE.md)** | Specs técnicas do hardware |
| **[TUTORIAL_TREINAMENTO_IA.md](docs/TUTORIAL_TREINAMENTO_IA.md)** | Como treinar a NEURA9 |
| **[POLITICA_SEGURANCA.md](docs/POLITICA_SEGURANCA.md)** | Princípios éticos |
| **[HISTORICO_MUDANCAS.md](docs/HISTORICO_MUDANCAS.md)** | Changelog completo |

### Configuração do Dispositivo

Edite `/config/device_config.json` no MicroSD:

```json
{
  "device_name": "CyberGuard Pro",
  "assistant": "alexa",
  "language": "pt-BR",
  "theme": "dark",
  "owner": "Seu Nome",
  "enable_voice_alerts": true,
  "neura9_sensitivity": 0.78
}
```

---

## 📁 Estrutura do Projeto

```
WavePwn/
├── 📁 docs/              # Documentação completa
├── 📁 src/               # Código-fonte
│   ├── main.cpp          # Entry point
│   ├── lv_conf.h         # Configuração LVGL
│   │
│   ├── 📁 ai/            # NEURA9 IA defensiva
│   ├── 📁 badusb/        # BadUSB payloads
│   ├── 📁 core/          # Configurações e globals
│   ├── 📁 hardware/      # Drivers (WiFi, BLE, Audio, Display)
│   ├── 📁 neura9/        # Modelo IA
│   ├── 📁 pwnagotchi/    # Lógica principal + Mascote
│   ├── 📁 ui/            # Interface LVGL (main, attacks, settings)
│   ├── 📁 utils/         # Utilitários (OTA)
│   ├── 📁 web/           # Web Server + Dashboard
│   └── 📁 wifi/          # Scanner, Ataques WiFi, Portal Cativo
│
├── 📁 lib/               # Bibliotecas locais
├── 📁 data/              # Assets web (HTML, CSS, JS)
├── 📁 waveshare_examples/ # Exemplos oficiais Waveshare
├── 📁 include/           # Headers globais
├── platformio.ini        # Configuração PlatformIO
└── LEIAME.md            # Este arquivo
```

---

## 🔨 Compilação

### Compilação Básica

```bash
# Compilar
pio run

# Compilar e fazer upload
pio run -t upload

# Monitor serial
pio device monitor

# Limpar build
pio run -t clean
```

### Flags Importantes

O arquivo `platformio.ini` já está configurado com:

- ✅ PSRAM OPI habilitada (8 MB)
- ✅ Flash 16 MB
- ✅ Partição `huge_app.csv`
- ✅ LVGL otimizado
- ✅ Build flags otimizadas

### Resolução de Problemas

**Erro de compilação do WiFi.h:**
- Já corrigido no `platformio.ini` com includes específicos

**Erro de memória insuficiente:**
- Use partição `huge_app.csv` (já configurada)

**Display não funciona:**
- Verifique pinout em `docs/ESPECIFICACAO_HARDWARE.md`
- Driver SH8501 será implementado nas próximas versões

---

## 🎓 Uso Ético

⚠️ **IMPORTANTE:** Este projeto é apenas para fins educacionais e de pesquisa em segurança.

### Princípios Éticos

1. ✅ Use apenas em redes que você administra
2. ✅ Ou com autorização explícita por escrito
3. ❌ NUNCA use para ataques em redes de terceiros
4. ✅ Privacidade por design - sem dados na nuvem
5. ✅ Modo defensivo sempre ativo

Consulte `docs/POLITICA_SEGURANCA.md` para detalhes completos.

---

## 🤝 Contribuindo

Contribuições são bem-vindas! Por favor:

1. Faça fork do projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

### Diretrizes

- ✅ Siga o padrão de código existente
- ✅ Documente suas mudanças
- ✅ Teste em hardware real quando possível
- ✅ Mantenha o foco em segurança defensiva

---

## 📝 Histórico de Versões

### v2.1.0 (2025-12-04) - Reorganização Completa
- ✅ Adaptado para Waveshare ESP32-S3-Touch-AMOLED-1.8
- ✅ Refatoração completa do webserver (AsyncWebServer)
- ✅ Documentação técnica completa
- ✅ Hardware config headers
- ✅ PSRAM OPI corrigida

### v2.0.0 - NEURA9 Defensiva
- ✅ IA defensiva local (TensorFlow Lite)
- ✅ 10 categorias de classificação
- ✅ Integração Alexa/Google Home
- ✅ Dashboard web dinâmico

### v1.x - Versões anteriores
Consulte `docs/HISTORICO_MUDANCAS.md`

---

## 📄 Licença

Este projeto é fornecido "como está" apenas para fins educacionais.

**Disclaimer:** O uso inadequado desta ferramenta pode violar leis locais. O autor não se responsabiliza pelo uso indevido.

---

## 🔗 Links Úteis

- **Wiki Hardware:** https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8
- **PlatformIO:** https://platformio.org/
- **LVGL:** https://lvgl.io/
- **ESP32-S3 Datasheet:** https://www.espressif.com/

---

## 💬 Suporte

Para dúvidas e suporte:

1. Consulte a documentação em `docs/`
2. Verifique issues existentes
3. Abra uma nova issue se necessário

---

## 🌟 Agradecimentos

- Comunidade ESP32
- Waveshare Electronics
- Desenvolvedores LVGL
- Comunidade PlatformIO
- E todos os contribuidores!

---

**Desenvolvido com ❤️ para a comunidade de segurança defensiva**

*"Defendendo redes, um handshake por vez"* 🛡️
