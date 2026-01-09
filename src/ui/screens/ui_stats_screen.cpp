/**
 * @file ui_stats_screen.cpp
 * @brief Implementação da tela de estatísticas
 */

#include "ui_stats_screen.h"
#include "../../plugins/age_tracker.h"
#include "../../plugins/battery_monitor.h"
#include "../../plugins/exp_system.h"
#include "../../plugins/memtemp.h"
#include "../../plugins/session_stats.h"
#include "../ui_main.h"

StatsScreen statsScreen;

StatsScreen::StatsScreen() : _screen(nullptr) {}

void StatsScreen::create(lv_obj_t *parent) {
  _screen = lv_obj_create(parent);
  lv_obj_set_size(_screen, lv_pct(100), lv_pct(100));
  lv_obj_set_style_bg_color(_screen, lv_color_hex(0x0a0a1a), 0);
  lv_obj_set_style_border_width(_screen, 0, 0);
  lv_obj_set_style_pad_all(_screen, 10, 0);
  lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);

  // Título
  lv_obj_t *title = lv_label_create(_screen);
  lv_label_set_text(title, "📊 Estatísticas");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(title, lv_color_hex(0x00ff88), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

  // Container para stats
  lv_obj_t *cont = lv_obj_create(_screen);
  lv_obj_set_size(cont, lv_pct(100), lv_pct(70));
  lv_obj_align(cont, LV_ALIGN_CENTER, 0, -10);
  lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(cont, 0, 0);
  lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_row(cont, 8, 0);

  // XP e Level
  _lblLevel = lv_label_create(cont);
  lv_label_set_text(_lblLevel, "Level: 1 - Hatchling");
  lv_obj_set_style_text_color(_lblLevel, lv_color_hex(0xffff00), 0);

  // Barra de XP
  _barXP = lv_bar_create(cont);
  lv_obj_set_size(_barXP, lv_pct(80), 15);
  lv_bar_set_range(_barXP, 0, 100);
  lv_bar_set_value(_barXP, 0, LV_ANIM_OFF);
  lv_obj_set_style_bg_color(_barXP, lv_color_hex(0x333355), 0);
  lv_obj_set_style_bg_color(_barXP, lv_color_hex(0x00ff88), LV_PART_INDICATOR);

  _lblXP = lv_label_create(cont);
  lv_label_set_text(_lblXP, "XP: 0/100");
  lv_obj_set_style_text_color(_lblXP, lv_color_hex(0xaaaaaa), 0);

  // Separador
  lv_obj_t *sep = lv_obj_create(cont);
  lv_obj_set_size(sep, lv_pct(90), 1);
  lv_obj_set_style_bg_color(sep, lv_color_hex(0x444444), 0);
  lv_obj_set_style_border_width(sep, 0, 0);

  // Stats de sessão
  _lblSession = lv_label_create(cont);
  lv_label_set_text(_lblSession, "Sessão: 00:00:00");
  lv_obj_set_style_text_color(_lblSession, lv_color_hex(0xcccccc), 0);

  _lblNetworks = lv_label_create(cont);
  lv_label_set_text(_lblNetworks, "Redes: 0");

  _lblHandshakes = lv_label_create(cont);
  lv_label_set_text(_lblHandshakes, "Handshakes: 0");
  lv_obj_set_style_text_color(_lblHandshakes, lv_color_hex(0x00ff88), 0);

  _lblDeauths = lv_label_create(cont);
  lv_label_set_text(_lblDeauths, "Deauths: 0");

  // Sistema
  _lblMemory = lv_label_create(cont);
  lv_label_set_text(_lblMemory, "RAM: 0KB | Temp: 0C");
  lv_obj_set_style_text_color(_lblMemory, lv_color_hex(0x888888), 0);

  _lblBattery = lv_label_create(cont);
  lv_label_set_text(_lblBattery, "Bateria: 100%");

  // Botão voltar
  _btnBack = lv_btn_create(_screen);
  lv_obj_set_size(_btnBack, 80, 35);
  lv_obj_align(_btnBack, LV_ALIGN_BOTTOM_MID, 0, -5);
  lv_obj_set_style_bg_color(_btnBack, lv_color_hex(0x333355), 0);
  lv_obj_add_event_cb(_btnBack, onBackClick, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *lblBack = lv_label_create(_btnBack);
  lv_label_set_text(lblBack, "← Voltar");
  lv_obj_center(lblBack);
}

void StatsScreen::show() {
  if (_screen) {
    lv_obj_clear_flag(_screen, LV_OBJ_FLAG_HIDDEN);
    update();
  }
}

void StatsScreen::hide() {
  if (_screen)
    lv_obj_add_flag(_screen, LV_OBJ_FLAG_HIDDEN);
}

void StatsScreen::update() {
  if (!_screen)
    return;

  char buf[64];

  // XP e Level
  snprintf(buf, sizeof(buf), "Level %d - %s", expPlugin.getLevel(),
           expPlugin.getTitle());
  lv_label_set_text(_lblLevel, buf);

  snprintf(buf, sizeof(buf), "XP: %u / %u", expPlugin.getTotalXP(),
           expPlugin.getXPForNextLevel());
  lv_label_set_text(_lblXP, buf);
  lv_bar_set_value(_barXP, expPlugin.getProgressPercent(), LV_ANIM_ON);

  // Sessão
  snprintf(buf, sizeof(buf), "Sessão: %s",
           agePlugin.getFormattedUptime().c_str());
  lv_label_set_text(_lblSession, buf);

  // Stats
  const SessionStats &stats = sessionPlugin.getStats();
  snprintf(buf, sizeof(buf), "Redes: %u", stats.networks_found);
  lv_label_set_text(_lblNetworks, buf);

  snprintf(buf, sizeof(buf), "Handshakes: %u", stats.handshakes_captured);
  lv_label_set_text(_lblHandshakes, buf);

  snprintf(buf, sizeof(buf), "Deauths: %u", stats.deauths_sent);
  lv_label_set_text(_lblDeauths, buf);

  // Sistema
  snprintf(buf, sizeof(buf), "RAM: %s | Temp: %s",
           memtempPlugin.getFormattedHeap().c_str(),
           memtempPlugin.getFormattedTemp().c_str());
  lv_label_set_text(_lblMemory, buf);

  snprintf(buf, sizeof(buf), "Bateria: %d%% %s", batteryPlugin.getPercent(),
           batteryPlugin.isCharging() ? "(Carregando)" : "");
  lv_label_set_text(_lblBattery, buf);
}

void StatsScreen::onBackClick(lv_event_t *e) {
  statsScreen.hide();
  ui_main_show();
}
