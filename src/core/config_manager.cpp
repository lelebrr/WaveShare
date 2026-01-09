/**
 * @file config_manager.cpp
 * @brief Gerenciador de configurações com persistência NVS
 *
 * Salva e carrega todas as 60 opções de configuração do LeleWatch
 * organizadas em 5 namespaces:
 * - disp: Display & Som (12)
 * - net: Rede & Wireless (16)
 * - atk: Ataques & Automatização (14)
 * - sys: Sistema & Energia (12)
 * - drg: Personalidade do Dragão (6)
 */

#include "config_manager.h"
#include "pin_definitions.h"
#include <ArduinoJson.h>
#include <FS.h>
#include <SD_MMC.h>

ConfigManager config_manager;

ConfigManager::ConfigManager() {}

void ConfigManager::begin() {
  Serial.println("[CONFIG] Inicializando ConfigManager...");
  load();       // Load from NVS
  loadFromSD(); // Overlay with SD Config if present
  Serial.println("[CONFIG] Configurações carregadas (NVS + SD)");
}

void ConfigManager::load() {
  loadDisplaySettings();
  loadNetworkSettings();
  loadAttackSettings();
  loadSystemSettings();
  loadDragonSettings();
  loadAiSettings();
  loadRuntimeDefaults();
}

void ConfigManager::save() {
  saveDisplaySettings();
  saveNetworkSettings();
  saveAttackSettings();
  saveSystemSettings();
  saveDragonSettings();
  saveAiSettings();
  Serial.println("[CONFIG] Todas as configurações salvas no NVS");
}

void ConfigManager::reset() {
  // Limpa todos os namespaces
  prefs.begin("disp", false);
  prefs.clear();
  prefs.end();
  prefs.begin("net", false);
  prefs.clear();
  prefs.end();
  prefs.begin("atk", false);
  prefs.clear();
  prefs.end();
  prefs.begin("sys", false);
  prefs.clear();
  prefs.end();
  prefs.begin("drg", false);
  prefs.clear();
  prefs.end();
  prefs.begin("ai", false);
  prefs.clear();
  prefs.end();

  // Recarrega defaults
  load();
  Serial.println("[CONFIG] Configurações resetadas para padrão");
}

// ============================================================
// DISPLAY & SOM (1-12)
// ============================================================
void ConfigManager::loadDisplaySettings() {
  prefs.begin("disp", true); // true = read-only

  g_state.screen_brightness = prefs.getUChar("bright", BRIGHTNESS_DEFAULT);
  g_state.screen_timeout_s = prefs.getUChar("timeout", 30);
  g_state.always_on = prefs.getBool("alwayson", false);
  g_state.dragon_theme = prefs.getUChar("dtheme", 0);
  g_state.dragon_eye_color = prefs.getUChar("eyecolor", 10); // Verde neon
  g_state.dragon_size = prefs.getUChar("dsize", 1);          // Médio
  g_state.buzzer_volume = prefs.getUChar("buzzvol", 50);
  g_state.sound_on_handshake = prefs.getBool("sndhs", true);
  g_state.vibrate_on_boot = prefs.getBool("vibboot", true);
  g_state.invert_colors = prefs.getBool("invert", false);
  g_state.show_fps = prefs.getBool("fps", false);
  g_state.system_font = prefs.getUChar("font", 0);

  prefs.end();
}

void ConfigManager::saveDisplaySettings() {
  prefs.begin("disp", false);

  prefs.putUChar("bright", g_state.screen_brightness);
  prefs.putUChar("timeout", g_state.screen_timeout_s);
  prefs.putBool("alwayson", g_state.always_on);
  prefs.putUChar("dtheme", g_state.dragon_theme);
  prefs.putUChar("eyecolor", g_state.dragon_eye_color);
  prefs.putUChar("dsize", g_state.dragon_size);
  prefs.putUChar("buzzvol", g_state.buzzer_volume);
  prefs.putBool("sndhs", g_state.sound_on_handshake);
  prefs.putBool("vibboot", g_state.vibrate_on_boot);
  prefs.putBool("invert", g_state.invert_colors);
  prefs.putBool("fps", g_state.show_fps);
  prefs.putUChar("font", g_state.system_font);

  prefs.end();
}

