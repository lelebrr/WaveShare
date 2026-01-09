#include "globals.h"

// Instâncias globais dos drivers
Pwnagotchi *g_pwnagotchi = nullptr;
Arduino_GFX *g_display = nullptr;
ESP_IOExpander *g_expander = nullptr;

// Flag para suspender BLE/LVGL durante ataques intensivos
bool g_suspend_ble_lvgl = false;

// Instância global de Pwnagotchi para retrocompatibilidade
#include "../pwnagotchi/pwnagotchi.h"
Pwnagotchi pwn;

// Estado global (order must match GlobalState struct in globals.h!)
GlobalState g_state = {
    // ============================================================
    // SISTEMA (runtime - não persistido)
    // ============================================================
    .uptime_seconds = 0,
    .handshakes_captured = 0,
    .networks_seen = 0,
    .battery_percent = 0,
    .is_charging = false,

    // ============================================================
    // DISPLAY & SOM (1-12) - Persistido
    // ============================================================
    .screen_brightness = BRIGHTNESS_DEFAULT,
    .screen_timeout_s = 30,
    .always_on = false,
    .dragon_theme = 0,
    .dragon_eye_color = 0,
    .dragon_size = 1,
    .buzzer_volume = 50,
    .sound_on_handshake = true,
    .vibrate_on_boot = false,
    .invert_colors = false,
    .show_fps = false,
    .system_font = 0,

    // ============================================================
    // REDE & WIRELESS (13-28) - Persistido
    // ============================================================
    .wifi_tx_power_dbm = 20,
    .wifi_channel = -1,
    .custom_mac = {0, 0, 0, 0, 0, 0},
    .use_custom_mac = false,
    .mac_spoof_on_boot = false,
    .radio_2ghz_enabled = true,

    .beacon_spam_enabled = false,
    .beacon_spam_speed = 5,
    .beacon_ssid_count = 20,
    .evil_twin_auto = false,
    .karma_mana_enabled = false,
    .deauth_all_networks = false,
    .monitor_mode_only = false,
    .ble_spam_enabled = false,
    .ble_spam_type = 0,

    .promiscuous_mode = false,

    // ============================================================
    // ATAQUES & AUTOMATIZAÇÃO (29-42) - Persistido
    // ============================================================
    .wps_pixie_auto = false,
    .wps_bruteforce = false,
    .wps_flood_intensity = 5,
    .rogue_ap_count = 10,
    .auto_capture_new_only = true,
    .auto_save_pcap = true,
    .auto_attack_favorites = false,
    .favorite_count = 0,
    .insane_mode_enabled = false,

    .captive_portal_auto = false,
    .captive_template_id = 0,
    .badusb_auto = false,
    .usb_payload_id = 0,

    // ============================================================
    // SISTEMA & ENERGIA (43-54) - Persistido
    // ============================================================
    .deep_sleep_minutes = 0,
    .wake_on_touch = true,
    .wake_on_shake = false,
    .big_battery_display = false,
    .stealth_boot = false,
    .timezone_offset = -3,

    // ============================================================
    // PERSONALIDADE DO DRAGÃO (55-60) - Persistido
    // ============================================================
    .dragon_name = {'N', 'e', 'u', 'r', 'a', 0},
    .dragon_personality = 0,
    .dragon_talk_enabled = true,
    .dragon_talk_lang = 0,
    .dragon_talk_freq = 2,
    .dragon_exaggerated = true,
    .pet_only_mode = false,

    // ============================================================
    // AI & INTEGRAÇÃO (61-65) - Persistido
    // ============================================================
    // .openai_api_key = "",
    // .openai_model = "gpt-4o-mini",
    // .openai_voice = "alloy",
    // .chatgpt_enabled = false,

    // ============================================================
    // RUNTIME STATE (não persistido - derivado/temporário)
    // ============================================================
    .wifi_enabled = true,
    .ble_enabled = true,
    .sleep_mode = false,
    .mascot_enabled = true,
    .audio_enabled = true,
    .audio_volume = 50,
    .scan_time_ms = 15000,
    .auto_attack_enabled = false,
    .auto_deauth = false,
    .auto_capture = true,
    .show_clock = true,
    .voice_enabled = false,
    .current_theme = THEME_HACKER_DARK};
