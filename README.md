<p align="center">
  <img src="https://raw.githubusercontent.com/your-username/wavepwn/main/assets/logo.png" alt="WavePwn Logo" width="400"/>
</p>

<h1 align="center">🌊 WavePwn v2.1.0</h1>

<p align="center">
  <strong>🐉 The Ultimate Pwnagotchi for Waveshare ESP32-S3-Touch-AMOLED-1.8</strong>
</p>

<p align="center">
  <em>Offensive Wireless Framework with AI-Powered Threat Detection</em>
</p>

<p align="center">
  <a href="#-features">Features</a> •
  <a href="#-hardware">Hardware</a> •
  <a href="#-installation">Installation</a> •
  <a href="#-documentation">Docs</a> •
  <a href="#-contributing">Contributing</a>
</p>

---

<!-- Badges Section -->
<p align="center">
  <!-- Build & Platform -->
  <img src="https://img.shields.io/badge/Platform-ESP32--S3-blue?style=for-the-badge&logo=espressif&logoColor=white" alt="Platform"/>
  <img src="https://img.shields.io/badge/Framework-Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white" alt="Framework"/>
  <img src="https://img.shields.io/badge/PlatformIO-Compatible-orange?style=for-the-badge&logo=platformio&logoColor=white" alt="PlatformIO"/>
</p>

<p align="center">
  <!-- Status -->
  <img src="https://img.shields.io/badge/Status-Active%20Development-yellow?style=for-the-badge" alt="Status"/>
  <img src="https://img.shields.io/badge/Version-2.1.0-brightgreen?style=for-the-badge" alt="Version"/>
  <img src="https://img.shields.io/badge/License-Educational-green?style=for-the-badge" alt="License"/>
</p>

<p align="center">
  <!-- Hardware Specs -->
  <img src="https://img.shields.io/badge/Flash-16MB-purple?style=flat-square" alt="Flash"/>
  <img src="https://img.shields.io/badge/PSRAM-8MB%20OPI-purple?style=flat-square" alt="PSRAM"/>
  <img src="https://img.shields.io/badge/Display-AMOLED%201.8%22-ff69b4?style=flat-square" alt="Display"/>
  <img src="https://img.shields.io/badge/Resolution-368x448-ff69b4?style=flat-square" alt="Resolution"/>
</p>

<p align="center">
  <!-- Tech Stack -->
  <img src="https://img.shields.io/badge/GUI-LVGL%208.4-4B275F?style=flat-square&logo=lvgl&logoColor=white" alt="LVGL"/>
  <img src="https://img.shields.io/badge/AI-TensorFlow%20Lite-FF6F00?style=flat-square&logo=tensorflow&logoColor=white" alt="TensorFlow"/>
  <img src="https://img.shields.io/badge/WiFi-802.11%20b%2Fg%2Fn-1ea3d8?style=flat-square&logo=wifi&logoColor=white" alt="WiFi"/>
  <img src="https://img.shields.io/badge/BLE-5.0-0082FC?style=flat-square&logo=bluetooth&logoColor=white" alt="BLE"/>
</p>

<p align="center">
  <!-- Languages -->
  <img src="https://img.shields.io/badge/C++-17-00599C?style=flat-square&logo=c%2B%2B&logoColor=white" alt="C++"/>
  <img src="https://img.shields.io/badge/Python-3.x-3776AB?style=flat-square&logo=python&logoColor=white" alt="Python"/>
  <img src="https://img.shields.io/badge/HTML5-E34F26?style=flat-square&logo=html5&logoColor=white" alt="HTML5"/>
  <img src="https://img.shields.io/badge/JavaScript-F7DF1E?style=flat-square&logo=javascript&logoColor=black" alt="JavaScript"/>
</p>

---

## 🎯 About

**WavePwn** is a cutting-edge offensive wireless security framework designed specifically for the **Waveshare ESP32-S3-Touch-AMOLED-1.8** development board. Inspired by the legendary Pwnagotchi, it combines advanced WiFi/BLE attack capabilities with an adorable AI-powered mascot that learns and grows alongside you.

### 🌟 What Makes WavePwn Special?

| Feature | Description |
|---------|-------------|
| 🧠 **NEURA9 AI** | First embedded TensorFlow Lite AI for WiFi threat classification |
| 🐉 **Dragon Mascot** | Interactive mascot with 22+ animated expressions |
| 📱 **Modern UI** | Beautiful LVGL-based interface on AMOLED display |
| 🌐 **Web Dashboard** | Real-time monitoring via WebSocket |
| 🎙️ **Voice Control** | "Hey Dragon" wake word + 10 voice commands |
| 🔌 **Plugin System** | Extensible architecture with XP/leveling system |
| ⚡ **100% Offline** | All processing runs locally on the device |

