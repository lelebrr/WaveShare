<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Assets-Graphics%20%26%20Audio-purple?style=for-the-badge" alt="Assets"/>
  <img src="https://img.shields.io/badge/Sprites-Pixel%20Art-pink?style=for-the-badge" alt="Graphics"/>
  <img src="https://img.shields.io/badge/Audio-16kHz%20PCM-blue?style=for-the-badge" alt="Audio"/>
</p>

# 🎨 Guia de Ativos (Assets) - LeleWatch

Este documento descreve as especificações para criar sons, imagens e temas para o LeleWatch.

---

# PARTE 1: GRÁFICOS (Sprites & UI)

## 🎨 Especificações Técnicas Globais

*   **Formato de Arquivo:** PNG (Fundo Transparente Obrigatório).
*   **Palette de Cores:** 8-bit Indexada (256 cores) para sprites; 32-bit para UI de alta fidelidade.
*   **Margem de Segurança:** 2px de borda transparente em todos os sprites para evitar artefatos de renderização.
*   **Nomenclatura:** Tudo em `snake_case` (letras minúsculas e underline).

## 📁 Estrutura de Pastas (Gráficos)

```text
data/
├── logo/                   # Logos do Boot (Device)
├── web/
│   ├── assets/             # Assets estáticos da Web
│   │   ├── logo_loading.png    # Logo da tela de boot web
│   │   └── logo_dashboard.png  # Logo da sidebar (dashboard)
│   └── badges/             # Ícones de Conquista
└── sprites/
    ├── system/             # Ícones universais (Bateria, WiFi)
    ├── dragon/             # Mascote 0: Neura
    ├── lele/               # Mascote 1: Lele
    └── lisa/               # Mascote 2: Lisa
```

## 🌐 Web Dashboard Logos (NOVO)
*Local: `data/web/assets/`*

### Logo da Tela de Boot
| Arquivo | Resolução | Formato | Descrição |
| :--- | :--- | :--- | :--- |
| `logo_loading.png` | **512x512px** | PNG (transparência) | Logo exibido durante o boot da web. Animação fade-in com glow degradê ciano/magenta. Responsivo: ajusta de 80px (mobile) até 300px (4K). |

### Logo da Sidebar (Dashboard)
| Arquivo | Resolução | Formato | Descrição |
| :--- | :--- | :--- | :--- |
| `logo_dashboard.png` | **400x120px** (horizontal) ou **200x200px** (quadrado) | PNG (transparência) | Logo do canto superior esquerdo da sidebar. Substitui o emoji do mascote + nome "LeleWatch". Responsivo com glow dinâmico no hover.

## 1. ⚙️ System Assets (Universais)
*Local: `data/sprites/system/`*

### 1.1 Status e Alertas
| Arquivo | Resolução | Descrição Visual Exata |
| :--- | :--- | :--- |
| `sys_battery_low.png` | 64x64px | Bateria vermelha piscando ou pegando fogo estilizado. |
| `sys_battery_charge.png` | 64x64px | Raio verde neon pulsando sobre uma bateria cheia. |
| `sys_wifi_off.png` | 64x64px | Ícone de WiFi quebrado ou com um "X" glitchado vermelho. |
| `sys_temp_hot.png` | 64x64px | Termômetro estourando / vermelho brilhante (Alerta de superaquecimento). |
| `sys_sd_error.png` | 64x64px | Cartão SD triste ou com um curativo/band-aid. |

### 1.2 Boot & Updates
| Arquivo | Resolução | Descrição Visual Exata |
| :--- | :--- | :--- |
| `sys_ota_1.png` | 120x120px | Ícone de chip/IC recebendo dados (Frame 1). |
| `sys_ota_2.png` | 120x120px | Ícone de chip/IC processando dados (Frame 2 - Giro). |
| `sys_boot_logo.png` | 240x240px | Logo "WavePwn" High-Res para boot inicial. |

## 2. 🐉 Mascote: NEURA (O Dragão Cyber)
*Pasta: `data/sprites/dragon/`*
*Tema de Cor (Hex): `#00FFF5` (Ciano)*

