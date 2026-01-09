/**
 * @file pwnagotchi.cpp
 * @brief Classe principal do WavePwn - orquestra todos os subsistemas
 *
 * Integra:
 * - Hardware (Display, Touch, PMU, Sensores)
 * - LVGL interface
 * - WiFi (scan, ataques)
 * - BLE (scan, spam)
 * - Web interface
 * - Mascote
 */

#include "pwnagotchi.h"
#include "../core/config.h"
#include "../core/globals.h"
#include "../hardware/lvgl_driver.h"
#include "../ui/ui_attacks.h"
#include "../ui/ui_main.h"
#include "../wifi/wifi_attacks.h"
#include "../wifi/wps_attacks.h"

// Flags de controle
static bool lvgl_ready = false;
static bool ui_ready = false;

Pwnagotchi::Pwnagotchi()
    : _mascot(nullptr), _lastUpdate(0), _lastScan(0), _isScanning(false) {
  // Set defaults
  g_state.scan_time_ms = 30000;
  g_state.mascot_enabled = true;
}

void Pwnagotchi::begin() {
  Serial.println("\n");
  Serial.println("╔══════════════════════════════════════════╗");
  Serial.println("║      WavePwn v1.0 - Iniciando...         ║");
  Serial.println("╚══════════════════════════════════════════╝");
  Serial.println();

  // 1. Hardware Init (Display, Touch, PMU, Sensores)
  Serial.println("[1/7] Inicializando Hardware...");
  if (!sys_hw.init()) {
    Serial.println("[PWN] ⚠ ERRO DE HARDWARE!");
  } else {
    Serial.println("[PWN] ✓ Hardware OK");
  }

  // Panic Button Init
  pinMode(BTN_USER, INPUT_PULLUP);

  // 2. LVGL Init
  Serial.println("[2/7] Inicializando LVGL...");
  if (sys_hw.getDisplay()) {
    if (lvgl_driver_init(sys_hw.getDisplay())) {
      lvgl_ready = true;
      Serial.println("[PWN] ✓ LVGL OK");
    } else {
      Serial.println("[PWN] ⚠ LVGL falhou");
    }
  }

  // 3. UI Principal
  Serial.println("[3/7] Inicializando Interface...");
  if (lvgl_ready) {
    if (ui_main_init()) {
      ui_ready = true;
      Serial.println("[PWN] ✓ UI OK");
    } else {
      Serial.println("[PWN] ⚠ UI falhou");
    }
  }

  // 4. Mascot (fallback se LVGL não estiver pronto)
  if (!ui_ready && sys_hw.getDisplay()) {
    Serial.println("[4/7] Inicializando Mascote (modo GFX)...");
    _mascot = new DragonMascot(sys_hw.getDisplay());
    Serial.println("[PWN] ✓ Mascote OK");
  } else {
    Serial.println("[4/7] Mascote integrado na UI LVGL");
  }

  // 5. WiFi
  Serial.println("[5/7] Inicializando WiFi...");
  wifi_driver.begin();

  // Start the Access Point to ensure LwIP is ready for the Web Server
  if (wifi_driver.startAP(WIFI_AP_SSID, WIFI_AP_PASSWORD, WIFI_AP_CHANNEL)) {
    Serial.println("[PWN] AP Iniciado: " WIFI_AP_SSID);
  } else {
    Serial.println("[PWN] Falha ao iniciar AP!");
  }

  wifi_attacks.begin();
  g_state.wifi_enabled = true;
  Serial.println("[PWN] ✓ WiFi OK");

  // 6. BLE
  Serial.println("[6/7] Inicializando BLE...");
  // BLE agora é iniciado sob demanda para economizar RAM
  // ble_driver.begin(DRAGON_NAME);
  g_state.ble_enabled = false;
  Serial.println("[PWN] ✓ BLE em Standby (economia de RAM)");

  // 7. Web Interface
  Serial.println("[7/7] Inicializando Web Server...");
  web_interface.begin();
  Serial.println("[PWN] ✓ Web Server OK");

  // Mensagem final
  Serial.println();
  Serial.println("╔══════════════════════════════════════════╗");
  Serial.println("║         Sistema Pronto!                  ║");
  Serial.printf("║  RAM: %d KB livres                       ║\n",
                ESP.getFreeHeap() / 1024);
  Serial.printf("║  PSRAM: %d KB livres                     ║\n",
                ESP.getFreePsram() / 1024);
  Serial.println("╚══════════════════════════════════════════╝");
  Serial.println();

  // Splash screen se UI não estiver pronta
  if (!ui_ready && sys_hw.getDisplay()) {
    sys_hw.getDisplay()->fillScreen(BLACK);
    sys_hw.getDisplay()->setCursor(10, 10);
    sys_hw.getDisplay()->setTextColor(GREEN);
    sys_hw.getDisplay()->setTextSize(2);
    sys_hw.getDisplay()->println("WavePwn v1.0");
    sys_hw.getDisplay()->setTextColor(WHITE);
    sys_hw.getDisplay()->println("Sistema Pronto!");
    delay(1500);
    sys_hw.getDisplay()->fillScreen(BLACK);
  }
}

