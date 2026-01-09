#pragma once

#include "Arduino_GFX_Library.h"
#include "config.h"
#include "pin_definitions.h"
#include <Arduino.h>
#include <ESP_IOExpander_Library.h>

// Forward declarations
class Pwnagotchi;

// === TEMAS DISPONÍVEIS ===
enum UITheme {
  THEME_HACKER_DARK,      // Verde neon em fundo preto (Original)
  THEME_CYBERPUNK,        // Rosa/roxo neon (Cyber Purple)
  THEME_MATRIX,           // Verde matrix clássico (Matrix Green)
  THEME_OCEAN,            // Azul ciano (Ice Blue alternative)
  THEME_BLOOD,            // Vermelho escuro (Blood Red)
  THEME_DARK_DRAGON,      // Novo: Dark Dragon (Padrão)
  THEME_TRUE_BLACK,       // Novo: Modo AMOLED True Black
  THEME_RETRO_MIAMI,      // Novo: Retro Miami
  THEME_ICE_BLUE,         // Novo: Ice Blue específico
  THEME_STEALTH,          // Novo: Modo Stealth (preto + contornos vermelhos)
  THEME_HACKER_HOLLYWOOD, // Novo: Scanlines, glitch effects, OCR-A
  THEME_COUNT
};

struct GlobalState {
  uint32_t uptime_seconds;
  uint16_t handshakes_captured;
  uint16_t pmkid_captured;        // PMKID captures
  uint32_t deauth_packets_sent;   // Total deauth packets sent
  uint32_t networks_seen;
  uint8_t battery_percent;
  bool is_charging;

  // ============================================================
  // DISPLAY & SOM (1-12) - Persistido
  // ============================================================
  uint8_t screen_brightness; // 0-255 (10-255 slider)
  uint8_t screen_timeout_s;  // 5-255s, 0=never
  bool always_on;            // Modo always on (dragão acordado)
  uint8_t
      dragon_theme; // 0-11 (12 temas: Cyber, Retro, Kawaii, Evil, Flipper...)
  uint8_t dragon_eye_color; // 0-31 (32 cores neon)
  uint8_t dragon_size;      // 0=Pequeno, 1=Médio, 2=Grande
  uint8_t buzzer_volume;    // 0-100%
  bool sound_on_handshake;  // Tocar som ao capturar
  bool vibrate_on_boot;     // Vibração ao ligar
  bool invert_colors;       // Modo stealth noturno
  bool show_fps;            // Mostrar FPS no canto
  uint8_t system_font;      // 0=Neuropol, 1=Pixel, 2=OCR-A

  // ============================================================
  // REDE & WIRELESS (13-28) - Persistido
  // ============================================================
  uint8_t wifi_tx_power_dbm; // 8-20 dBm
  int8_t wifi_channel;       // -1=auto, 1-13=fixo
  uint8_t custom_mac[6];     // MAC customizado
  bool use_custom_mac;       // Usar MAC customizado
  bool mac_spoof_on_boot;    // Randomizar MAC a cada boot
  bool radio_2ghz_enabled;   // Ativar 2.4 GHz

  bool beacon_spam_enabled;  // Beacon spam ativo
  uint8_t beacon_spam_speed; // 1-10 velocidade
  uint8_t beacon_ssid_count; // Quantidade de SSIDs
  bool evil_twin_auto;       // Evil Twin automático
  bool karma_mana_enabled;   // Karma + Mana attack
  bool deauth_all_networks;  // Deauth em todas as redes
  bool smart_deauth_enabled; // Deauth inteligente (só clientes)
  bool turbo_deauth_enabled; // Turbo 300pps
  bool ghost_ap_enabled;     // AP fantasma (pisca)
  bool monitor_mode_only;    // Desliga STA, só monitor
  bool ble_spam_enabled;     // BLE Spam ativo
  uint8_t ble_spam_type;     // 0=Apple, 1=Windows, 2=Samsung

  bool promiscuous_mode; // Forçar modo promiscuous

