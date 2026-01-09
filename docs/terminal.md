# LeleWatch Terminal - Documentação Completa

Terminal interativo do LeleWatch com **100+ comandos** organizados em 10 categorias.

---

## Atalhos de Teclado

| Tecla | Função |
|-------|--------|
| `Enter` | Executar comando |
| `Tab` | Auto-completar |
| `↑` / `↓` | Navegar histórico |
| `Ctrl+L` | Limpar tela |

---

## Comandos por Categoria

### ⚔️ WiFi Attacks (19 comandos)

| Comando | Descrição |
|---------|-----------|
| `scan` | Escanear redes WiFi |
| `networks` | Listar redes encontradas |
| `clients` | Listar clientes conectados |
| `deauth [bssid]` | Deauth flood em rede específica ou broadcast |
| `beacon [count]` | Beacon spam (default: 50 redes falsas) |
| `handshake` | Capturar handshake WPA2/WPA3 |
| `pmkid` | Capturar PMKID (sem cliente) |
| `evil_twin [ssid]` | Clonar rede para capturar credenciais |
| `karma` | Responder a todos os probe requests |
| `probe_flood` | Inundar espectro com probe requests |
| `auth_flood` | Flood de requisições de autenticação |
| `deassoc` | Forçar desassociação de clientes |
| `channel_hop` | Alternar canais para monitoramento |
| `hidden_reveal` | Descobrir SSIDs ocultos |
| `wps_pixie` | WPS Pixie Dust attack |
| `wps_brute` | WPS PIN brute-force |
| `hcxdump` | Modo caçador HCX agressivo |
| `channel [ch]` | Mudar/ver canal atual |
| `stop_wifi` | Parar todos os ataques WiFi |

### 📱 BLE Attacks (12 comandos)

| Comando | Descrição |
|---------|-----------|
| `ble_scan` | Escanear dispositivos BLE |
| `ble_spam [type]` | BLE spam (apple/windows/samsung/google) |
| `ble_apple` | Spam Apple AirPods |
| `ble_windows` | Spam Windows Swift Pair |
| `ble_samsung` | Spam Samsung Buds |
| `ble_google` | Spam Google Fast Pair |
| `ble_airtag` | Spam AirTag falso |
| `ble_skimmer` | Detectar skimmers Bluetooth |
| `ble_tracker` | Caçar AirTags/trackers ocultos |
| `ble_clone` | Clonar dispositivo BLE |
| `ble_flood` | Flood de conexões BLE |
| `stop_ble` | Parar ataques BLE |

### 🖥️ Sistema (17 comandos)

| Comando | Descrição |
|---------|-----------|
| `status` | Status completo do sistema |
| `reboot` | Reiniciar dispositivo (3s delay) |
| `uptime` | Tempo ligado |
| `heap` | Memória heap livre |
| `psram` | Status PSRAM |
| `temp` | Temperatura CPU |
| `battery` | Nível da bateria |
| `version` | Versão do firmware |
| `mac` | Endereços MAC WiFi/BLE |
| `ip` | Endereço IP atual |
| `hostname` | Nome do host |
| `cpu_freq` | Frequência da CPU |
| `sdk_version` | Versão do SDK ESP |
| `factory_reset` | Reset para configurações de fábrica |
| `backup` | Fazer backup das configurações |
| `restore` | Restaurar backup |
| `deep_sleep` | Entrar em modo deep sleep |

### 📁 Arquivos (13 comandos)

| Comando | Descrição |
|---------|-----------|
| `ls [dir]` | Listar arquivos |
| `cd [dir]` | Mudar diretório |
| `pwd` | Diretório atual |
| `cat [arquivo]` | Ver conteúdo de arquivo |
| `rm [arquivo]` | Remover arquivo |
| `mkdir [nome]` | Criar pasta |
| `touch [nome]` | Criar arquivo vazio |
| `mv [src] [dst]` | Mover arquivo |
| `cp [src] [dst]` | Copiar arquivo |
| `df` | Espaço em disco |
| `du [dir]` | Uso de disco |
| `head [arquivo]` | Primeiras linhas |
| `tail [arquivo]` | Últimas linhas |

### 🌐 Rede (12 comandos)

| Comando | Descrição |
|---------|-----------|
| `ping [host]` | Ping para host |
| `traceroute [host]` | Traceroute para host |
| `dns [domain]` | Resolver DNS |
| `arp` | Tabela ARP |
| `netstat` | Conexões ativas |
| `ifconfig` | Configuração de rede |
| `route` | Tabela de rotas |
| `nslookup [domain]` | DNS lookup |
| `whois [domain]` | WHOIS lookup |
| `port_scan [ip]` | Scan de portas |
| `http_get [url]` | HTTP GET request |
| `wifi_connect [ssid] [pass]` | Conectar a rede WiFi |

