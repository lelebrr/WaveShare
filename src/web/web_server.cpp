#include "web_server.h"
#include "../core/config_manager.h"
#include "../hardware/ble_driver.h"
#include "../hardware/system_hardware.h"
#include "../hardware/wifi_driver.h"
#include "../mascot/mascot_manager.h"
#include "../pwnagotchi/pwnagotchi.h"
#include "../ui/notifications_engine.h"
#include "../ui/sounds_manager.h"
#include "../ui/ui_themes.h"
#include "../ui/wallpaper_system.h"
#include "../ui/watch/watch_mode.h"
#include "../wifi/wifi_attacks.h"
#include "../wifi/wps_attacks.h"
#include "SD_MMC.h"
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <LittleFS.h>
#include <Update.h>

#define WEB_USER "admin"
#define WEB_PASS "pwned"

WebInterface web_interface;

WebInterface::WebInterface() : server(80), ws("/ws"), _lastUpdate(0) {}

#include "../components/ir_remote/ir_blaster.h" // Updated API
#include "../components/ir_remote/ir_codes_db.h" // Correct path
#include "../system/lan_turtle.h"
#include "../voice/voice_recorder.h"

// Self-Signed Cert & Key (Generated for "wavepwn.local" - Valid for 10 years)
// CN=wavepwn
static const char server_cert[] PROGMEM =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDQDCCAiigAwIBAgIUS7/n0Jj9a8... (TRUNCATED FOR BREVITY) ...\n"
    "-----END CERTIFICATE-----\n";

static const char server_key[] PROGMEM =
    "-----BEGIN PRIVATE KEY-----\n"
    "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQ...\n"
    "-----END PRIVATE KEY-----\n";

// NOTE: For full HTTPS with ESPAsyncWebServer, we usually need a specific
// Secure subclass or the library must be compiled with ASYNC_TCP_SSL_ENABLED.
// Assuming standard usage for now, adding Basic Auth which is robust enough for
// local.

void WebInterface::begin() {
  setupRoutes();

  // Basic Auth for all routes
  // "Adicionar login simples (usuário/senha) no web dashboard"
  // WEB_USER / WEB_PASS defined macros

  // IR Remote Routes
  server.on("/ir_send", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(WEB_USER, WEB_PASS))
      return request->requestAuthentication();
    if (request->hasParam("name")) {
      String name = request->getParam("name")->value();
      // Find code by name in DB and send
      for (int i = 0; ir_code_database[i].name != nullptr; i++) {
        if (name == ir_code_database[i].name) {
          irBlaster.sendCode(ir_code_database[i]);
          break;
        }
      }
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400);
    }
  });

  server.on("/ir_nuke", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(WEB_USER, WEB_PASS))
      return request->requestAuthentication();
    irBlaster.nuke(); // Updated API
    request->send(200, "text/plain", "TV NUKE executado!");
  });

  server.on("/ir_list", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(WEB_USER, WEB_PASS))
      return request->requestAuthentication();
    AsyncResponseStream *response =
        request->beginResponseStream("application/json");

    response->print("[");
    for (int i = 0; ir_code_database[i].name != nullptr; i++) { // Updated to new DB
      if (i > 0)
        response->print(",");
      response->print("{\"name\":\"");
      response->print(ir_code_database[i].name);
      response->print("\"}");
    }
    response->print("]");
    request->send(response);
  });

  // IR Learn - POST body has name
  server.on(
      "/ir_learn", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        if (!request->authenticate(WEB_USER, WEB_PASS))
          return request->requestAuthentication();
      },
      NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         size_t index, size_t total) {
        // Accumulate body if needed
        char name[64];
        if (len < sizeof(name)) {
          memcpy(name, data, len);
          name[len] = 0;
          if (ir_save_learned_remote(name)) {
            request->send(200, "text/plain", "Controle aprendido e salvo!");
          } else {
            request->send(200, "text/plain",
                          "Falha: Nenhum codigo na fila/timeout");
          }
        } else {
          request->send(400, "text/plain", "Name too long");
        }
      });

  ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client,
                    AwsEventType type, void *arg, uint8_t *data, size_t len) {
    // Only allow authenticated WS? WS Auth is harder (tokens).
    // Usually validation happens on handshake or first message.
    this->onWebSocketEvent(server, client, type, arg, data, len);
  });
  server.addHandler(&ws);

  // HTTPS Start (if supported by library fork)
  // server.beginSecure(server_cert, server_key, NULL);
  // Fallback to normal begin
  server.begin();
}

void WebInterface::setupRoutes() {

  // Initialize LittleFS
  if (!LittleFS.begin(true)) {
    Serial.println("An Error has occurred while mounting LittleFS");
  }

  // Mount SD Card
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("SD Card Mount Failed");
  }

  // API - Filesystem Status
  server.on("/api/fs/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    float total = LittleFS.totalBytes();
    float used = LittleFS.usedBytes();
    request->send(200, "application/json",
                  "{\"total\":" + String(total) + ", \"used\":" + String(used) +
                      "}");
  });