void Pwnagotchi::loop() {
  unsigned long now = millis();

  // 1. LVGL Tasks (se ativo)
  if (lvgl_ready) {
    lvgl_driver_loop();
  }

  // 2. BLE Spam (se ativo)
  if (g_state.ble_enabled) {
    ble_driver.updateSpam();
  }

  // 3. WiFi Attacks (se ativo)
  if (wifi_attacks.isActive()) {
    wifi_attacks.update();
  }

  // 4. Auto-scan WiFi (a cada 30s)
  // 4. Auto-scan WiFi
  if (g_state.wifi_enabled && !wifi_attacks.isActive() &&
      (now - _lastScan > g_state.scan_time_ms)) {
    _lastScan = now;
    performWiFiScan();
  }

  // 4a. Mascot Mood Logic
  static unsigned long last_mood = 0;
  static uint32_t last_hs = 0;

  if (now - last_mood > 5000) { // Check every 5s
    if (g_state.handshakes_captured > last_hs) {
      ui_set_mascot_face(MASCOT_FACE_HAPPY);
      ui_set_mood_text("Handshake! \nComendo chaves... 🔑");
      last_hs = g_state.handshakes_captured;
      last_mood = now + 5000; // Keep happy longer
    } else if (wifi_attacks.isActive()) {
      ui_set_mascot_face(MASCOT_FACE_ANGRY);
      ui_set_mood_text("Destruindo redes... 🔥");
    } else if (g_state.networks_seen == 0 && _isScanning) {
      ui_set_mascot_face(MASCOT_FACE_CONFUSED);
      ui_set_mood_text("Procurando WiFi... 👀");
    } else if (g_state.networks_seen > 0) {
      ui_set_mascot_face(MASCOT_FACE_NORMAL);
      ui_set_mood_text("Escaneando o éter... 🐲");
    } else {
      ui_set_mascot_face(MASCOT_FACE_SLEEP);
      ui_set_mood_text("Zzz... Silêncio no ar...");
    }
    last_mood = now;
  }

  // 5. Mascot update (modo GFX, se não usar LVGL)
  if (!ui_ready && _mascot) {
    _mascot->update();
    _mascot->draw(110, 100);

    // Stats display
    if (now % 1000 < 50) {
      updateStatsDisplay();
    }
  }

  // 6. Core Logic (mood, etc)
  updateLogic();

  // 7. Input Handling
  processInputs();

  // 8. WiFi driver update
  wifi_driver.update();

  // 9. Check Scan Results
  checkScanResults();
}

void Pwnagotchi::performWiFiScan() {
  Serial.println("[PWN] Iniciando scan WiFi...");

  if (_mascot)
    _mascot->setState(DRAGON_SCANNING);
  if (ui_ready)
    ui_set_mood_text("Procurando redes...");

  int n = wifi_driver.scanNetworks(true);

  if (n == WIFI_SCAN_RUNNING || n == -1) {
    _isScanning = true;
  } else {
    // Error or sync result (unexpected with async=true)
    _isScanning = false;
  }
}