// ============================================================
// REDE & WIRELESS (13-28)
// ============================================================
void ConfigManager::loadNetworkSettings() {
  prefs.begin("net", true);

  g_state.wifi_tx_power_dbm = prefs.getUChar("txpwr", 20); // Max power
  g_state.wifi_channel = prefs.getChar("channel", -1);     // Auto
  prefs.getBytes("cmac", g_state.custom_mac, 6);
  g_state.use_custom_mac = prefs.getBool("usemac", false);
  g_state.mac_spoof_on_boot = prefs.getBool("macspoof", false);
  g_state.radio_2ghz_enabled = prefs.getBool("r2ghz", true);

  g_state.beacon_spam_enabled = prefs.getBool("beacon", false);
  g_state.beacon_spam_speed = prefs.getUChar("beaconspd", 5);
  g_state.beacon_ssid_count = prefs.getUChar("ssidcnt", 0);
  g_state.evil_twin_auto = prefs.getBool("eviltwin", false);
  g_state.karma_mana_enabled = prefs.getBool("karma", false);
  g_state.deauth_all_networks = prefs.getBool("deauthall", false);
  g_state.monitor_mode_only = prefs.getBool("mononly", false);
  g_state.ble_spam_enabled = prefs.getBool("blespam", false);
  g_state.ble_spam_type = prefs.getUChar("bletype", 0);

  g_state.promiscuous_mode = prefs.getBool("promisc", true);

  prefs.end();
}

void ConfigManager::saveNetworkSettings() {
  prefs.begin("net", false);

  prefs.putUChar("txpwr", g_state.wifi_tx_power_dbm);
  prefs.putChar("channel", g_state.wifi_channel);
  prefs.putBytes("cmac", g_state.custom_mac, 6);
  prefs.putBool("usemac", g_state.use_custom_mac);
  prefs.putBool("macspoof", g_state.mac_spoof_on_boot);
  prefs.putBool("r2ghz", g_state.radio_2ghz_enabled);

  prefs.putBool("beacon", g_state.beacon_spam_enabled);
  prefs.putUChar("beaconspd", g_state.beacon_spam_speed);
  prefs.putUChar("ssidcnt", g_state.beacon_ssid_count);
  prefs.putBool("eviltwin", g_state.evil_twin_auto);
  prefs.putBool("karma", g_state.karma_mana_enabled);
  prefs.putBool("deauthall", g_state.deauth_all_networks);
  prefs.putBool("mononly", g_state.monitor_mode_only);
  prefs.putBool("blespam", g_state.ble_spam_enabled);
  prefs.putUChar("bletype", g_state.ble_spam_type);

  prefs.putBool("promisc", g_state.promiscuous_mode);

  prefs.end();
}

// ============================================================
// ATAQUES & AUTOMATIZAÇÃO (29-42)
// ============================================================
void ConfigManager::loadAttackSettings() {
  prefs.begin("atk", true);

  g_state.wps_pixie_auto = prefs.getBool("pixie", false);
  g_state.wps_bruteforce = prefs.getBool("wpsbrute", false);
  g_state.wps_flood_intensity = prefs.getUChar("wpsflood", 5);
  g_state.rogue_ap_count = prefs.getUChar("rogueap", 10);
  g_state.auto_capture_new_only = prefs.getBool("capnew", true);
  g_state.auto_save_pcap = prefs.getBool("autopcap", true);
  g_state.auto_attack_favorites = prefs.getBool("atkfav", false);
  g_state.favorite_count = prefs.getUChar("favcnt", 0);
  g_state.insane_mode_enabled = prefs.getBool("insane", false);

  g_state.captive_portal_auto = prefs.getBool("captive", false);
  g_state.captive_template_id = prefs.getUChar("captpl", 0);
  g_state.badusb_auto = prefs.getBool("badusb", false);
  g_state.usb_payload_id = prefs.getUChar("usbpay", 0);

  prefs.end();
}

