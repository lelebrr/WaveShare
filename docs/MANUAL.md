<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/Tipo-Guia%20do%20Usuário-blue?style=for-the-badge" alt="User Guide"/>
  <img src="https://img.shields.io/badge/Nível-Iniciante%20a%20Avançado-green?style=for-the-badge" alt="Level"/>
  <img src="https://img.shields.io/badge/Idioma-Português-yellow?style=for-the-badge" alt="Language"/>
</p>

---

# 📖 GUIA DO USUÁRIO - LeleWatch v2.2.0

## 📋 Índice

1. [Primeiro Uso](#-primeiro-uso)
2. [Navegação na Interface](#-navegação-na-interface)
3. [Menu Principal](#-menu-principal)
4. [Ataques WiFi](#-ataques-wifi)
5. [Ataques BLE](#-ataques-ble)
6. [NEURA9 AI](#-neura9-ai)
7. [Configurações](#-configurações)
8. [Web Dashboard](#-web-dashboard)
9. [Comandos de Voz](#-comandos-de-voz)
10. [Dicas Avançadas](#-dicas-avançadas)

---

## 🚀 Primeiro Uso

### 1. Ligando o Dispositivo
1. Conecte uma bateria LiPo ou cabo USB-C
   *Nota: Certifique-se de ter feito o "Upload Filesystem Image" no PlatformIO para que os arquivos do site funcionem.*
2. Aguarde a animação de boot (5-10 segundos)
3. O mascote aparecerá na tela inicial

### 2. Conectando ao Web Dashboard
1. O dispositivo cria um AP WiFi automaticamente
   - **SSID:** `LeleWatch`
   - **Senha:** `lelewatch`
2. Conecte seu computador/celular
3. Abra o navegador em `http://192.168.4.1`

### 3. Configuração Inicial
1. Acesse Configurações no dispositivo ou web
2. Defina seu nome de usuário
3. Ajuste brilho e som conforme preferência
4. Configure o nome do mascote

---

## 🖐️ Navegação na Interface

### Gestos Touch

| Gesto | Ação |
|-------|------|
| **Toque** | Selecionar/confirmar |
| **Deslizar ←→** | Navegar entre telas |
| **Deslizar ↑** | Configurações rápidas |
| **Deslizar ↓** | Notificações |
| **Toque longo** | Menu de contexto |

### Barra de Status

```
┌────────────────────────────────────┐
│ 🔋 85%  📶 CH6  ⏰ 12:30  🧠 SAFE │
└────────────────────────────────────┘
```

| Ícone | Significado |
|-------|-------------|
| 🔋 | Nível de bateria |
| 📶 | Canal WiFi atual |
| ⏰ | Hora atual |
| 🧠 | Status NEURA9 |

---

## 📱 Menu Principal

### Telas Disponíveis

1. **🏠 Home** - Tela principal com mascote
2. **📡 WiFi** - Ataques e scans WiFi
3. **📶 BLE** - Ataques Bluetooth
4. **🧠 AI** - Status e controle NEURA9
5. **🛠️ Tools** - Ferramentas úteis
6. **🎙️ Voice** - Controle de voz
7. **⚙️ Settings** - Configurações

---

## 📡 Ataques WiFi

### 🔍 Scan de Redes

1. Acesse **WiFi → Scan**
2. Aguarde a varredura (10-30 segundos)
3. Lista de redes aparecerá ordenada por sinal
4. Toque em uma rede para ver detalhes

### ⚡ Deauthentication

1. Selecione uma rede alvo
2. Toque em **Deauth**
3. Configure:
   - **Duração:** 10-300 segundos
   - **Intervalo:** 50-500ms
4. Toque em **Iniciar**

> ⚠️ Use apenas em redes autorizadas!

### 📻 Beacon Flood

1. Acesse **WiFi → Beacon Flood**
2. Configure:
   - **SSIDs:** Lista de nomes fake
   - **Tipo:** Aberto ou WPA2 fake
   - **Intervalo:** Velocidade
3. Inicie e observe a confusão!

### 👿 Evil Twin

1. Selecione rede alvo
2. Toque em **Evil Twin**
3. Configure portal cativo
4. Aguarde vítimas se conectarem
5. Credenciais aparecem na tela

### 🤝 Captura de Handshake

1. Selecione rede alvo
2. Toque em **Handshake Capture**
3. Opcionalmente, force deauth para acelerar
4. Handshake salvo no SD: `/lelewatch/handshakes/`

### ☢️ One-Tap Nuke (Deauth All)
> **🚨 PERIGO:** Use com extrema cautela!

1.  Toque e segure o avatar do dragão na Home.
2.  **OU** acesse **WiFi → Nuke**.
3.  O dispositivo enviará frames de deauth bradcast em **todos os canais** rapidamente.
4.  Efeito: Desconecta todos dispositivos em um raio de ~20m.

---

## 📺 Controle Universal (IR Remote)

Transforme seu LeleWatch em um controle remoto universal ou TV-B-Gone.

### TV-B-Gone (Desligar Tudo)
1.  Acesse **Tools → IR Nuke**.
2.  O LED infravermelho piscará enviando códigos de "Power Off" para 100+ marcas de TV.
3.  Mire para a TV e aguarde (o ciclo leva ~30s).

### Controle Universal
1.  Acesse o Web Dashboard (`http://192.168.4.1/ir`).
2.  Clique nos botões virtuais (Volume, Canal, Power).
3.  Ou use `/ir_learn` para copiar um controle existente.

---

## 🖥️ Acesso Remoto (VNC)

Controle seu LeleWatch pelo navegador em tempo real!

### Como usar

1. Conecte-se ao WiFi do LeleWatch
2. Abra `http://192.168.4.1`
3. Clique em **"🖥️ Acesso Remoto"** no menu lateral
4. A tela do dispositivo aparecerá em um frame de smartwatch
5. **Clique em qualquer lugar** da tela para interagir

### Dicas
- A tela é atualizada automaticamente (~8-10 FPS)
- O toque no navegador simula toque no dispositivo
- Use "Abrir em Nova Aba" para visualização fullscreen
- Funciona em celular e desktop

### Limitações
- Latência de ~50-100ms (normal para WiFi)
- Requer conexão WiFi ativa com o dispositivo

---

## 📶 Ataques BLE

### 🍎 Apple Spam
Envia notificações fake de AirPods/AirTag para iPhones próximos.

### 🤖 Android Spam
Fast Pair spam para dispositivos Android.

### 🪟 Windows Spam
Swift Pair para PCs Windows.

### Configuração
1. Acesse **BLE → Spam**
2. Selecione tipo de alvo
3. Configure intervalo (50-500ms)
4. Inicie o spam

---

## 🧠 NEURA9 AI

### O que é?
IA embarcada que analisa o ambiente WiFi e classifica ameaças em tempo real.

### Como usar
1. Acesse **AI → Status**
2. Veja classificação atual
3. Ajuste sensibilidade se necessário

### Interpretando resultados

| Status | Significado | Ação |
|--------|-------------|------|
| 🟢 SAFE | Ambiente seguro | Continue normalmente |
| 🟡 CROWDED | Muitas redes | Normal em lugares públicos |
| 🟡 OPEN_NETWORK | Redes abertas | Cuidado ao conectar |
| 🔴 DEAUTH_DETECTED | Ataque em curso | Investigue! |
| 🔴 EVIL_TWIN_RISK | Possível AP fake | Não conecte |
| 🔴 HIGH_RISK | Múltiplas ameaças | Afaste-se |

---

## ⚙️ Configurações

### Categorias

#### 🖥️ Display & Som
- Brilho (0-100%)
- Volume (0-100%)
- Timeout de tela
- Tema de cores

#### 📡 Rede & Wireless
- SSID do AP
- Senha do AP
- Canal padrão
- Potência WiFi

#### ⚔️ Ataques & Automação
- Duração padrão de deauth
- Auto-captura de handshake
- Beacon flood settings

#### 🔋 Sistema & Energia
- Tema de boot (10 opções)
- Sleep timeout
- Economy mode

#### 🐉 Personalidade do Dragão
- Nome do mascote
- Velocidade de animação
- Expressões ativas
- Frases customizadas

---

## 🌐 Web Dashboard

### URL
`http://192.168.4.1`

### Funcionalidades

1. **📊 Dashboard** - Visão geral em tempo real
2. **📡 Networks** - Lista de redes encontradas
3. **⚔️ Attacks** - Controle de ataques
4. **📈 Stats** - Estatísticas detalhadas
5. **🖥️ Acesso Remoto** - VNC para controlar a tela
6. **⚙️ Config** - Configurações remotas
7. **📤 OTA** - Atualização de firmware (em Ferramentas)

### WebSocket
Dados atualizados em tempo real sem refresh!

---

## 🎙️ Comandos de Voz

### Ativação
Diga **"Hey Dragon"** para ativar.

### Comandos Suportados

| Comando | Ação |
|---------|------|
| "Scan WiFi" | Inicia varredura |
| "Start Attack" | Inicia ataque selecionado |
| "Stop" | Para operação atual |
| "Status" | Fala status atual |
| "BLE Spam" | Liga/desliga BLE spam |
| "Beacon Flood" | Liga/desliga beacon |
| "Deauth" | Inicia deauth |
| "Help" | Lista comandos |
| "Sleep" | Modo economia |
| "Wake" | Despertar |

### Respostas
O mascote responde com voz sintetizada (TTS).

---

## 💡 Dicas Avançadas

### 1. Maximizar Captura de Handshakes
- Foque em redes com clientes ativos
- Use deauth em ráfagas curtas (2-3 pacotes)
- Posicione-se entre AP e cliente

### 2. Economizar Bateria
- Reduza brilho para 50%
- Desative wifi scan contínuo
- Use sleep entre operações

### 3. Ser Discreto
- Desative sons e vibração
- Use beacon flood com cuidado
- Evil Twin é facilmente detectável

### 4. Melhorar NEURA9
- Colete dados reais
- Treine modelo customizado
- Ajuste thresholds

### 5. Backup de Capturas
- Copie regularmente do SD
- Use nomenclatura clara
- Mantenha logs organizados

---

## 📞 Suporte
- 📖 Documentação: `/docs/`
- 🐛 Issues: GitHub

---

**Última Atualização:** 2025-12-09  
**Versão:** 2.2.0  
**Autor:** Comunidade LeleWatch

---


---

# ❓ FAQ - Perguntas Frequentes

## 🎯 Geral

### O que é o LeleWatch?
LeleWatch é um framework de segurança wireless ofensiva para o hardware Waveshare ESP32-S3-Touch-AMOLED-1.8, inspirado no Pwnagotchi. Combina ataques WiFi/BLE com IA embarcada para detecção de ameaças.

### É legal usar o LeleWatch?
⚠️ **Apenas em redes que você possui ou tem autorização explícita por escrito.** Uso não autorizado é ilegal e pode resultar em penalidades criminais.

---

## 🔧 Hardware

### Qual hardware é suportado?
Apenas o **Waveshare ESP32-S3-Touch-AMOLED-1.8**:
- ESP32-S3 N16R8 (16MB Flash, 8MB PSRAM)
- Display AMOLED 1.8" 368x448
- Touch FT3168
- IO Expander TCA9554

### Posso usar outro ESP32-S3?
O código é específico para este hardware. Adaptar requer modificações significativas nos drivers de display (SH8601) e touch (FT3168).

---

## 🔨 Compilação e Erros

### Erro: "DRAM overflow"
**Solução:** O tensor_arena foi movido para PSRAM na v2.1.0. Atualize para a versão mais recente.

### Erro: "WiFiClientSecure.h not found"
**Solução:** Adicione `WiFiClientSecure` no `platformio.ini`.

### Compilação demora muito
Normal. A compilação completa leva ~2-3 minutos na primeira vez.

---

## 📡 Ataques e IA

### Por que o deauth não funciona?
1. Verifique o canal.
2. Alguns APs modernos protegem contra quadros de gerenciamento (PMF).
3. Distância do alvo.

### Handshake não é capturado
1. O cliente alvo deve se reconectar durante o ataque.
2. Use deauth para forçar desconexão momentânea.
3. Compatível apenas com WPA/WPA2.

### Beacon flood trava o dispositivo?
Reduza o número de SSIDs fake ou aumente o intervalo entre beacons.

### O que a NEURA9 detecta?
Detecta 10 categorias como SAFE, CROWDED, DEAUTH_DETECTED, EVIL_TWIN_RISK, etc. Treinamento customizado é possível (veja `AI_TRAINING.md`).

---

## 🔧 Problemas Comuns

### Display não liga
1. Verifique IO Expander TCA9554.
2. Confirme driver SH8601.

### Touch não responde
1. Touch é FT3168 @ 0x38.
2. Verifique I2C (SDA=15, SCL=14).

### "AsyncTCP failed to start task"
**Solução:** Atualize para SDK v2.1.0+ e use PSRAM.

### Device reinicia sozinho
**Causas:** Watchdog timeout (loop travado) ou brownout (cabo USB ruim).

---

