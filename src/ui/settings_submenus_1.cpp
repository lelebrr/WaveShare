#include "../core/globals.h"
#include "notifications_engine.h"
#include "settings_menu_v2.h"
#include "sounds_manager.h"
#include "wallpaper_system.h"


// Slider value update callback
static void slider_value_cb(lv_event_t *e) {
  lv_obj_t *slider = lv_event_get_target(e);
  lv_obj_t *valLbl = (lv_obj_t *)lv_event_get_user_data(e);
  if (valLbl)
    lv_label_set_text_fmt(valLbl, "%d", (int)lv_slider_get_value(slider));
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 1: DISPLAY & INTERFACE (14 items)
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createDisplaySubmenu(lv_obj_t *parent) {
  createSlider(parent, "Brilho da Tela", 10, 100,
               g_state.screen_brightness * 100 / 255, slider_value_cb);
  createSlider(parent, "Tempo Até Apagar (s)", 5, 255, g_state.screen_timeout_s,
               slider_value_cb);
  createSwitch(parent, "Always On Display", g_state.always_on, nullptr);
  createButton(parent, "Wallpaper Atual", "Galeria", nullptr);
  createDropdown(parent, "Cor da Fonte",
                 "Cyan\nMagenta\nVerde\nAmarelo\nRoxo\nVermelho\nBranco", 0,
                 nullptr);
  createSlider(parent, "Transparência Menu", 0, 90,
               wallpaper_system.getMenuTransparency(), slider_value_cb);
  createDropdown(parent, "Estilo Ícones", "Flutuante\nBorda Neon\nMinimalista",
                 0, nullptr);
  createDropdown(parent, "Tamanho Ícones", "Pequeno\nMédio\nGrande",
                 g_state.dragon_size, nullptr);
  createDropdown(parent, "Tema Geral", "Cyberpunk\nStealth\nRetro\nKawaii", 0,
                 nullptr);
  createSwitch(parent, "Inverter Cores", g_state.invert_colors, nullptr);
  createSwitch(parent, "Bateria Gigante", g_state.big_battery_display, nullptr);
  createDropdown(parent, "Formato Relógio", "24h\n12h", 0, nullptr);
  createDropdown(
      parent, "Fonte Sistema",
      "Neuropol\nOrbitron\nShare Tech\nExo 2\nPixel\nOCR-A\nMonospace",
      g_state.system_font, nullptr);
  createSwitch(parent, "Mostrar FPS", g_state.show_fps, nullptr);
  return parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 2: DRAGÃO & PERSONALIDADE (8 items)
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createDragonSubmenu(lv_obj_t *parent) {
  createButton(parent, "Nome do Dragão", g_state.dragon_name, nullptr);
  createDropdown(parent, "Personalidade",
                 "Aggressive\nCute\nPsycho\nLazy\nHacker\nEvil",
                 g_state.dragon_personality, nullptr);
  createDropdown(parent, "Cor dos Olhos",
                 "Cyan\nMagenta\nVerde\nAmarelo\nVermelho\nRoxo\nBranco",
                 g_state.dragon_eye_color, nullptr);
  createDropdown(parent, "Tamanho", "Pequeno\nMédio\nGrande",
                 g_state.dragon_size, nullptr);
  createSwitch(parent, "Fala Ativada", g_state.dragon_talk_enabled, nullptr);
  createDropdown(parent, "Idioma", "PT-BR\nEnglish", g_state.dragon_talk_lang,
                 nullptr);
  createDropdown(parent, "Frequência Falas", "Nunca\nRaro\nNormal\nSpam",
                 g_state.dragon_talk_freq, nullptr);
  createSwitch(parent, "Reações Exageradas", g_state.dragon_exaggerated,
               nullptr);
  createSwitch(parent, "Modo Pet Only", g_state.pet_only_mode, nullptr);
  return parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 3: SOM & VIBRAÇÃO (12 items)
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createSoundSubmenu(lv_obj_t *parent) {
  createSlider(parent, "Volume Master", 0, 100, sounds_manager.getVolume(),
               slider_value_cb);
  createSwitch(parent, "Som ao Handshake",
               sounds_manager.getConfig().soundOnHandshake, nullptr);
  createSwitch(parent, "Som ao Ligar", sounds_manager.getConfig().soundOnBoot,
               nullptr);
  createSwitch(parent, "Som ao Ataque",
               sounds_manager.getConfig().soundOnAttack, nullptr);
  createButton(parent, "Biblioteca de Sons", "Abrir", nullptr);
  createButton(parent, "Testar Som", "▶ Play", nullptr);
  createSwitch(parent, "Vibração Master", g_state.vibrate_on_boot, nullptr);
  createDropdown(parent, "Padrão Vibração",
                 "Curta ·\nDupla ··\nLonga —\nCoração\nAlerta\nMorse SOS", 0,
                 nullptr);
  createButton(parent, "Testar Vibração", "Testar", nullptr);
  createSlider(parent, "Volume Buzzer", 0, 100, g_state.buzzer_volume,
               slider_value_cb);
  createSwitch(parent, "Som Handshake", g_state.sound_on_handshake, nullptr);
  createButton(parent, "Upload Sons (Web)", "→", nullptr);
  return parent;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUBMENU 4: NOTIFICAÇÕES VISUAIS (12 items)
// ═══════════════════════════════════════════════════════════════════════════
lv_obj_t *SettingsMenuV2::createNotificationsSubmenu(lv_obj_t *parent) {
  NotificationConfig cfg = notifications_engine.getConfig();

  createSwitch(parent, "Notificações Visuais", cfg.enabled, nullptr);
  createDropdown(
      parent, "Tipo de Efeito",
      "Flash Total\nBordas Neon\nPulso Central\nScanline\nGlitch\nDragão Pisca",
      cfg.effect, nullptr);
  createButton(parent, "Cor da Notificação", "Paleta", nullptr);
  createSlider(parent, "Duração (ms)", 300, 3000, cfg.duration_ms,
               slider_value_cb);
  createDropdown(parent, "Intensidade Fade", "Suave\nMédio\nAgressivo",
                 cfg.fade, nullptr);
  createButton(parent, "🔔 TESTAR", "TESTAR", nullptr);
  // Event checkboxes
  createSwitch(parent, "→ Novo Handshake", cfg.event_enabled[0], nullptr);
  createSwitch(parent, "→ WPS PIN", cfg.event_enabled[1], nullptr);
  createSwitch(parent, "→ BLE Próximo", cfg.event_enabled[2], nullptr);
  createSwitch(parent, "→ Bateria < 20%", cfg.event_enabled[3], nullptr);
  createSwitch(parent, "→ SD Quase Cheio", cfg.event_enabled[4], nullptr);
  createSwitch(parent, "→ Ataque Concluído", cfg.event_enabled[5], nullptr);
  return parent;
}