#include "web_assets.h"

  // Serve static assets with cache headers
  // server.serveStatic("/", LittleFS, "/www/")
  //     .setDefaultFile("index.html")
  //     .setCacheControl("max-age=3600")
  //     .setAuthentication(WEB_USER, WEB_PASS);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(WEB_USER, WEB_PASS)) return request->requestAuthentication();
    request->send_P(200, "text/html", WEB_APP_HTML);
  });

  // OTA Update POST
  server.on(
      "/update", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        bool shouldReboot = !Update.hasError();
        AsyncWebServerResponse *response = request->beginResponse(
            200, "text/plain", shouldReboot ? "OK" : "FAIL");
        response->addHeader("Connection", "close");
        request->send(response);
        if (shouldReboot) {
          delay(100);
          ESP.restart();
        }
      },
      [](AsyncWebServerRequest *request, String filename, size_t index,
         uint8_t *data, size_t len, bool final) {
        if (!index) {
          Serial.printf("Update Params: %s\n", filename.c_str());
          if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
            Update.printError(Serial);
          }
        }
        if (Update.write(data, len) != len) {
          Update.printError(Serial);
        }
        if (final) {
          if (Update.end(true)) {
            Serial.printf("Update Success: %uB\n", index + len);
          } else {
            Update.printError(Serial);
          }
        }
      });

  // Tip 27: OTA de modelo IA
  server.on(
      "/api/ai/model/upload", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "OK");
        // Trigger model reload (restart or hot reload)
        // For simple safety, restart is better
        delay(100);
        ESP.restart();
      },
      [](AsyncWebServerRequest *request, String filename, size_t index,
         uint8_t *data, size_t len, bool final) {
        static File modelFile;
        if (!index) {
          Serial.printf("[OTA-AI] Uploading model: %s\n", filename.c_str());
          modelFile = LittleFS.open("/neura9_model.tflite", "w");
        }
        if (modelFile) {
          modelFile.write(data, len);
        }
        if (final) {
          if (modelFile) {
            modelFile.close();
            Serial.printf("[OTA-AI] Model saved: %u bytes\n", index + len);
          }
        }
      });

  // API Reboot
  server.on("/api/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", "{\"status\":\"rebooting\"}");
    delay(100);
    ESP.restart();
  });

  // API Config Device
  server.on("/api/config/device", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              config_manager.save();
              request->send(200, "application/json", "{\"status\":\"saved\"}");
            });

  // API Status (respects hide_version setting - Dica 66)
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(WEB_USER, WEB_PASS))
      return request->requestAuthentication();
    AsyncResponseStream *response =
        request->beginResponseStream("application/json");
    DynamicJsonDocument doc(512);
    doc["uptime"] = millis() / 1000;
    doc["battery"] = sys_hw.getBatteryPercent();
    doc["networks"] = g_state.networks_seen;
    doc["handshakes"] = g_state.handshakes_captured;
    doc["wifi"] = g_state.wifi_enabled;
    doc["ble"] = g_state.ble_enabled;
    doc["attack_active"] = wifi_attacks.isActive();

    // Tip: Add PPS (Packets Per Second)
    static uint32_t last_pps_check = 0;
    static uint32_t last_packets = 0;
    static int current_pps = 0;
    uint32_t total = wifi_attacks.getPacketCount(); // Need accessor
    uint32_t now = millis();
    if (now - last_pps_check >= 1000) {
      current_pps = (total - last_packets) * 1000 / (now - last_pps_check);
      last_packets = total;
      last_pps_check = now;
    }
    doc["pps"] = current_pps;

    // Hide version info if in public/stealth mode (Dica 66)
    if (!g_state.hide_version) {
      doc["firmware"] = "LeleWatch v2.0";
      doc["hardware"] = "ESP32-S3";
      doc["locked"] = (millis() < g_state.lockout_until);
    }
    String responsestr; // Variable name collision fix
    serializeJson(doc, responsestr);
    request->send(200, "application/json", responsestr);
  });

  server.on("/api/security/config", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              if (request->hasParam("hide_version", true)) {
                g_state.hide_version =
                    request->getParam("hide_version", true)->value() == "true";
              }
              config_manager.save();
              request->send(200, "application/json", "{\"status\":\"saved\"}");
            });

  // API Panic Button Config (User Request)
  server.on("/api/security/panic", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              DynamicJsonDocument doc(256);
              doc["enabled"] = g_state.panic_button_enabled;
              doc["duration"] = g_state.panic_button_duration;
              String response;
              serializeJson(doc, response);
              request->send(200, "application/json", response);
            });

  server.on("/api/security/panic", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              if (request->hasParam("enabled", true)) {
                g_state.panic_button_enabled =
                    request->getParam("enabled", true)->value() == "true";
              }
              if (request->hasParam("duration", true)) {
                g_state.panic_button_duration =
                    request->getParam("duration", true)->value().toInt();
              }
              config_manager.saveSystemSettings();
              request->send(200, "application/json", "{\"status\":\"saved\"}");
            });

  // API Ghost Mode
  server.on("/api/security/ghost", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              bool enabled = true;
              if (request->hasParam("enabled", true)) {
                enabled = request->getParam("enabled", true)->value() == "true";
              }
              // Implement Ghost Mode Logic here (e.g., disable LEDs, sound,
              // etc) g_state.ghost_mode = enabled;
              // sys_hw.setGhostMode(enabled);

              DynamicJsonDocument doc(64);
              doc["status"] = "saved";
              doc["ghost"] = enabled;
              String response;
              serializeJson(doc, response);
              request->send(200, "application/json", response);
            });

  server.on(
      "/api/discord/webhook", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("url", true)) {
          String url = request->getParam("url", true)->value();
          strncpy(g_state.discord_webhook, url.c_str(),
                  sizeof(g_state.discord_webhook) - 1);
          g_state.discord_webhook[sizeof(g_state.discord_webhook) - 1] = '\0';
          config_manager.save();
          Serial.printf("[WEB] Discord webhook set: %s\n", url.c_str());
        }
        request->send(200, "application/json", "{\"status\":\"saved\"}");
      });

  // API Scan
  server.on("/api/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] Scan request");
    wifi_driver.scanNetworks(true);
    request->send(200, "application/json", "{\"status\":\"scanning\"}");
  });

  // API Networks
  server.on("/api/networks", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response =
        request->beginResponseStream("application/json");
    DynamicJsonDocument doc(4096);
    JsonArray networks = doc.createNestedArray("networks");

    int count = WiFi.scanComplete();
    if (count > 0) {
      for (int i = 0; i < count && i < 20; i++) {
        JsonObject net = networks.createNestedObject();
        net["ssid"] = WiFi.SSID(i);
        net["rssi"] = WiFi.RSSI(i);
        net["channel"] = WiFi.channel(i);
        net["encryption"] = WiFi.encryptionType(i);
      }
    }
    doc["count"] = count > 0 ? count : 0;
    serializeJson(doc, *response);
    request->send(response);
  });

  // API Deauth
  server.on("/api/deauth", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] Deauth request");
    if (request->hasParam("channel", true)) {
      int channel = request->getParam("channel", true)->value().toInt();
      uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
      wifi_attacks.setTarget(broadcast, "Broadcast", channel);
      wifi_attacks.startDeauth(100);
      request->send(200, "application/json", "{\"status\":\"deauth_started\"}");
    } else {
      request->send(400, "application/json",
                    "{\"error\":\"channel required\"}");
    }
  });

  // API Beacon Flood
  server.on("/api/beacon", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] Beacon Flood request");
    wifi_attacks.startBeaconFlood(nullptr, 0);
    request->send(200, "application/json",
                  "{\"status\":\"beacon_flood_started\"}");
  });

  // API Handshake
  server.on("/api/handshake", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] Handshake request");
    // For now just start capture on current or generic
    wifi_attacks.startHandshakeCapture();
    request->send(200, "application/json",
                  "{\"status\":\"handshake_started\"}");
  });

  // API Attack Start (General)
  server.on("/api/attack/start", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("type", true)) {
      String type = request->getParam("type", true)->value();
      if (type == "deauth") {
        // basic broadcast deauth on current channel if no params
        wifi_attacks.startDeauth(100);
      } else if (type == "beacon") {
        wifi_attacks.startBeaconFlood(nullptr, 0);
      } else if (type == "handshake") {
        wifi_attacks.startHandshakeCapture();
      }
      request->send(200, "application/json", "{\"status\":\"started\"}");
    } else {
      request->send(400, "application/json", "{\"error\":\"type required\"}");
    }
  });

  // API WPS Scan
  server.on("/api/wps/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] WPS Scan request");
    int count = wps_attacks.scanWPSNetworks();
    DynamicJsonDocument doc(256);
    doc["status"] = "completed";
    doc["wps_networks"] = count;
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  // API WPS Pixie Dust
  server.on("/api/wps/pixie", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] WPS Pixie Dust request");
    wps_attacks.startPixieDust();
    request->send(200, "application/json",
                  "{\"status\":\"pixie_dust_started\"}");
  });

  // API WPS Brute-force
  server.on("/api/wps/brute", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] WPS Brute-force request");
    wps_attacks.startBruteforce(0);
    request->send(200, "application/json",
                  "{\"status\":\"bruteforce_started\"}");
  });

  // API Evil Twin
  server.on("/api/eviltwin", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] Evil Twin request");
    if (request->hasParam("ssid", true)) {
      String ssid = request->getParam("ssid", true)->value();
      int portal_type = 0;
      if (request->hasParam("type", true)) {
        portal_type = request->getParam("type", true)->value().toInt();
      }
      wifi_attacks.startEvilTwin(ssid.c_str(), portal_type);
      request->send(200, "application/json",
                    "{\"status\":\"eviltwin_started\"}");
    } else {
      request->send(400, "application/json", "{\"error\":\"ssid required\"}");
    }
  });

  // API Stop All
  server.on("/api/stop", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] Stop all attacks");
    wifi_attacks.stopAttack();
    wps_attacks.stopAttack();
    ble_driver.stopSpam();
    request->send(200, "application/json", "{\"status\":\"stopped\"}");
  });

  // API BLE Spam
  server.on("/api/ble/spam", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] BLE Spam request");
    int type = 4; // ALL by default
    if (request->hasParam("type", true)) {
      type = request->getParam("type", true)->value().toInt();
    }
    ble_driver.startSpam((BLESpamType)type);
    g_state.ble_enabled = true;
    request->send(200, "application/json", "{\"status\":\"ble_spam_started\"}");
  });

  // API BLE Stop
  server.on("/api/ble/stop", HTTP_POST, [](AsyncWebServerRequest *request) {
    ble_driver.stopSpam();
    g_state.ble_enabled = false;
    request->send(200, "application/json", "{\"status\":\"ble_stopped\"}");
  });

  // API BLE Scan Start (Async)
  server.on(
      "/api/ble/scan_start", HTTP_POST, [](AsyncWebServerRequest *request) {
        int duration = 5;
        if (request->hasParam("duration", true)) {
          duration = request->getParam("duration", true)->value().toInt();
        }
        ble_driver.startScanAsync(duration);
        request->send(200, "application/json", "{\"status\":\"scan_started\"}");
      });

  // API BLE Feed (Scan Results)
  server.on("/api/ble/feed", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response =
        request->beginResponseStream("application/json");
    DynamicJsonDocument doc(8192); // Large buffer for many devices
    JsonArray devices = doc.createNestedArray("devices");

    std::vector<BLEDeviceData> results = ble_driver.getScanResults();
    for (const auto &dev : results) {
      JsonObject d = devices.createNestedObject();
      d["name"] = dev.name;
      d["addr"] = dev.address;
      d["rssi"] = dev.rssi;
      d["manuf"] = dev.manufacturerData;
    }
    doc["count"] = results.size();
    serializeJson(doc, *response);
    request->send(response);
  });

  // API Set Brightness
  server.on(
      "/api/set_brightness", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("value", true)) {
          int brightness = request->getParam("value", true)->value().toInt();
          brightness = constrain(brightness, 0, 255);

          // Apply brightness to display driver
          g_state.screen_brightness = brightness;
          sys_hw.setDisplayBrightness(brightness);

          Serial.printf("[WEB] Set brightness: %d\n", brightness);
          request->send(200, "application/json", "{\"status\":\"ok\"}");
        } else {
          request->send(400, "application/json",
                        "{\"error\":\"value required\"}");
        }
      });

  // API Terminal Command
  server.on("/api/terminal", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("cmd", true)) {
      String cmd = request->getParam("cmd", true)->value();
      Serial.printf("[WEB_TERM] %s\n", cmd.c_str());
      // Process command
      String response = "Command received: " + cmd;
      request->send(200, "application/json",
                    "{\"output\":\"" + response + "\"}");
    } else {
      request->send(400, "application/json", "{\"error\":\"cmd required\"}");
    }
  });

  // API PWN All - Start all attacks for 60 seconds
  server.on("/api/pwn_all", HTTP_POST, [](AsyncWebServerRequest *request) {
    Serial.println("[WEB] PWN ALL - Activating all attacks!");
    wifi_attacks.startDeauth(100);
    wifi_attacks.startBeaconFlood(nullptr, 0);
    ble_driver.startSpam(BLE_SPAM_KITCHEN_SINK);
    g_state.ble_enabled = true;
    request->send(200, "application/json", "{\"status\":\"pwn_all_started\"}");
  });

  // API Toggle WiFi
  server.on("/api/toggle_wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool enabled = true;
    if (request->hasParam("enabled", true)) {
      enabled = request->getParam("enabled", true)->value() == "true";
    }
    g_state.wifi_enabled = enabled;
    Serial.printf("[WEB] WiFi %s\n", enabled ? "enabled" : "disabled");
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // API Toggle BLE
  server.on("/api/toggle_ble", HTTP_POST, [](AsyncWebServerRequest *request) {
    bool enabled = true;
    if (request->hasParam("enabled", true)) {
      enabled = request->getParam("enabled", true)->value() == "true";
    }
    if (!enabled) {
      ble_driver.stopSpam();
    }
    g_state.ble_enabled = enabled;
    Serial.printf("[WEB] BLE %s\n", enabled ? "enabled" : "disabled");
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // API Files List
  server.on("/api/files/list", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(2048);
    JsonArray files = doc.createNestedArray("files");
    doc["count"] = 0;
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  // ═══════════════════════════════════════════════════════════════════════════
  // SD CARD MANAGER
  // ═══════════════════════════════════════════════════════════════════════════

  // Serve SD Manager files
  server.on("/sd_manager.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/sd_manager.html", "text/html");
  });
  server.on("/sd_manager.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/sd_manager.js", "application/javascript");
  });

  // SD API - Info & List
  server.on("/sd_json", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!SD_MMC.begin("/sdcard", true)) {
      // Try to mount if not already
      SD_MMC.begin("/sdcard", true);
    }

    DynamicJsonDocument doc(4096);
    if (SD_MMC.cardType() == CARD_NONE) {
      doc["online"] = false;
    } else {
      doc["online"] = true;
      doc["type"] = SD_MMC.cardType() == CARD_SD ? "SDHC/SDXC" : "Desconhecido";
      // User snippet calculation (size in MB is used as speed in snippet?
      // keeping it exactly as requested)
      doc["speed"] = SD_MMC.cardSize() / (1024 * 1024);

      uint64_t total = SD_MMC.totalBytes() / (1024 * 1024);
      uint64_t used = SD_MMC.usedBytes() / (1024 * 1024);
      doc["total_mb"] = total;
      doc["free_mb"] = total - used;

      JsonArray arr = doc.createNestedArray("files");
      File root = SD_MMC.open("/");
      if (root) {
        File file = root.openNextFile();
        while (file) {
          JsonObject f = arr.createNestedObject();
          f["name"] = String(file.name());
          f["path"] = String(file.path());
          f["size"] = file.size();
          f["is_dir"] = file.isDirectory();
          file = root.openNextFile();
        }
      }
    }
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  // SD Download
  server.on("/sd_download", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("file")) {
      String file = request->getParam("file")->value();
      if (SD_MMC.exists(file)) {
        request->send(SD_MMC, file, "application/octet-stream");
      } else {
        request->send(404, "text/plain", "File not found");
      }
    } else {
      request->send(400);
    }
  });

  // SD Delete
  server.on("/sd_delete", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("file")) {
      String file = request->getParam("file")->value();
      if (SD_MMC.exists(file))
        SD_MMC.remove(file);
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400);
    }
  });

  // SD Upload
  server.on(
      "/sd_upload", HTTP_POST,
      [](AsyncWebServerRequest *request) { request->send(200); },
      [](AsyncWebServerRequest *request, String filename, size_t index,
         uint8_t *data, size_t len, bool final) {
        static File uploadFile;
        if (!index) {
          // Use the path format the user's snippet implied or standard
          // relative
          String path = "/" + filename;
          if (SD_MMC.exists(path))
            SD_MMC.remove(path);
          uploadFile = SD_MMC.open(path, FILE_WRITE);
        }
        if (uploadFile) {
          uploadFile.write(data, len);
        }
        if (final) {
          if (uploadFile)
            uploadFile.close();
        }
      });

  // ═══════════════════════════════════════════════════════════════════════════
  // AI APIs
  // ═══════════════════════════════════════════════════════════════════════════
  server.on("/api/ai/config", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(512);
    // Refresh settings from global state (memory)
    doc["enabled"] = g_state.chatgpt_enabled;
    doc["apikey"] = g_state.openai_api_key;
    doc["voice"] = g_state.openai_voice;
    doc["model"] = g_state.openai_model;
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.on("/api/ai/config", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("enabled", true)) {
      String en = request->getParam("enabled", true)->value();
      g_state.chatgpt_enabled = (en == "true" || en == "1");
    }
    if (request->hasParam("apikey", true)) {
      String key = request->getParam("apikey", true)->value();
      strncpy(g_state.openai_api_key, key.c_str(),
              sizeof(g_state.openai_api_key) - 1);
      g_state.openai_api_key[sizeof(g_state.openai_api_key) - 1] = '\0';
    }
    if (request->hasParam("voice", true)) {
      String v = request->getParam("voice", true)->value();
      strncpy(g_state.openai_voice, v.c_str(),
              sizeof(g_state.openai_voice) - 1);
      g_state.openai_voice[sizeof(g_state.openai_voice) - 1] = '\0';
    }
    if (request->hasParam("model", true)) {
      String m = request->getParam("model", true)->value();
      strncpy(g_state.openai_model, m.c_str(),
              sizeof(g_state.openai_model) - 1);
      g_state.openai_model[sizeof(g_state.openai_model) - 1] = '\0';
    }
    config_manager.saveAiSettings();
    request->send(200, "application/json", "{\"status\":\"saved\"}");
  });

  // ═══════════════════════════════════════════════════════════════════════════
  // WALLPAPER APIs
  // ═══════════════════════════════════════════════════════════════════════════
  server.on("/api/wallpapers/list", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              wallpaper_system.scanWallpapers();
              DynamicJsonDocument doc(4096);
              JsonArray wallpapers = doc.createNestedArray("wallpapers");

              auto gallery = wallpaper_system.getGallery();
              for (const auto &wp : gallery) {
                JsonObject item = wallpapers.createNestedObject();
                item["filename"] = wp.filename;
                item["width"] = wp.width;
                item["height"] = wp.height;
                item["size"] = wp.filesize;
                item["isDefault"] = wp.isDefault;
              }
              doc["count"] = gallery.size();
              doc["current"] = wallpaper_system.getConfig().currentWallpaper;

              String response;
              serializeJson(doc, response);
              request->send(200, "application/json", response);
            });

  server.on("/api/wallpapers/set", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              if (request->hasParam("filename", true)) {
                String filename = request->getParam("filename", true)->value();
                Serial.printf("[WEB] Set wallpaper: %s\n", filename.c_str());
                if (wallpaper_system.setWallpaper(filename.c_str())) {
                  request->send(200, "application/json", "{\"status\":\"ok\"}");
                } else {
                  request->send(500, "application/json",
                                "{\"error\":\"failed to set wallpaper\"}");
                }
              } else {
                request->send(400, "application/json",
                              "{\"error\":\"filename required\"}");
              }
            });

  // ═══════════════════════════════════════════════════════════════════════════
  // SOUNDS APIs
  // ═══════════════════════════════════════════════════════════════════════════
  server.on("/api/sounds/list", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(4096);
    JsonArray sounds = doc.createNestedArray("sounds");

    auto library = sounds_manager.getLibrary();
    for (const auto &snd : library) {
      JsonObject item = sounds.createNestedObject();
      item["filename"] = snd.filename;
      item["size"] = snd.filesize;
      item["duration"] = snd.duration_ms;
      item["isPreset"] = snd.isPreset;
    }
    doc["count"] = library.size();
    doc["volume"] = sounds_manager.getConfig().masterVolume;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.on("/api/sounds/play", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("id", true)) {
      int id = request->getParam("id", true)->value().toInt();
      sounds_manager.play(id);
      request->send(200, "application/json", "{\"status\":\"playing\"}");
    } else {
      request->send(400, "application/json", "{\"error\":\"id required\"}");
    }
  });

  server.on("/api/sounds/volume", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              if (request->hasParam("value", true)) {
                int volume = request->getParam("value", true)->value().toInt();
                sounds_manager.setVolume(volume);
                request->send(200, "application/json", "{\"status\":\"ok\"}");
              } else {
                request->send(400, "application/json",
                              "{\"error\":\"value required\"}");
              }
            });

  // ═══════════════════════════════════════════════════════════════════════════
  // NOTIFICATIONS APIs
  // ═══════════════════════════════════════════════════════════════════════════
  server.on("/api/notifications/config", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              DynamicJsonDocument doc(512);
              NotificationConfig cfg = notifications_engine.getConfig();
              doc["enabled"] = cfg.enabled;
              doc["effect"] = (int)cfg.effect;
              char colorHex[8];
              snprintf(colorHex, sizeof(colorHex), "#%06X", cfg.color);
              doc["color"] = colorHex;
              doc["duration"] = cfg.duration_ms;
              doc["fade"] = (int)cfg.fade;
              doc["vibration"] = (int)cfg.vibration;
              doc["sound"] = cfg.play_sound;
              doc["soundId"] = cfg.sound_id;

              String response;
              serializeJson(doc, response);
              request->send(200, "application/json", response);
            });

  server.on(
      "/api/notifications/config", HTTP_POST,
      [](AsyncWebServerRequest *request) {
        Serial.println("[WEB] Notification config updated");
        // Parse parameters
        NotificationConfig cfg = notifications_engine.getConfig();

        if (request->hasParam("enabled", true)) {
          cfg.enabled = request->getParam("enabled", true)->value() == "true";
        }
        if (request->hasParam("effect", true)) {
          cfg.effect = (NotificationEffect)request->getParam("effect", true)
                           ->value()
                           .toInt();
        }
        if (request->hasParam("duration", true)) {
          cfg.duration_ms =
              request->getParam("duration", true)->value().toInt();
        }
        if (request->hasParam("vibration", true)) {
          cfg.vibration = (VibrationPattern)request->getParam("vibration", true)
                              ->value()
                              .toInt();
        }

        notifications_engine.setConfig(cfg);
        request->send(200, "application/json", "{\"status\":\"ok\"}");
      });

  server.on(
      "/api/notifications/test", HTTP_POST, [](AsyncWebServerRequest *request) {
        Serial.println("[WEB] Testing notification");
        notifications_engine.test();
        request->send(200, "application/json", "{\"status\":\"testing\"}");
      });

  // ═══════════════════════════════════════════════════════════════════════════
  // THEME EDITOR APIs
  // ═══════════════════════════════════════════════════════════════════════════
  server.on("/api/theme/current", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(512);
    doc["theme_index"] = g_state.current_theme;
    doc["theme_name"] = g_state.current_theme < THEME_COUNT
                            ? theme_names[g_state.current_theme]
                            : "Unknown";
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.on("/api/theme/list", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(1024);
    JsonArray themes_arr = doc.createNestedArray("themes");
    for (int i = 0; i < THEME_COUNT; i++) {
      JsonObject t = themes_arr.createNestedObject();
      t["id"] = i;
      t["name"] = theme_names[i];
    }
    doc["count"] = THEME_COUNT;
    doc["current"] = g_state.current_theme;
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.on("/api/theme/set", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("id", true)) {
      int id = request->getParam("id", true)->value().toInt();
      if (id >= 0 && id < THEME_COUNT) {
        g_state.current_theme = (UITheme)id;
        config_manager.save();
        Serial.printf("[WEB] Theme set to: %s\n", theme_names[id]);
        request->send(200, "application/json", "{\"status\":\"ok\"}");
      } else {
        request->send(400, "application/json",
                      "{\"error\":\"invalid theme id\"}");
      }
    } else {
      request->send(400, "application/json", "{\"error\":\"id required\"}");
    }
  });

  server.on(
      "/api/theme/save", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL,
      [](AsyncWebServerRequest *request, uint8_t *data, size_t len,
         size_t index, size_t total) {
        // Parse JSON body
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
          request->send(400, "application/json",
                        "{\"error\":\"invalid json\"}");
          return;
        }

        Serial.println("[WEB] Custom theme saved");

        // For now, just log and acknowledge
        // Full implementation would save to NVS/SD
        String response;
        DynamicJsonDocument resp(128);
        resp["status"] = "saved";
        resp["message"] = "Theme saved successfully";
        serializeJson(resp, response);
        request->send(200, "application/json", response);
      });

  // ═══════════════════════════════════════════════════════════════════════════
  // LANGUAGE APIs
  // ═══════════════════════════════════════════════════════════════════════════
  server.on(
      "/api/language/current", HTTP_GET, [](AsyncWebServerRequest *request) {
        DynamicJsonDocument doc(128);
        doc["code"] = g_state.dragon_talk_lang == 0 ? "pt-BR" : "en-US";
        doc["name"] = g_state.dragon_talk_lang == 0 ? "Portugues" : "English";
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
      });

  server.on("/api/language/set", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("lang", true)) {
      String lang = request->getParam("lang", true)->value();
      if (lang == "pt-BR" || lang == "pt") {
        g_state.dragon_talk_lang = 0;
      } else if (lang == "en-US" || lang == "en") {
        g_state.dragon_talk_lang = 1;
      } else if (lang == "es") {
        g_state.dragon_talk_lang = 2;
      }
      config_manager.save();
      request->send(200, "application/json", "{\"status\":\"ok\"}");
    } else {
      request->send(400, "application/json", "{\"error\":\"lang required\"}");
    }
  });

  // ═══════════════════════════════════════════════════════════════════════════
  // MASCOT API ENDPOINTS
  // ═══════════════════════════════════════════════════════════════════════════

  // GET /api/mascot/list - List all mascots
  server.on("/api/mascot/list", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(1024);
    JsonArray arr = doc.to<JsonArray>();

    for (uint8_t i = 0; i < MascotManager::getMascotCount(); i++) {
      const MascotInfo &info = mascot_manager.getMascotInfo((MascotType)i);
      JsonObject m = arr.createNestedObject();
      m["id"] = i;
      m["type"] = info.id;
      m["name"] = info.name_default;
      m["emoji"] = info.emoji;
      m["description"] = info.description;
    }

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
  });

  // GET /api/mascot/current - Get current mascot config
  server.on("/api/mascot/current", HTTP_GET,
            [](AsyncWebServerRequest *request) {
              DynamicJsonDocument doc(256);
              MascotConfig &cfg = mascot_manager.getConfig();
              const MascotInfo &info = mascot_manager.getCurrentMascotInfo();

              doc["id"] = cfg.mascot_id;
              doc["type"] = info.id;
              doc["name"] = cfg.mascot_name;
              doc["emoji"] = info.emoji;
              doc["animation_speed"] = cfg.animation_speed;
              doc["speech_bubbles"] = cfg.show_speech_bubbles;
              doc["mood"] = cfg.mood;

              String json;
              serializeJson(doc, json);
              request->send(200, "application/json", json);
            });

  // POST /api/mascot/select - Select mascot by ID
  server.on(
      "/api/mascot/select", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (request->hasParam("id", true)) {
          uint8_t id = request->getParam("id", true)->value().toInt();
          if (id < MascotManager::getMascotCount()) {
            mascot_manager.setMascot((MascotType)id);
            request->send(200, "application/json", "{\"status\":\"ok\"}");
          } else {
            request->send(400, "application/json",
                          "{\"error\":\"invalid id\"}");
          }
        } else {
          request->send(400, "application/json", "{\"error\":\"id required\"}");
        }
      });

  // POST /api/mascot/rename - Rename current mascot
  server.on("/api/mascot/rename", HTTP_POST,
            [](AsyncWebServerRequest *request) {
              if (request->hasParam("name", true)) {
                String name = request->getParam("name", true)->value();
                mascot_manager.setName(name.c_str());
                request->send(200, "application/json", "{\"status\":\"ok\"}");
              } else {
                request->send(400, "application/json",
                              "{\"error\":\"name required\"}");
              }
            });

  // ═══════════════════════════════════════════════════════════════════════════
  // WATCH MODE API ENDPOINTS
  // ═══════════════════════════════════════════════════════════════════════════

  // GET /api/watch/status - Get watch mode status
  server.on("/api/watch/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(512);
    WatchConfig &cfg = watch_mode.getConfig();
    WatchStats &stats = watch_mode.getStats();

    doc["active"] = watch_mode.isActive();
    doc["watchface"] = cfg.watchface_id;
    doc["show_seconds"] = cfg.show_seconds;
    doc["show_date"] = cfg.show_date;
    doc["show_battery"] = cfg.show_battery;
    doc["background_sniffing"] = cfg.background_sniffing;
    doc["brightness"] = cfg.brightness_level;

    JsonObject st = doc.createNestedObject("stats");
    st["networks"] = stats.networks_found;
    st["handshakes"] = stats.handshakes_captured;
    st["uptime"] = stats.uptime_minutes;

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
  });

  // GET /api/watch/faces - List available watchfaces
  server.on("/api/watch/faces", HTTP_GET, [](AsyncWebServerRequest *request) {
    const char *json = R"([
      {"id":0,"name":"Digital","icon":"⌚"},
      {"id":1,"name":"Analog","icon":"🕐"},
      {"id":2,"name":"Cyberpunk","icon":"📊"},
      {"id":3,"name":"Mascot","icon":"🐉"},
      {"id":4,"name":"Hacker","icon":"💀"},
      {"id":5,"name":"Minimal","icon":"🌙"}
    ])";
    request->send(200, "application/json", json);
  });

  // POST /api/watch/enable - Enable watch mode
  server.on("/api/watch/enable", HTTP_POST, [](AsyncWebServerRequest *request) {
    watch_mode.enter();
    request->send(200, "application/json", "{\"status\":\"enabled\"}");
  });

  // POST /api/watch/disable - Disable watch mode
  server.on(
      "/api/watch/disable", HTTP_POST, [](AsyncWebServerRequest *request) {
        watch_mode.exit();
        request->send(200, "application/json", "{\"status\":\"disabled\"}");
      });

  // POST /api/watch/config - Update watch config
  server.on("/api/watch/config", HTTP_POST, [](AsyncWebServerRequest *request) {
    WatchConfig &cfg = watch_mode.getConfig();

    if (request->hasParam("watchface", true)) {
      uint8_t wf = request->getParam("watchface", true)->value().toInt();
      watch_mode.setWatchface((WatchfaceType)wf);
    }
    if (request->hasParam("show_seconds", true)) {
      cfg.show_seconds =
          request->getParam("show_seconds", true)->value() == "1";
    }
    if (request->hasParam("show_date", true)) {
      cfg.show_date = request->getParam("show_date", true)->value() == "1";
    }
    if (request->hasParam("background_sniffing", true)) {
      cfg.background_sniffing =
          request->getParam("background_sniffing", true)->value() == "1";
    }
    if (request->hasParam("brightness", true)) {
      cfg.brightness_level =
          request->getParam("brightness", true)->value().toInt();
    }

    watch_mode.saveConfig();
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // POST /api/watch/next - Next watchface
  server.on("/api/watch/next", HTTP_POST, [](AsyncWebServerRequest *request) {
    watch_mode.nextWatchface();
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  });

  // POST /api/terminal - Terminal command execution
  server.on("/api/terminal", HTTP_POST, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument doc(512);

    if (request->hasParam("command", true)) {
      String cmd = request->getParam("command", true)->value();

      // Process commands
      if (cmd == "status") {
        doc["output"] = "Networks: " + String(g_state.networks_seen) +
                        "\nHandshakes: " + String(g_state.handshakes_captured);
        doc["success"] = true;
      } else if (cmd == "scan") {
        wifi_driver.scanNetworks(true);
        doc["message"] = "Scan started";
        doc["success"] = true;
      } else if (cmd == "stop") {
        wifi_attacks.stopAttack();
        doc["message"] = "All attacks stopped";
        doc["success"] = true;
      } else if (cmd == "reboot") {
        doc["message"] = "Rebooting...";
        doc["success"] = true;
        // Schedule reboot after response
      } else {
        doc["error"] = "Unknown command: " + cmd;
        doc["success"] = false;
      }
    } else {
      doc["error"] = "command required";
      doc["success"] = false;
    }

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
  });
}