### 📺 IR Control (8 comandos)

| Comando | Descrição |
|---------|-----------|
| `ir_send [nome]` | Enviar código IR |
| `ir_learn` | Aprender código IR do controle original |
| `ir_list` | Listar códigos salvos |
| `ir_delete [nome]` | Deletar código |
| `ir_nuke` | TV NUKE - desligar todas as TVs |
| `ir_test` | Testar emissor IR |
| `ir_raw [hex]` | Enviar código raw |
| `ir_power [marca]` | Só o botão power |

### 📷 Câmeras (6 comandos)

| Comando | Descrição |
|---------|-----------|
| `cam_scan` | Escanear câmeras IP na rede |
| `cam_attack [ip]` | Atacar câmera específica |
| `cam_rtsp [ip]` | Capturar stream RTSP |
| `cam_onvif` | ONVIF discovery |
| `cam_creds [ip]` | Testar credenciais padrão |
| `cam_screenshot [ip]` | Capturar screenshot da câmera |

### 🔊 Som (7 comandos)

| Comando | Descrição |
|---------|-----------|
| `play [nome]` | Tocar som do SD |
| `say [texto]` | Text-to-speech |
| `volume [0-100]` | Ajustar volume |
| `mute` | Silenciar |
| `beep` | Emitir beep |
| `sounds` | Listar sons disponíveis |
| `song [nome]` | Tocar música |

### 🎮 Fun & Easter Eggs (17 comandos)

| Comando | Descrição |
|---------|-----------|
| `matrix` | Ativar modo Matrix (tema hacker) |
| `dragon [msg]` | Fazer o dragão falar |
| `whoami` | Informações do sistema estilizadas |
| `cowsay [msg]` | ASCII cow diz a mensagem |
| `fortune` | Frase aleatória hacker |
| `ascii_art [nome]` | Mostrar arte ASCII (dragon/skull) |
| `banner` | Mostrar banner LeleWatch |
| `rainbow [msg]` | Texto colorido arco-íris |
| `hack` | Simular hacking cinematográfico |
| `neofetch` | Info do sistema estilo neofetch |
| `sl` | Steam Locomotive 🚂 |
| `rickroll` | Never gonna give you up... |
| `lele` | Invocar mascote Lele |
| `lisa` | Invocar mascote Lisa |
| `dance` | Emojis de dança |
| `fire` | Mostrar fogo 🔥 |
| `explosion` | Explosão 💥 |
| `pirate` | Modo pirata |

### ⚙️ Geral (20 comandos)

| Comando | Descrição |
|---------|-----------|
| `help` | Lista todos os comandos por categoria |
| `clear` / `cls` | Limpar terminal |
| `history` | Ver histórico de comandos |
| `alias` | Ver aliases disponíveis |
| `export` | Exportar variável |
| `env` | Variáveis de ambiente |
| `echo [msg]` | Imprimir mensagem |
| `date` | Data e hora atual |
| `time` | Hora atual |
| `theme [nome]` | Mudar tema da interface |
| `stop` | Parar todos os ataques |
| `pwn` | PWN EVERYTHING - todos os ataques |
| `cinema` | Modo cinema fullscreen |
| `credits` | Créditos do projeto |
| `about` | Sobre o LeleWatch |
| `exit` | Voltar ao dashboard |
| `man [cmd]` | Manual do comando |
| `locate [arquivo]` | Localizar arquivo |
| `grep [padrão] [arquivo]` | Buscar padrão |
| `find [nome]` | Encontrar arquivos |
| `top` | Processos (simplificado) |
| `kill [nome]` | Matar processo |
| `watch_led` | Piscar LED do dispositivo |
| `watch_vibrate` | Vibrar dispositivo |
| `screenshot` | Capturar tela do dispositivo |

---

## Exemplos de Uso

```bash
# Escanear redes e atacar
$ scan
$ deauth FF:FF:FF:FF:FF:FF

# Capturar handshake
$ handshake

# BLE spam Apple
$ ble_apple

# Ver arquivos no SD
$ ls /handshakes

# Desligar todas as TVs
$ ir_nuke

# Fazer o dragão falar
$ dragon Você foi hackeado!
```

---

## Temas Disponíveis

- `cyberpunk` - Neon azul/rosa
- `hacker` - Matrix verde
- `stealth` - Modo escuro discreto
- `light` - Modo claro
- `dracula` - Roxo escuro
- `nord` - Azul gelo
- `monokai` - Editor classico

---

## Notas

- Comandos são case-insensitive
- Use `man [comando]` para ver detalhes de qualquer comando
- O histórico persiste durante a sessão
- Alguns comandos requerem conexão com o dispositivo ESP32
