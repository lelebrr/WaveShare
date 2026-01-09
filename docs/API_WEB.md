<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/API-REST-blue?style=for-the-badge" alt="API"/>
  <img src="https://img.shields.io/badge/WebSocket-Realtime-green?style=for-the-badge" alt="WebSocket"/>
  <img src="https://img.shields.io/badge/Port-80-orange?style=for-the-badge" alt="Port"/>
</p>

---

# 🌐 API WEB - WavePwn v2.1.0

## 📋 Visão Geral

O WavePwn expõe uma API REST e WebSocket para controle remoto e monitoramento em tempo real.

**Base URL:** `http://192.168.4.1`  
**WebSocket:** `ws://192.168.4.1/ws`

---

## 🔐 Autenticação

| Método | Descrição |
|--------|-----------|
| **AP WiFi** | SSID: `WavePwn`, Senha: `wavepwn` |

---

## 📡 Endpoints REST

### 📊 Status & Informações

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `GET` | `/api/status` | Status geral do dispositivo |
| `GET` | `/api/info` | Informações do sistema |
| `GET` | `/api/stats` | Estatísticas de sessão |
| `GET` | `/api/battery` | Status da bateria |

#### Exemplo: GET /api/status
```json
{
  "status": "online",
  "uptime": 3600,
  "wifi_channel": 6,
  "networks_seen": 42,
  "handshakes": 5,
  "neura9_status": "SAFE",
  "battery": 85,
  "charging": false
}
```

---

### 📡 WiFi

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `GET` | `/api/wifi/networks` | Lista redes encontradas |
| `POST` | `/api/wifi/scan` | Inicia novo scan |
| `POST` | `/api/wifi/deauth` | Inicia deauth attack |
| `POST` | `/api/wifi/beacon` | Inicia beacon flood |
| `POST` | `/api/wifi/stop` | Para ataque atual |

#### Exemplo: POST /api/wifi/deauth
```json
{
  "target_bssid": "AA:BB:CC:DD:EE:FF",
  "channel": 6,
  "duration": 30
}
```

---

### 📶 BLE

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `GET` | `/api/ble/devices` | Lista dispositivos BLE |
| `POST` | `/api/ble/spam/start` | Inicia BLE spam |
| `POST` | `/api/ble/spam/stop` | Para BLE spam |

#### Exemplo: POST /api/ble/spam/start
```json
{
  "type": "apple",
  "interval": 100
}
```

**Tipos de spam:** `apple`, `android`, `windows`, `all`

---

### 🧠 NEURA9 AI

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `GET` | `/api/neura9/status` | Status da IA |
| `GET` | `/api/neura9/classify` | Última classificação |
| `POST` | `/api/neura9/sensitivity` | Ajusta sensibilidade |

#### Exemplo: GET /api/neura9/classify
```json
{
  "class": 0,
  "class_name": "SAFE",
  "confidence": 0.95,
  "timestamp": 1702000000
}
```

---

### ⚙️ Configurações

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `GET` | `/api/config` | Configuração atual |
| `POST` | `/api/config` | Atualiza configuração |
| `POST` | `/api/config/reset` | Reset para padrão |

---

### 🐉 Mascote

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `GET` | `/api/mascot/list` | Lista mascotes |
| `GET` | `/api/mascot/current` | Mascote atual |
| `POST` | `/api/mascot/select` | Seleciona mascote |
| `POST` | `/api/mascot/rename` | Renomeia mascote |

---

### 📤 OTA & Sistema

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `POST` | `/api/ota/upload` | Upload de firmware |
| `POST` | `/api/reboot` | Reinicia dispositivo |
| `GET` | `/api/logs` | Últimos logs |

---

## 🔄 WebSocket

### Conexão
```javascript
const ws = new WebSocket('ws://192.168.4.1/ws');

ws.onmessage = (event) => {
  const data = JSON.parse(event.data);
  console.log('Received:', data);
};
```

### Eventos Recebidos

| Evento | Descrição |
|--------|-----------|
| `status_update` | Atualização de status |
| `network_found` | Nova rede encontrada |
| `handshake_captured` | Handshake capturado |
| `attack_started` | Ataque iniciado |
| `attack_stopped` | Ataque parado |
| `neura9_classification` | Nova classificação IA |
| `battery_update` | Atualização de bateria |

### Exemplo de Mensagem
```json
{
  "event": "handshake_captured",
  "data": {
    "ssid": "MyNetwork",
    "bssid": "AA:BB:CC:DD:EE:FF",
    "timestamp": 1702000000
  }
}
```

---

## 📁 Arquivos Estáticos

| Endpoint | Descrição |
|----------|-----------|
| `/` | Dashboard principal |
| `/index.html` | Página inicial |
| `/style.css` | Estilos |
| `/script.js` | JavaScript |
| `/attacks.html` | Página de ataques |
| `/settings.html` | Configurações |
| `/logs.html` | Visualização de logs |

---

## 🔧 Códigos de Resposta

| Código | Descrição |
|--------|-----------|
| `200` | Sucesso |
| `400` | Requisição inválida |
| `404` | Não encontrado |
| `500` | Erro interno |

---

**Última Atualização:** 2025-12-08  
**Versão:** 1.0
