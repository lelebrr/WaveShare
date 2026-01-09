# WavePwn - The Ultimate ESP32-S3 Wireless Pentest Framework

## Hardware: Waveshare ESP32-S3-Touch-AMOLED-1.8

### Guia Rápido de Funcionalidades

Este firmware transforma seu dispositivo em uma ferramenta completa de auditoria wireless.

#### 1. Mascote "WaveDragon"
O dragão na tela inicial não é apenas um desenho. Ele reage ao ambiente:
- **Dormindo:** Bateria fraca ou inatividade.
- **Feliz:** Redes WiFi encontradas.
- **Excitado/Bravo:** Interação via Touch ou ataque em andamento.

#### 2. Funcionalidades de Rede
- **WiFi Scan:** Varre redes automaticamente em segundo plano.
- **Monitor Mode:** Pronto para capturar pacotes (infraestrutura de injeção inclusa).
- **BLE Spam:** Capacidade de enviar beacons que geram popups em iOS, Android e Windows (inicia via código/menu).

#### 3. BadUSB
Conecte o dispositivo via USB a um PC.
- O dispositivo se comporta como um teclado HID.
- Payloads "DuckyScript" podem ser executados (ex: abrir CMD, Notepad).

#### 4. Interface Web
Conecte-se à rede WiFi `WavePwn` (Senha: `wavepwn123`).
Acesse `http://192.168.4.1` para ver o dashboard em tempo real.

### Como Compilar
Use PlatformIO. As bibliotecas necessárias estão configuradas no `platformio.ini`.

**Nota:** Este projeto é para fins educacionais e testes de segurança autorizados.