  // ============================================================
  // ATAQUES & AUTOMATIZAÇÃO (29-42) - Persistido
  // ============================================================
  bool wps_pixie_auto;           // WPS Pixie Dust automático
  bool wps_bruteforce;           // WPS brute-force online
  uint8_t wps_flood_intensity;   // 1-10 intensidade DoS
  uint8_t rogue_ap_count;        // Quantidade de APs falsos (1-50)
  bool auto_capture_new_only;    // Auto-captura só redes novas
  bool pmkid_only_mode;          // Captura apenas PMKID (sem clientes)
  bool handshake_sniper_enabled; // Só ataca se tiver clientes + sinal forte
  bool auto_save_pcap;           // Auto-salvar .pcap a cada 500 pkts
  bool auto_attack_favorites;    // Auto-ataque em redes favoritas
  uint8_t favorite_count;        // Quantidade de redes favoritas
  bool insane_mode_enabled;      // Tudo ligado por 60s
  bool eco_mode_enabled; // Modo Eco: ataques mais lentos, economiza bateria
                         // (Tip 30)

  bool captive_portal_auto;    // Portal cativo automático
  uint8_t captive_template_id; // 0-9 templates de portal
  bool badusb_auto;            // BadUSB automático ao conectar PC
  uint8_t usb_payload_id;      // ID do payload padrão (0-9)

  // ============================================================
  // SISTEMA & ENERGIA (43-54) - Persistido
  // ============================================================
  uint8_t deep_sleep_minutes;    // 0=desabilitado, 1-255 minutos
  bool wake_on_touch;            // Acordar com toque
  bool wake_on_shake;            // Acordar com acelerômetro
  bool big_battery_display;      // Mostrar % bateria gigante
  bool stealth_boot;             // Boot stealth (tela preta + ponto ciano)
  int8_t timezone_offset;        // Offset do fuso horário (-12 a +14)
  bool panic_button_enabled;     // Dica 10: Panic Button ativo
  uint8_t panic_button_duration; // Duração do long press (em segundos)

  // ============================================================
  // PERSONALIDADE DO DRAGÃO (55-60) - Persistido
  // ============================================================
  char dragon_name[16];       // Nome editável do dragão
  uint8_t dragon_personality; // 0=Aggro, 1=Chill, 2=Psycho, 3=Cute, 4=Hacker,
                              // 5=Sleepy
  bool dragon_talk_enabled;   // Fala do dragão on/off
  uint8_t dragon_talk_lang;   // 0=PT-BR, 1=EN
  uint8_t dragon_talk_freq;   // 0=never, 1=raro, 2=normal, 3=spam
  bool dragon_exaggerated;    // Reações exageradas
  bool pet_only_mode;         // Só o dragão na tela, sem menu

  // ============================================================
  // AI & INTEGRAÇÃO (61-65) - Persistido
  // ============================================================
  char openai_api_key[128];
  char openai_model[32];
  char openai_voice[16];
  bool chatgpt_enabled;

  // ============================================================
  // RUNTIME STATE (não persistido - derivado/temporário)
  // ============================================================
  bool wifi_enabled;        // WiFi ligado/desligado
  bool ble_enabled;         // BLE ligado/desligado
  bool sleep_mode;          // Em modo sleep
  bool mascot_enabled;      // Mascote visível
  bool audio_enabled;       // Áudio habilitado
  uint8_t audio_volume;     // Volume atual (0-100)
  uint32_t scan_time_ms;    // Tempo de scan em ms
  bool auto_attack_enabled; // Auto-attack ligado
  bool auto_deauth;         // Auto-deauth ligado
  bool auto_capture;        // Auto-capture ligado
  bool show_clock;          // Mostrar relógio
  bool voice_enabled;       // Voice assistant ligado
  bool ghost_mode_enabled;  // Dica 16: Ghost Mode (Stealth total)
  UITheme current_theme;    // Tema visual atual

  // ============================================================
  // WEB DASHBOARD SECURITY (Dicas 66, 69, 30)
  // ============================================================
  bool hide_version;         // Ocultar versão do firmware em modo público
  uint8_t login_attempts;    // Contador de tentativas de login falhas
  uint32_t lockout_until;    // Timestamp até quando está bloqueado
  char discord_webhook[256]; // URL do webhook Discord
};

// Global flag for aggressive performance optimization (Tip 19)
extern bool g_suspend_ble_lvgl;

extern GlobalState g_state;
extern Arduino_GFX *g_display;
