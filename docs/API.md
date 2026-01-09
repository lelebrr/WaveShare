<!-- Badges -->
<p align="center">
  <img src="https://img.shields.io/badge/API-REST-blue?style=for-the-badge" alt="API"/>
  <img src="https://img.shields.io/badge/WebSocket-Realtime-green?style=for-the-badge" alt="WebSocket"/>
  <img src="https://img.shields.io/badge/Port-80-orange?style=for-the-badge" alt="Port"/>
</p>

---

# 🌐 API WEB - LeleWatch v2.2.0

## 📋 Visão Geral

O LeleWatch expõe uma API REST e WebSocket para controle remoto e monitoramento em tempo real.

**Base URL:** `http://192.168.4.1`  
**WebSocket:** `ws://192.168.4.1/ws`

---

## 🔐 Autenticação

| Método | Descrição |
|--------|-----------|
| **AP WiFi** | SSID: `LeleWatch`, Senha: `lelewatch` |

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
| `GET` | `/api/ble/feed` | Feed detalhado com classificação |
| `POST` | `/api/ble/spam/start` | Inicia BLE spam |
| `POST` | `/api/ble/spam/stop` | Para BLE spam |

#### Exemplo: GET /api/ble/feed
```json
{
  "devices": [
    {
      "name": "AirPods Pro",
      "addr": "AA:BB:CC:DD:EE:FF",
      "rssi": -45,
      "manuf": "4C00...",
      "manuf_name": "Apple",
      "type": 3,
      "distance_m": 1.2
    }
  ],
  "count": 1
}
```

**Tipos de dispositivo (type):**  
`0`=Unknown, `1`=Phone, `2`=Headphones, `3`=Watch, `4`=Speaker, `5`=Beacon, `6`=Tracker, `7`=Computer, `8`=Keyboard, `9`=Mouse, `10`=Gamepad, `11`=TV, `12`=Car, `13`=Medical, `14`=Fitness

**Tipos de spam:** `apple`, `samsung`, `windows`, `google`, `rickroll`, `chaos`, `airtag_flood`, `easter_egg`

---

### 💾 SD Card Manager

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `GET` | `/sd_json` | Lista arquivos (raiz) |
| `GET` | `/sd_json?path=/pasta` | Lista arquivos no diretório |
| `GET` | `/sd_download?file=/path` | Download de arquivo |
| `GET` | `/sd_delete?file=/path` | Deleta arquivo ou pasta |
| `POST` | `/sd_upload?filename=nome.txt` | Upload de arquivo |

#### Exemplo: GET /sd_json?path=/captures
```json
{
  "online": true,
  "type": "SDHC/SDXC",
  "speed": 40,
  "total_mb": 30000,
  "free_mb": 25000,
  "current_path": "/captures",
  "files": [
    {"name": "handshake.pcap", "path": "/captures/handshake.pcap", "size": 1024, "is_dir": false},
    {"name": "pmkid", "path": "/captures/pmkid", "size": 0, "is_dir": true}
  ]
}
```

**Interface Web:** `http://192.168.4.1/sd_manager.html`

---

### 📁 LittleFS / Wallpapers

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `GET` | `/api/files/list` | Lista arquivos na raiz (JSON) |
| `GET` | `/api/fs/status` | Uso do LittleFS (Total/Used) |
| `GET` | `/api/wallpapers/list` | Lista wallpapers disponíveis no SD |

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
| `GET` | `/api/security/panic` | Ativa Panic Button (Lock) |

---

### 📺 IR Remote (Controle Universal)

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `GET` | `/ir_send?name=TV_POWER` | Envia comando IR salvo |
| `GET` | `/ir_nuke` | ☢️ **TV-B-Gone** (Desliga tudo) |
| `GET` | `/ir_list` | Lista controles salvos |
| `POST` | `/ir_learn` | Aprende novo código (Raw) |

---

### 🖥️ Acesso Remoto (VNC-Style)

| Método | Endpoint | Descrição |
|--------|----------|-----------|
| `GET` | `/stream` | Frame BMP da tela (para polling) |
| `GET` | `/touch?x=123&y=456` | Simula toque na tela |
| `GET` | `/remote.html` | Interface VNC no navegador |

#### Exemplo: Simular toque
```bash
curl "http://192.168.4.1/touch?x=184&y=224"
```

#### Resposta
```
OK
```

#### Streaming de Tela (JavaScript)
```javascript
// Polling-based frame update
setInterval(() => {
    document.getElementById('screen').src = '/stream?' + Date.now();
}, 120); // ~8 FPS
```

**Notas:**
- Frame retornado em formato BMP RGB565 (368x448 pixels)
- Coordenadas x/y devem estar entre 0-367 e 0-447 respectivamente
- Rate limiting: ~10 FPS máximo
- Autenticação: Basic Auth (admin/pwned)

---

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
| `sys_info` | CPU Temp, RAM Usage, Heap Free, SD Status |
| `mac_info` | Endereços MAC de WiFi e BLE |
| `channel_stats` | Distribuição de canais WiFi (Gráfico) |

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
| `/css/main.css` | Estilos Principais |
| `/js/app.js` | JavaScript Principal |
| `/remote.html` | Acesso remoto VNC-style |
| `/update.html` | Página de OTA Update |
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

**Última Atualização:** 2025-12-09  
**Versão:** 2.0