---

## ✨ Features

### 📡 WiFi Capabilities
| Feature | Status | Description |
|---------|--------|-------------|
| 🔍 Network Scanning | ✅ | Discover all nearby WiFi networks |
| 🤝 Handshake Capture | ✅ | WPA/WPA2/WPA3 handshake capture |
| 🔑 PMKID Capture | ✅ | Hashcat-compatible PMKID extraction |
| ⚡ Deauthentication | ✅ | Targeted deauth attacks |
| 📻 Beacon Flood | ✅ | Custom SSID beacon spam |
| 👿 Evil Twin | ✅ | Rogue AP with captive portal |
| 🌐 Captive Portal | ✅ | Custom credential harvesting |
| 💾 PCAP Export | ✅ | Save captures to SD card |

### 📶 BLE Capabilities
| Feature | Status | Description |
|---------|--------|-------------|
| 🍎 Apple Spam | ✅ | Fake AirPods/AirTag popups |
| 🤖 Android Spam | ✅ | Fast Pair spam attacks |
| 🪟 Windows Spam | ✅ | Swift Pair notifications |
| 🔍 BLE Scanning | ✅ | Discover BLE devices |

### 🧠 NEURA9 AI Engine
| Feature | Status | Description |
|---------|--------|-------------|
| 🎯 Threat Detection | ✅ | 10 threat categories |
| 📊 Real-time Analysis | ✅ | ~50ms inference time |
| 🔄 Continuous Learning | ⏳ | Dataset collection mode |
| ⚙️ Adjustable Sensitivity | ✅ | 0.0 - 1.0 threshold |

<details>
<summary><b>📋 NEURA9 Threat Categories</b></summary>

| ID | Category | Description |
|----|----------|-------------|
| 0 | `SAFE` | No threats detected |
| 1 | `CROWDED` | High density WiFi environment |
| 2 | `OPEN_NETWORK` | Insecure open networks present |
| 3 | `EVIL_TWIN_RISK` | Possible evil twin attack |
| 4 | `DEAUTH_DETECTED` | Deauth attack in progress |
| 5 | `ROGUE_AP` | Malicious access point |
| 6 | `HIGH_RISK` | Multiple threat indicators |
| 7 | `BATTERY_CRITICAL` | Low power mode active |
| 8 | `GESTURE_COMMAND` | IMU gesture detected |
| 9 | `LEARNING_MODE` | Data collection active |

</details>

### 🎨 User Interface
| Feature | Status | Description |
|---------|--------|-------------|
| 🐉 Animated Mascot | ✅ | 22 expressions + idle animations |
| 🎨 10 Boot Themes | ✅ | Customizable boot animations |
| 📊 Status Bar | ✅ | Battery, WiFi, memory, time |
| ⚙️ 82+ Settings | ✅ | Comprehensive configuration |
| 🖼️ Wallpapers | ✅ | Custom backgrounds from SD |
| 🔔 Notifications | ✅ | Toast + vibration + sound |

### 🌐 Web Interface
| Feature | Status | Description |
|---------|--------|-------------|
| 📊 Live Dashboard | ✅ | Real-time stats via WebSocket |
| 📈 Charts | ✅ | Attack/capture visualization |
| ⚙️ Configuration | ✅ | Remote device settings |
| 📤 OTA Updates | ✅ | Over-the-air firmware updates |
| 📁 File Manager | ✅ | SD card browsing |

### 🎙️ Voice Assistant
| Feature | Status | Description |
|---------|--------|-------------|
| 👂 Wake Word | ✅ | "Hey Dragon" activation |
| 🗣️ Voice Commands | ✅ | 10+ supported commands |
| 📢 TTS Feedback | ✅ | Spoken responses |

<details>
<summary><b>📋 Voice Commands</b></summary>

| Command | Action |
|---------|--------|
| "Scan WiFi" | Start network scan |
| "Start Attack" | Begin selected attack |
| "Stop" | Stop current operation |
| "Status" | Speak current status |
| "BLE Spam" | Toggle BLE spam |
| "Beacon Flood" | Toggle beacon flood |
| "Deauth" | Start deauthentication |
| "Help" | List commands |
| "Sleep" | Enter sleep mode |
| "Wake" | Wake from sleep |