void Pwnagotchi::checkScanResults() {
  if (!_isScanning)
    return;

  int n = wifi_driver.getNetworkCount();
  if (n >= 0) {
    _isScanning = false;
    g_state.networks_seen = n;
    Serial.printf("[PWN] Encontradas %d redes\n", n);

    if (_mascot)
      _mascot->setState(n > 5 ? DRAGON_EXCITED : DRAGON_HAPPY);

    if (ui_ready) {
      char buf[32];
      snprintf(buf, sizeof(buf), "Encontradas %d redes!", n);
      ui_set_mood_text(buf);
      ui_set_mood_text(buf);
      ui_set_mascot_face(MASCOT_FACE_HAPPY);
      // Refresh Attacks UI list
      if (ui_attacks_is_active()) {
        ui_attacks_refresh();
      }
    }

    // Reset mode to AP if it was AP?
    // Usually scan keeps us in STA or AP_STA.
    // But if we were AP, we are now AP_STA. This is fine.
  } else if (n == -2) {
    // Failed
    _isScanning = false;
    Serial.println("[PWN] Scan falhou");
    if (ui_ready)
      ui_set_mood_text("Scan falhou!");
  }
}

void Pwnagotchi::updateStatsDisplay() {
  if (!sys_hw.getDisplay())
    return;

  sys_hw.getDisplay()->fillRect(0, 0, LCD_WIDTH, 40, BLACK);
  sys_hw.getDisplay()->setCursor(5, 5);
  sys_hw.getDisplay()->setTextColor(WHITE);
  sys_hw.getDisplay()->setTextSize(2);
  sys_hw.getDisplay()->printf("NET:%d BAT:%d%%", g_state.networks_seen,
                              sys_hw.getBatteryPercent());
}

void Pwnagotchi::updateLogic() {
  // Update global state
  g_state.uptime_seconds = millis() / 1000;
  g_state.battery_percent = sys_hw.getBatteryPercent();
  g_state.is_charging = sys_hw.isCharging();

  // BLE stats
  if (g_state.ble_enabled) {
    // Atualizado pelo driver
  }

  // WiFi attack stats
  if (wifi_attacks.isActive()) {
    AttackStats stats = wifi_attacks.getStats();
    g_state.handshakes_captured = stats.handshakes_captured;
  }

  // Mascot mood (se não estiver usando UI LVGL)
  if (_mascot) {
    if (sys_hw.getBatteryPercent() < 20) {
      _mascot->setState(DRAGON_SLEEPING);
    } else if (g_state.handshakes_captured > 0) {
      _mascot->setState(DRAGON_EXCITED);
    } else if (g_state.networks_seen > 10) {
      _mascot->setState(DRAGON_EXCITED);
    } else if (g_state.networks_seen > 0) {
      _mascot->setState(DRAGON_HAPPY);
    } else {
      _mascot->setState(DRAGON_IDLE);
    }
  }
}

void Pwnagotchi::processInputs() {
  // Web clients
  web_interface.update();

  // Touch handling (se não estiver usando LVGL)
  if (!lvgl_ready) {
    SystemHardware::TouchPoint tp = sys_hw.getTouch();
    if (tp.touched) {
      Serial.printf("[PWN] Touch: %d, %d\n", tp.x, tp.y);
      if (_mascot)
        _mascot->setState(DRAGON_EXCITED);
    }
  }

  // Panic Button Handling (Tip 10 variant)
  if (g_state.panic_button_enabled) {
    static uint32_t press_start = 0;
    static bool pressed = false;

    // Logic for GPIO 47 (BTN_USER) - Active LOW
    if (digitalRead(BTN_USER) == LOW) {
      if (!pressed) {
        pressed = true;
        press_start = millis();
      } else {
        if ((millis() - press_start) > (g_state.panic_button_duration * 1000)) {
          // PANIC ACTION!
          Serial.println("[PANIC] BUTTON HELD! STOPPING EVERYTHING!");
          stopAllAttacks();
          if (ui_ready) {
            ui_set_mood_text("!!! PANIC !!!");
            ui_set_mascot_face(MASCOT_FACE_ANGRY); // Panic face
          }
          // Optional: Reset?
          // ESP.restart();
          pressed = false; // Reset to avoid loop
        }
      }
    } else {
      pressed = false;
    }
  }

  // Se LVGL está ativo, o touch é processado pelo input driver do LVGL
}

// === Funções de Controle ===

void Pwnagotchi::startDeauthAttack(const uint8_t *bssid, const char *ssid,
                                   uint8_t channel) {
  Serial.printf("[PWN] Iniciando Deauth em %s\n", ssid);
  wifi_attacks.setTarget(bssid, ssid, channel);
  wifi_attacks.startDeauth(100);
  if (ui_ready)
    ui_set_mood_text("Deauth ATIVO!");
  ui_set_mascot_face(MASCOT_FACE_ANGRY);
  if (_mascot)
    _mascot->setState(DRAGON_ANGRY);
}

