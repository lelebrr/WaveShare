<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Changelog-Histórico-blue?style=for-the-badge" alt="Changelog"/>
  <img src="https://img.shields.io/badge/Versão%20Atual-2.1.0-brightgreen?style=for-the-badge" alt="Version"/>
  <img src="https://img.shields.io/badge/Atualizado-Dezembro%202025-orange?style=for-the-badge" alt="Updated"/>
</p>

---

# 📋 CHANGELOG - WavePwn

Todas as mudanças notáveis neste projeto serão documentadas neste arquivo.

O formato é baseado em [Keep a Changelog](https://keepachangelog.com/pt-BR/).

---

## [2.1.0] - 2025-12-08

### ✨ Adicionado
- 🧠 **NEURA9 AI** - Sistema de IA defensiva com TensorFlow Lite
  - 10 categorias de classificação de ameaças
  - Inferência em tempo real (~50ms)
  - Alocação dinâmica em PSRAM
- 🎙️ **Assistente de Voz** - Controle por comandos de voz
  - Wake word "Hey Dragon"
  - 10+ comandos suportados
  - Text-to-Speech para feedback
- 🔌 **Sistema de Plugins** - Arquitetura extensível
  - ExpSystem (XP e níveis)
  - BatteryMonitor
  - AgeTracker
  - SessionStats
  - ClockDisplay
  - MemTemp
  - InternetCheck
- 🎨 **10 Temas de Boot** - Animações de inicialização customizáveis
- ⚙️ **82+ Configurações** - Menu de configurações completo
  - Display & Som
  - Rede & Wireless
  - Ataques & Automação
  - Sistema & Energia
  - Personalidade do Dragão
- 🖼️ **Sistema de Wallpapers** - Papéis de parede do SD card
- 🔔 **Engine de Notificações** - Toast, vibração e sons
- 🔊 **Gerenciador de Sons** - Sons customizáveis do SD card
- 📚 **Documentação Completa** - 10+ arquivos de documentação

### 🔧 Corrigido
- ✅ Overflow de DRAM (economizados 204KB)
- ✅ Driver IMU QMI8658C corrigido
- ✅ APIs de configuração do acelerômetro/giroscópio
- ✅ Guards de pré-processador para conflitos
- ✅ Documentação de hardware (SH8601, FT3168)

### 🔄 Modificado
- Tensor arena movido para PSRAM
- Otimização de uso de memória
- Refatoração do sistema de configurações

---

## [2.0.0] - 2025-12-04

### ✨ Adicionado
- 🖥️ **Suporte Waveshare ESP32-S3-Touch-AMOLED-1.8**
  - Display AMOLED 368x448
  - Touch FT3168 capacitivo
  - PMU AXP2101
- 🐉 **Mascote Animado** - 22+ expressões
- 🌐 **Web Dashboard** - Interface web responsiva
  - Monitoramento em tempo real via WebSocket
  - Gráficos Chart.js
  - Configuração remota
- 📡 **Ataques WiFi Avançados**
  - Deauthentication
  - Beacon Flood
  - Evil Twin
  - Handshake Capture
  - PMKID Capture
- 📶 **BLE Spam** - Apple, Android, Windows

### 🔧 Corrigido
- Integração PSRAM OPI
- Partição huge_app.csv

---

## [1.0.0] - 2025-11-01

### ✨ Adicionado
- 🎯 Versão inicial do projeto
- 📡 Scanner WiFi básico
- 🎨 Interface LVGL inicial
- 🔧 Drivers de hardware básicos

---

## 📝 Legenda

| Emoji | Significado |
|-------|-------------|
| ✨ | Nova feature |
| 🔧 | Correção de bug |
| 🔄 | Mudança/refatoração |
| ⚠️ | Deprecated |
| 🗑️ | Removido |
| 🔒 | Segurança |
| 📝 | Documentação |
| 🚀 | Performance |

---

## 🔮 Roadmap

### v2.2.0 (Planejado)
- [ ] Modo Relógio Smartwatch
- [ ] Sistema de Múltiplos Mascotes
- [ ] Suporte WPA3-SAE
- [ ] Integração Home Assistant

### v3.0.0 (Futuro)
- [ ] Treinamento NEURA9 avançado
- [ ] Modo mesh entre dispositivos
- [ ] App mobile companion

---

**Mantido com ❤️ pela comunidade WavePwn**
