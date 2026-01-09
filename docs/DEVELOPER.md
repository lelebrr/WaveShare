<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Arquitetura-Sistema-blue?style=for-the-badge" alt="Architecture"/>
  <img src="https://img.shields.io/badge/Módulos-13+-green?style=for-the-badge" alt="Modules"/>
  <img src="https://img.shields.io/badge/Design-Modular-purple?style=for-the-badge" alt="Design"/>
</p>

---

# 🏗️ GUIA DO DESENVOLVEDOR - LeleWatch v2.1.1

## 📋 Visão Geral

O LeleWatch é construído com uma arquitetura modular que separa responsabilidades em camadas bem definidas.

```
┌────────────────────────────────────────────────────────────────┐
│                        🖥️ INTERFACE                            │
├────────────────────────────────────────────────────────────────┤
│  LVGL UI   │   Web Dashboard   │   Voice Assistant   │ Serial │
├────────────────────────────────────────────────────────────────┤
│                     🔌 CAMADA DE PLUGINS                        │
├────────────────────────────────────────────────────────────────┤
│ ExpSystem │ BatteryMonitor │ AgeTracker │ SessionStats │ Clock │
├────────────────────────────────────────────────────────────────┤
│                     🧠 CAMADA DE LÓGICA                         │
├────────────────────────────────────────────────────────────────┤
│  Pwnagotchi  │  NEURA9 AI  │  WiFi Attacks  │  BLE Spam       │
├────────────────────────────────────────────────────────────────┤
│                     🔧 CAMADA DE HARDWARE                       │
├────────────────────────────────────────────────────────────────┤
│ Display │ Touch │ WiFi │ BLE │ Audio │ PMU │ IMU │ RTC │ SD   │
└────────────────────────────────────────────────────────────────┘
```

---

## 📁 Estrutura de Módulos

### 🧠 AI (src/ai/)
| Arquivo | Descrição |
|---------|-----------|
| `neura9_inference.h/cpp` | Interface de inferência NEURA9 |

### 📡 WiFi (src/wifi/)
| Arquivo | Descrição |
|---------|-----------|
| `wifi_attacks.h/cpp` | Deauth, Beacon Flood, Probe |
| `captive_portal.h/cpp` | Evil Twin com portal cativo |
| `wps_attacks.h/cpp` | Ataques WPS |

### 🎨 UI (src/ui/)
| Arquivo | Descrição |
|---------|-----------|
| `ui_main.h/cpp` | Tela principal |
| `ui_attacks.h/cpp` | Menu de ataques |
| `ui_settings*.h/cpp` | Configurações (82 opções) |
| `boot_animation.h/cpp` | Animação de boot |
| `boot_themes.h/cpp` | 10 temas de boot |
| `mascot_faces.h/cpp` | 22+ expressões do mascote |
| `status_bar.h/cpp` | Barra de status |
| `notifications_engine.h/cpp` | Sistema de notificações |
| `wallpaper_system.h/cpp` | Sistema de wallpapers |
| `sounds_manager.h/cpp` | Gerenciador de sons |

### 🔧 Hardware (src/hardware/)
| Arquivo | Descrição |
|---------|-----------|
| `wifi_driver.h/cpp` | Driver WiFi |
| `ble_driver.h/cpp` | Driver BLE |
| `audio_driver.h/cpp` | Driver áudio ES8311 |
| `lvgl_driver.h/cpp` | Driver LVGL |
| `system_hardware.h/cpp` | Inicialização de hardware |
| `es8311.h/c` | Codec de áudio |

### 🔌 Plugins (src/plugins/)
| Plugin | Descrição |
|--------|-----------|
| `exp_system` | Sistema de XP e níveis |
| `battery_monitor` | Monitoramento de bateria |
| `age_tracker` | Tempo de funcionamento |
| `session_stats` | Estatísticas de sessão |
| `clock_display` | Display de relógio |
| `memtemp` | Memória e temperatura |
| `internet_check` | Verificação de internet |

### 🐉 Pwnagotchi (src/pwnagotchi/)
| Arquivo | Descrição |
|---------|-----------|
| `pwnagotchi.h/cpp` | Controlador principal |
| `personality.h/cpp` | Personalidade do mascote |

### 🎙️ Voice (src/voice/)
| Arquivo | Descrição |
|---------|-----------|
| `voice_assistant.h/cpp` | Assistente de voz |

### 🌐 Web (src/web/)
| Arquivo | Descrição |
|---------|-----------|
| `web_server.h/cpp` | Servidor web AsyncWebServer |
| `web_api.h/cpp` | APIs REST |

---

## 🔄 Fluxo de Inicialização