void Pwnagotchi::startBeaconFlood() {
  Serial.println("[PWN] Iniciando Beacon Flood");
  wifi_attacks.startBeaconFlood(nullptr, 0);
  if (ui_ready)
    ui_set_mood_text("Beacon Flood!");
  if (_mascot)
    _mascot->setState(DRAGON_EXCITED);
}

void Pwnagotchi::startHandshakeCapture(const uint8_t *bssid, const char *ssid,
                                       uint8_t channel) {
  Serial.printf("[PWN] Capturando handshakes de %s\n", ssid);
  wifi_attacks.setTarget(bssid, ssid, channel);
  wifi_attacks.startHandshakeCapture();
  if (ui_ready)
    ui_set_mood_text("Caçando handshakes...");
  if (_mascot)
    _mascot->setState(DRAGON_SCANNING);
}

void Pwnagotchi::startBLESpam(BLESpamType type) {
  g_state.ble_enabled = true;

  // Tip 24: Conditional Initialization (Lazy Init)
  // Ensure BLE is initialized before starting spam
  ble_driver.begin("WavePwn");

  ble_driver.startSpam(type);

  const char *type_name = "Todos";
  switch (type) {
  case BLE_SPAM_APPLE:
    type_name = "Apple";
    break;
  case BLE_SPAM_SAMSUNG:
    type_name = "Samsung";
    break;
  case BLE_SPAM_WINDOWS:
    type_name = "Windows";
    break;
  case BLE_SPAM_GOOGLE:
    type_name = "Google";
    break;
  default:
    break;
  }

  Serial.printf("[PWN] BLE Spam iniciado: %s\n", type_name);
  if (ui_ready) {
    char buf[32];
    snprintf(buf, sizeof(buf), "BLE Spam: %s", type_name);
    ui_set_mood_text(buf);
  }
}

void Pwnagotchi::stopAllAttacks() {
  wifi_attacks.stopAttack();
  wps_attacks.stopAttack();
  ble_driver.stopSpam();
  g_state.ble_enabled = false;
  Serial.println("[PWN] Todos os ataques parados");
  if (ui_ready)
    ui_set_mood_text("Pronto para caçar!");
  if (_mascot)
    _mascot->setState(DRAGON_IDLE);
}

String Pwnagotchi::getMoodString() {
  if (wifi_attacks.isActive())
    return "Atacando";
  if (g_state.ble_enabled)
    return "BLE Spam";
  if (g_state.handshakes_captured > 0)
    return "Feliz++";
  if (g_state.networks_seen > 10)
    return "Animado";
  if (g_state.networks_seen > 0)
    return "Feliz";
  if (sys_hw.getBatteryPercent() < 20)
    return "Cansado";
  return "Pronto";
}

bool Pwnagotchi::isUIReady() { return ui_ready; }

// === WPS Attack Control Methods ===

void Pwnagotchi::startWPSPixieDust(const uint8_t *bssid, const char *ssid,
                                   uint8_t channel) {
  Serial.printf("[PWN] Iniciando Pixie Dust em %s\n", ssid);
  wps_attacks.setTarget(bssid, ssid, channel);
  wps_attacks.startPixieDust();
  if (ui_ready)
    ui_set_mood_text("Pixie Dust ATIVO!");
  if (_mascot)
    _mascot->setState(DRAGON_ANGRY);
}

void Pwnagotchi::startWPSBruteforce(const uint8_t *bssid, const char *ssid,
                                    uint8_t channel) {
  Serial.printf("[PWN] Iniciando WPS Brute-force em %s\n", ssid);
  wps_attacks.setTarget(bssid, ssid, channel);
  wps_attacks.startBruteforce(0);
  if (ui_ready)
    ui_set_mood_text("WPS Brute-force...");
  if (_mascot)
    _mascot->setState(DRAGON_SCANNING);
}

void Pwnagotchi::startWPSFlood(const uint8_t *bssid, const char *ssid,
                               uint8_t channel) {
  Serial.printf("[PWN] Iniciando WPS PIN Flood em %s\n", ssid);
  wps_attacks.setTarget(bssid, ssid, channel);
  wps_attacks.startPINFlood();
  if (ui_ready)
    ui_set_mood_text("WPS Flood DoS!");
  if (_mascot)
    _mascot->setState(DRAGON_ANGRY);
}