### 2.1 Estado: Ocioso (Idle)
| Arquivo | Ação | Descrição Visual Exata |
| :--- | :--- | :--- |
| `s_idle_1.png` | Float | Dragão flutuando suavemente no ar, cauda balançando. |
| `s_idle_2.png` | Smoke | Pequena fumaça sai das narinas (resfriamento líquido). |
| `s_idle_3.png` | Glitch | O corpo do dragão sofre um leve glitch visual (falha na renderização) por um frame. |

### 2.2 Estado: Ataque (Breath)
| Arquivo | Ação | Descrição Visual Exata |
| :--- | :--- | :--- |
| `s_attack_1.png` | Charge | peito e garganta brilham intensamente em azul neon. |
| `s_attack_2.png` | Fire | Rajada de fogo digital (pixels azuis e brancos) sai da boca. |
| `s_attack_3.png` | Beam | A rajada vira um laser contínuo concentrado. |

*(Nota: Especificações completas para LELE e LISA disponíveis nos arquivos de design originais, omitidos aqui para brevidade se não implementados no core atual)*

---

# PARTE 2: ÁUDIO & VOZ

## 🎵 Especificações Técnicas (MANDATÓRIO)

Para garantir compatibilidade com o amplificador **ES8311** e o buffer de áudio do ESP32, **TODOS** os arquivos devem seguir estritamente este formato:

*   **Formato do Arquivo:** `.wav` (Waveform Audio File Format)
*   **Taxa de Amostragem:** 16000 Hz (16kHz)
*   **Canais:** Mono (1 canal)
*   **Profundidade de Bits:** 16-bit PCM (Little Endian)
*   **Metadados:** Nenhum (Remover tags ID3/Metadata)

> **⚠️ Atenção:** Arquivos Stereo ou 44.1kHz causarão crash ou áudio distorcido ("chipmunk").

## 📁 Estrutura de Pastas (Áudio)

```text
data/
└── sounds/
    ├── system/        # Sons de interface (Clicks, Beeps)
    └── tts/           # Vozes e frases da IA (Text-to-Speech pré-gravado)
```

## 1. 📢 Text-to-Speech (TTS) e IA
*Local: `data/sounds/tts/`*

### 1.1 Comandos de Sistema
| Arquivo | Tamanho Max | Duração Max | Descrição / Contexto |
| :--- | :--- | :--- | :--- |
| `hello.wav` | 15kb | 3s | **"Iniciando..."** (Boot). |
| `goodbye.wav` | 15kb | 3s | **"Desligando..."** (Shutdown). |
| `listening.wav` | 5kb | 1s | Som de sonar "Plim!" (Wake word). |
| `processing.wav`| 10kb | 2s | Som de computação/dados. |
| `error.wav` | 8kb | 1.5s | **"Erro."** ou Glitch sound. |

### 1.2 Frases de Mascote
| Arquivo | Texto Sugerido |
| :--- | :--- |
| `tts_found_wifi.wav` | "Rede encontrada!" |
| `tts_attack_start.wav` | "Iniciando ataque!" |
| `tts_handshake.wav` | "Handshake capturado!" |
| `tts_battery_low.wav` | "Preciso de energia..." |

## 2. 🔊 Efeitos Sonoros do Sistema (SFX)
*Local: `data/sounds/system/`*

| Arquivo | Descrição | Uso |
| :--- | :--- | :--- |
| `click.wav` | Clique mecânico seco. | Toque em botões. |
| `swipe.wav` | "Swoosh" rápido. | Troca de menus. |
| `success.wav` | Acorde maior ascendente. | Sucesso. |
| `warning.wav` | Sirene curta. | Alerta/Perigo. |
| `lock.wav` | Trava sci-fi. | Bloqueio. |

## 3. ⚔️ Sons Específicos de Ataques

| Arquivo | Descrição |
| :--- | :--- |
| `pwn_ka_ching.wav` | Caixa registradora (Handshake). |
| `pwn_laser.wav` | Disparo laser (Deauth). |
| `pwn_radar.wav` | Sonar ping (Scan). |
| `pwn_glitch.wav` | Ruído branco (Jamming). |

---
**Última Atualização:** 2025-12-09