```
┌──────────────────────────────────────────────────────────────┐
│ 1. setup()                                                    │
│    ├─ Serial.begin()                                          │
│    ├─ config_manager.begin()     ← Carrega NVS               │
│    ├─ pwn.begin()               ← Hardware + Display + LVGL  │
│    ├─ registerDefaultPlugins()   ← Registra plugins          │
│    ├─ pluginManager.begin()      ← Inicializa plugins        │
│    ├─ neura9.begin()            ← Carrega modelo TFLite      │
│    ├─ audioDriver.begin()        ← ES8311 + I2S              │
│    ├─ voiceAssistant.begin()     ← Wake word + TTS           │
│    └─ bootAnimation.start()      ← Animação de boot          │
└──────────────────────────────────────────────────────────────┘
                            ↓
┌──────────────────────────────────────────────────────────────┐
│ 2. loop()                                                     │
│    ├─ lv_timer_handler()         ← Atualiza LVGL             │
│    ├─ pluginManager.update()     ← Atualiza todos plugins    │
│    ├─ mascotFaces.update()       ← Animações do mascote      │
│    ├─ audioDriver.update()       ← Processa áudio            │
│    ├─ voiceAssistant.update()    ← Processa voz              │
│    ├─ statusBar.update()         ← Atualiza barra de status  │
│    └─ pwn.loop()                 ← Loop principal            │
└──────────────────────────────────────────────────────────────┘
```

---

## 📊 Diagrama de Dependências

```mermaid
graph TD
    A[main.cpp] --> B[Pwnagotchi]
    A --> C[PluginManager]
    A --> D[NEURA9]
    A --> E[AudioDriver]
    A --> F[VoiceAssistant]
    A --> G[BootAnimation]
    
    B --> H[WiFiDriver]
    B --> I[BLEDriver]
    B --> J[WebServer]
    
    C --> K[ExpSystem]
    C --> L[BatteryMonitor]
    C --> M[AgeTracker]
    
    D --> N[TFLite Model]
    
    F --> E
    F --> D
    
    G --> O[BootThemes]
    G --> P[StatusBar]
```

---

## 🔐 Segurança

### Princípios de Design
- ✅ **100% Offline** - Nenhum dado enviado para nuvem
- ✅ **Sem Telemetria** - Privacidade total
- ✅ **Configurável** - Usuário controla tudo
- ✅ **Defensivo por Design** - Foco em detecção, não ataque

---

**Última Atualização:** 2025-12-09

---

# 🤝 Guia de Contribuição

Obrigado por considerar contribuir com o LeleWatch! Este guia ajudará você a começa.

## 📜 Código de Conduta

### ✅ Comportamento Esperado
- Seja respeitoso e inclusivo
- Aceite críticas construtivas
- Foque no que é melhor para a comunidade
- Mostre empatia com outros contribuidores

### ❌ Comportamento Inaceitável
- Uso de linguagem ou imagens ofensivas
- Trolling ou comentários depreciativos
- Assédio público ou privado
- Publicar informações privadas de outros

---

## 🚀 Como Contribuir

### 1. 🍴 Fork o Repositório
```bash
# Clone seu fork
git clone https://github.com/SEU-USERNAME/lelewatch.git
cd lelewatch
```

### 2. 🌿 Crie uma Branch
```bash
# Para features
git checkout -b feature/nome-da-feature

# Para bugfixes
git checkout -b fix/descricao-do-bug

# Para docs
git checkout -b docs/descricao
```

### 3. 💻 Faça suas Mudanças
- Siga os padrões de código
- Adicione testes quando possível
- Atualize documentação relevante

### 4. ✅ Commit
```bash
git add .
git commit -m "feat: adiciona nova funcionalidade X"
```

### 5. 📤 Push e PR
```bash
git push origin feature/nome-da-feature
```
Então abra um Pull Request no GitHub.

---

## 🛠️ Configurando o Ambiente

### Requisitos
| Ferramenta | Versão | Propósito |
|------------|--------|-----------|
| PlatformIO | Latest | Build system |
| Python | 3.8+ | Scripts de treinamento |
| VS Code | Latest | IDE recomendada |
| Git | Latest | Controle de versão |

### Instalação
```bash
# 1. Instale PlatformIO CLI
pip install platformio

# 2. Clone o repositório
git clone https://github.com/SEU-USERNAME/lelewatch.git
cd lelewatch

# 3. Compile
pio run

# 4. Upload (com hardware conectado)
pio run -t upload
```

---

## 📝 Padrões de Código

### C++ Style Guide

```cpp
// ✅ Nomes de classes em PascalCase
class PluginManager {

// ✅ Nomes de funções em camelCase
void updateStatus();

// ✅ Nomes de variáveis em snake_case
int battery_percent;

// ✅ Constantes em SCREAMING_SNAKE_CASE
#define MAX_NETWORKS 50

// ✅ Comentários em português ou inglês
// Atualiza o status da bateria
void updateBattery();
};
```

### Commits
Siga [Conventional Commits](https://www.conventionalcommits.org/):

| Prefixo | Uso |
|---------|-----|
| `feat:` | Nova feature |
| `fix:` | Correção de bug |
| `docs:` | Documentação |
| `style:` | Formatação |
| `refactor:` | Refatoração |
| `test:` | Testes |
| `chore:` | Manutenção |