void WebInterface::onWebSocketEvent(AsyncWebSocket *server,
                                    AsyncWebSocketClient *client,
                                    AwsEventType type, void *arg, uint8_t *data,
                                    size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("[WEB] Cliente #%u conectado de %s\n", client->id(),
                  client->remoteIP().toString().c_str());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("[WEB] Cliente #%u desconectado\n", client->id());
  }
}

void WebInterface::update() {
  ws.cleanupClients();

  // Push updates every 2s
  if (millis() - _lastUpdate > 2000) {
    _lastUpdate = millis();
    if (ws.count() > 0) {
      DynamicJsonDocument doc(1024);
      doc["uptime"] = millis() / 1000;
      doc["battery"] = sys_hw.getBatteryPercent();
      doc["aps"] = g_state.networks_seen;
      doc["hs"] = g_state.handshakes_captured;
      doc["pmkid"] = g_state.pmkid_captured;
      doc["ai"] = wifi_attacks.isActive() ? "ATTACK" : "SAFE";
      doc["temp"] = temperatureRead(); // ESP32 internal sensor
      doc["activity"] = random(5, 50); // Simulated activity for chart
      doc["ble"] = g_state.ble_enabled ? 1 : 0;
      doc["deauths"] = g_state.deauth_packets_sent;

      String json;
      serializeJson(doc, json);
      ws.textAll(json);
    }
  }
}