</details>

---

## 🔧 Hardware

### 📋 Specifications

<table>
<tr>
<td width="50%">

#### 🧠 Processor
| Component | Specification |
|-----------|---------------|
| **MCU** | ESP32-S3-WROOM-1-N16R8 |
| **CPU** | Xtensa LX7 dual-core @ 240MHz |
| **Flash** | 16 MB QIO |
| **PSRAM** | 8 MB OPI |

</td>
<td width="50%">

#### 📡 Connectivity
| Component | Specification |
|-----------|---------------|
| **WiFi** | 802.11 b/g/n (2.4GHz) |
| **Bluetooth** | BLE 5.0 |
| **USB** | Native USB OTG |

</td>
</tr>
<tr>
<td>

#### 🖥️ Display
| Component | Specification |
|-----------|---------------|
| **Type** | AMOLED |
| **Size** | 1.8 inches |
| **Resolution** | 368 × 448 px |
| **Driver** | SH8601 |
| **Interface** | QSPI |

</td>
<td>

#### 👆 Touch
| Component | Specification |
|-----------|---------------|
| **Controller** | FT3168 |
| **Type** | Capacitive |
| **Points** | Single touch |
| **I2C Address** | 0x38 |

</td>
</tr>
<tr>
<td>

#### 🔋 Power
| Component | Specification |
|-----------|---------------|
| **PMU** | AXP2101 |
| **Battery** | Li-Po support |
| **Charging** | 500mA configurable |
| **I2C Address** | 0x34 |

</td>
<td>

#### 📐 Sensors
| Component | Specification |
|-----------|---------------|
| **IMU** | QMI8658C 6-axis |
| **RTC** | PCF85063 |
| **Audio** | ES8311 codec |
| **IO Expander** | TCA9554 |

</td>
</tr>
</table>

### 📍 Pinout

<details>
<summary><b>Click to expand pinout</b></summary>

```
┌─────────────────────────────────────┐
│      ESP32-S3-Touch-AMOLED-1.8      │
├─────────────────────────────────────┤
│ PIN │ FUNCTION    │ DESCRIPTION     │
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
```

</details>

---

## 🚀 Installation

### 📋 Prerequisites

| Requirement | Version | Purpose |
|-------------|---------|---------|
| PlatformIO | Latest | Build system |
| Python | 3.8+ | AI training scripts |
| MicroSD | 4-128GB | Storage (FAT32) |

### ⚡ Quick Start

```bash
# 1. Clone the repository
git clone https://github.com/your-username/wavepwn.git
cd wavepwn

# 2. Build and upload
pio run -t upload

# 3. Monitor serial output
pio device monitor
```

### 📦 Build Configuration

The `platformio.ini` is pre-configured with optimal settings:

```ini
[env:wavepwn_final]
platform = espressif32@6.9.0
board = esp32-s3-devkitc-1
framework = arduino

# Memory configuration
board_build.flash_size = 16MB
board_build.psram = enabled
board_build.psram_type = opi

# Build optimizations
build_flags = 
    -D LV_CONF_INCLUDE_SIMPLE
    -D BOARD_HAS_PSRAM
    -O2
```

### 🌐 First Boot

1. **Power on** the device
2. **Wait** for boot animation to complete
3. **Connect** to WiFi: `WavePwn` (password: `wavepwn`)
4. **Open** browser: `http://192.168.4.1`
5. **Explore** the web dashboard!

---

## 📁 Project Structure

```
WavePwn/
├── 📁 docs/                 # 📚 Documentation
│   ├── ESPECIFICACAO_HARDWARE.md
│   ├── TUTORIAL_TREINAMENTO_NEURA9.md
│   └── ...
├── 📁 src/                  # 💻 Source code
│   ├── main.cpp             # Entry point
│   ├── 📁 ai/               # 🧠 NEURA9 AI inference
│   ├── 📁 core/             # ⚙️ Configuration & globals  
│   ├── 📁 hardware/         # 🔧 Hardware drivers
│   ├── 📁 neura9/           # 🎯 AI model data
│   ├── 📁 plugins/          # 🔌 Plugin system
│   ├── 📁 pwnagotchi/       # 🐉 Mascot logic
│   ├── 📁 ui/               # 🎨 LVGL interface
│   ├── 📁 voice/            # 🎙️ Voice assistant
│   ├── 📁 web/              # 🌐 Web server
│   └── 📁 wifi/             # 📡 WiFi attacks
├── 📁 data/                 # 📦 Web assets (HTML/CSS/JS)
├── 📁 lib/                  # 📚 Libraries
├── 📁 ai_training/          # 🐍 Python training scripts
└── platformio.ini          # ⚙️ Build configuration
```