void ConfigManager::saveAttackSettings() {
  prefs.begin("atk", false);

  prefs.putBool("pixie", g_state.wps_pixie_auto);
  prefs.putBool("wpsbrute", g_state.wps_bruteforce);
  prefs.putUChar("wpsflood", g_state.wps_flood_intensity);
  prefs.putUChar("rogueap", g_state.rogue_ap_count);
  prefs.putBool("capnew", g_state.auto_capture_new_only);
  prefs.putBool("autopcap", g_state.auto_save_pcap);
  prefs.putBool("atkfav", g_state.auto_attack_favorites);
  prefs.putUChar("favcnt", g_state.favorite_count);
  prefs.putBool("insane", g_state.insane_mode_enabled);

  prefs.putBool("captive", g_state.captive_portal_auto);
  prefs.putUChar("captpl", g_state.captive_template_id);
  prefs.putBool("badusb", g_state.badusb_auto);
  prefs.putUChar("usbpay", g_state.usb_payload_id);

  prefs.end();
}

// ============================================================
// SISTEMA & ENERGIA (43-54)
// ============================================================
void ConfigManager::loadSystemSettings() {
  prefs.begin("sys", true);

  g_state.deep_sleep_minutes = prefs.getUChar("dsleep", 0); // Disabled
  g_state.wake_on_touch = prefs.getBool("wtouch", true);
  g_state.wake_on_shake = prefs.getBool("wshake", false);
  g_state.big_battery_display = prefs.getBool("bigbat", false);
  g_state.stealth_boot = prefs.getBool("stealth", false);
  g_state.timezone_offset = prefs.getChar("tz", -3); // BRT default
  g_state.panic_button_enabled = prefs.getBool("panic", true);
  g_state.panic_button_duration = prefs.getUChar("panicdur", 5);

  prefs.end();
}

void ConfigManager::saveSystemSettings() {
  prefs.begin("sys", false);

  prefs.putUChar("dsleep", g_state.deep_sleep_minutes);
  prefs.putBool("wtouch", g_state.wake_on_touch);
  prefs.putBool("wshake", g_state.wake_on_shake);
  prefs.putBool("bigbat", g_state.big_battery_display);
  prefs.putBool("stealth", g_state.stealth_boot);
  prefs.putChar("tz", g_state.timezone_offset);
  prefs.putBool("panic", g_state.panic_button_enabled);
  prefs.putUChar("panicdur", g_state.panic_button_duration);

  prefs.end();
}

// ============================================================
// PERSONALIDADE DO DRAGÃO (55-60)
// ============================================================
void ConfigManager::loadDragonSettings() {
  prefs.begin("drg", true);

  String name = prefs.getString("name", "NEURA9");
  strncpy(g_state.dragon_name, name.c_str(), 15);
  g_state.dragon_name[15] = '\0';

  g_state.dragon_personality = prefs.getUChar("pers", 4); // Hacker default
  g_state.dragon_talk_enabled = prefs.getBool("talk", true);
  g_state.dragon_talk_lang = prefs.getUChar("lang", 0); // PT-BR
  g_state.dragon_talk_freq = prefs.getUChar("freq", 2); // Normal
  g_state.dragon_exaggerated = prefs.getBool("exag", true);
  g_state.pet_only_mode = prefs.getBool("petonly", false);

  prefs.end();
}

void ConfigManager::saveDragonSettings() {
  prefs.begin("drg", false);

  prefs.putString("name", g_state.dragon_name);
  prefs.putUChar("pers", g_state.dragon_personality);
  prefs.putBool("talk", g_state.dragon_talk_enabled);
  prefs.putUChar("lang", g_state.dragon_talk_lang);
  prefs.putUChar("freq", g_state.dragon_talk_freq);
  prefs.putBool("exag", g_state.dragon_exaggerated);
  prefs.putBool("petonly", g_state.pet_only_mode);

  prefs.end();
}