---

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [📋 Hardware Specs](docs/ESPECIFICACAO_HARDWARE.md) | Complete hardware documentation |
| [🧠 NEURA9 Training](docs/TUTORIAL_TREINAMENTO_NEURA9.md) | AI model training guide |
| [🔧 Hardware Fixes](docs/CORRECOES_HARDWARE.md) | Hardware corrections log |
| [📊 Progress](docs/PROGRESSO_PENDENCIAS.md) | Development progress tracking |
| [✅ Resolved Issues](docs/PENDENCIAS_RESOLVIDAS.md) | Fixed issues documentation |
| [📈 Before/After](docs/COMPARACAO_ANTES_DEPOIS.md) | Hardware comparison |
| [✔️ Validation Guide](docs/GUIA_VALIDACAO_HARDWARE.md) | Hardware validation tests |

---

## 🎮 Usage

### 🖐️ Touch Gestures

| Gesture | Action |
|---------|--------|
| **Tap** | Select/confirm |
| **Swipe Left/Right** | Navigate screens |
| **Swipe Up** | Quick settings |
| **Swipe Down** | Notifications |
| **Long Press** | Context menu |

### 🎙️ Voice Control

Say **"Hey Dragon"** followed by:
- `"Scan WiFi"` - Start network scan
- `"Start Attack"` - Begin attack
- `"Status"` - Get current status
- `"Stop"` - Stop all operations
- `"Sleep"` - Enter power save

---

## 🤝 Contributing

Contributions are welcome! Please read our guidelines:

1. **Fork** the repository
2. **Create** a feature branch: `git checkout -b feature/amazing-feature`
3. **Commit** changes: `git commit -m 'Add amazing feature'`
4. **Push** to branch: `git push origin feature/amazing-feature`
5. **Open** a Pull Request

### 📋 Code Standards

- ✅ Follow existing code style
- ✅ Document your changes
- ✅ Test on real hardware when possible
- ✅ Keep security focus (defensive tools only)

---

## ⚠️ Legal Disclaimer

> **🚨 EDUCATIONAL PURPOSES ONLY**

This project is intended for **authorized security testing** and **educational purposes** only.

- ✅ Use **ONLY** on networks you own or have **explicit written permission**
- ❌ **NEVER** use against networks without authorization
- ❌ **Unauthorized use is ILLEGAL** and may violate local laws
- ✅ The authors are **NOT responsible** for any misuse

By using this software, you agree to use it responsibly and ethically.

---

## 📜 License

This project is provided **"as-is"** for educational purposes.

```
MIT License - Educational Use

Permission is granted for educational and research purposes.
Commercial use requires explicit permission.
The authors are not liable for any damages or misuse.
```

---

## 🔗 Resources

| Resource | Link |
|----------|------|
| 📖 Waveshare Wiki | [ESP32-S3-Touch-AMOLED-1.8](https://www.waveshare.com/wiki/ESP32-S3-Touch-AMOLED-1.8) |
| 🛠️ PlatformIO | [platformio.org](https://platformio.org/) |
| 🎨 LVGL | [lvgl.io](https://lvgl.io/) |
| 🧠 TensorFlow Lite | [Microcontrollers](https://www.tensorflow.org/lite/microcontrollers) |
| 📦 ESP-IDF | [Espressif](https://docs.espressif.com/) |

---

## 💖 Acknowledgments

Special thanks to:

- 🌊 **Waveshare Electronics** - Amazing hardware
- 🎨 **LVGL Community** - Beautiful UI framework
- 🧠 **TensorFlow Team** - Embedded AI capabilities
- 🛠️ **PlatformIO** - Excellent build system
- 🌐 **ESP32 Community** - Endless support

---

<p align="center">
  <img src="https://img.shields.io/badge/Made%20with-❤️-red?style=for-the-badge" alt="Made with love"/>
  <img src="https://img.shields.io/badge/Powered%20by-☕-brown?style=for-the-badge" alt="Powered by coffee"/>
  <img src="https://img.shields.io/badge/For-Security%20Research-blue?style=for-the-badge" alt="For Security Research"/>
</p>

<p align="center">
  <strong>🐉 "Defending networks, one handshake at a time" 🛡️</strong>
</p>

<p align="center">
  <sub>Built with 💜 for the security research community</sub>
</p>