// ============================================================
// AI SETTINGS (ChatGPT)
// ============================================================
void ConfigManager::loadAiSettings() {
  prefs.begin("ai", true);

  g_state.chatgpt_enabled = prefs.getBool("enabled", false);

  String key = prefs.getString("apikey", "");
  strncpy(g_state.openai_api_key, key.c_str(),
          sizeof(g_state.openai_api_key) - 1);
  g_state.openai_api_key[sizeof(g_state.openai_api_key) - 1] = '\0';

  String voice = prefs.getString("voice", "alloy");
  strncpy(g_state.openai_voice, voice.c_str(),
          sizeof(g_state.openai_voice) - 1);
  g_state.openai_voice[sizeof(g_state.openai_voice) - 1] = '\0';

  String model = prefs.getString("model", "gpt-4o-mini");
  strncpy(g_state.openai_model, model.c_str(),
          sizeof(g_state.openai_model) - 1);
  g_state.openai_model[sizeof(g_state.openai_model) - 1] = '\0';

  prefs.end();
}

void ConfigManager::saveAiSettings() {
  prefs.begin("ai", false);

  prefs.putBool("enabled", g_state.chatgpt_enabled);
  prefs.putString("apikey", g_state.openai_api_key);
  prefs.putString("voice", g_state.openai_voice);
  prefs.putString("model", g_state.openai_model);

  prefs.end();
}

// ============================================================
// RUNTIME DEFAULTS (não persistido)
// ============================================================
void ConfigManager::loadRuntimeDefaults() {
  // Valores de runtime que são resetados a cada boot
  g_state.wifi_enabled = true;
  g_state.ble_enabled = false;
  g_state.sleep_mode = false;
  g_state.mascot_enabled = true;
  g_state.audio_enabled = true;
  g_state.audio_volume = 50;
  g_state.scan_time_ms = 5000;
  g_state.auto_attack_enabled = false;
  g_state.auto_deauth = false;
  g_state.auto_capture = true;
  g_state.show_clock = true;
  g_state.voice_enabled = false;
  g_state.ghost_mode_enabled = false;
  g_state.current_theme = THEME_CYBERPUNK;

  // Contadores
  g_state.uptime_seconds = 0;
  g_state.handshakes_captured = 0;
  g_state.networks_seen = 0;
  g_state.battery_percent = 100;
  g_state.is_charging = false;
}

bool ConfigManager::isPluginEnabled(const char *name) {
  if (!name)
    return true;
  for (const auto &s : _disabled_plugins) {
    if (s.equalsIgnoreCase(name))
      return false;
  }
  return true;
}

// ============================================================
// SD CARD CONFIGURATION
// ============================================================
void ConfigManager::loadFromSD() {
  if (!SD_MMC.begin("/sdcard", true)) {
    // Try to init, if fail return
    return;
  }

  File file = SD_MMC.open("/config.json", FILE_READ);
  if (!file) {
    return;
  }

  Serial.println("[CONFIG] Carregando /config.json do SD...");

  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error) {
    Serial.print("[CONFIG] JSON Parse Error: ");
    Serial.println(error.c_str());
    return;
  }

  // 1. Potência (TX Power)
  if (doc.containsKey("tx_power")) {
    g_state.wifi_tx_power_dbm = doc["tx_power"];
    Serial.printf("[CONFIG] SD Set TX Power: %d\n", g_state.wifi_tx_power_dbm);
  }

  // 2. Canal Fixo
  if (doc.containsKey("channel")) {
    g_state.wifi_channel = doc["channel"];
    Serial.printf("[CONFIG] SD Set Channel: %d\n", g_state.wifi_channel);
  }

  // 3. Idioma (Language)
  if (doc.containsKey("language")) {
    String lang = doc["language"].as<String>();
    if (lang == "pt")
      g_state.dragon_talk_lang = 0;
    else if (lang == "en")
      g_state.dragon_talk_lang = 1;
    Serial.printf("[CONFIG] SD Set Lang: %s\n", lang.c_str());
  }

  // 4. Plugins
  if (doc.containsKey("plugins")) {
    JsonObject plugins = doc["plugins"];
    for (JsonPair kv : plugins) {
      if (kv.value().is<bool>() && !kv.value().as<bool>()) {
        _disabled_plugins.push_back(kv.key().c_str());
        Serial.printf("[CONFIG] Plugin Disabled via SD: %s\n",
                      kv.key().c_str());
      }
    }
  }
}
