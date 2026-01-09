/**
 * @file ui_home.cpp
 * @brief Implementação corrigida da tela Home
 */

#include "ui_home.h"
#include "../core/globals.h"
#include "ui_avatar.h"
#include "ui_helpers.h"
#include "ui_main.h"
#include "ui_themes.h"

// Static Vars
static bool _initialized = false;
static lv_obj_t *_statsContainer = nullptr;
static lv_obj_t *_lblNetworks = nullptr;
static lv_obj_t *_lblHandshakes = nullptr;
static lv_obj_t *_lblPmkid = nullptr;
static lv_obj_t *_lblUptime = nullptr;

// Helper styles (local for now or ensure globals)
static void create_stat_card(lv_obj_t *parent, const char *icon,
                             const char *label, lv_obj_t **val_label,
                             uint32_t val_default) {
  // Config styles
  static lv_style_t style_card_local;
  static bool style_init = false;
  if (!style_init) {
    lv_style_init(&style_card_local);
    lv_style_set_bg_color(&style_card_local, lv_color_hex(0x101010));
    lv_style_set_bg_opa(&style_card_local, LV_OPA_COVER);
    lv_style_set_border_width(&style_card_local, 1);
    lv_style_set_border_color(&style_card_local, lv_color_hex(0x333333));
    lv_style_set_radius(&style_card_local, 8);
    style_init = true;
  }

  lv_obj_t *card = lv_obj_create(parent);
  lv_obj_add_style(card, &style_card_local, 0);
  lv_obj_set_size(
      card, 85,
      85); // Slightly smaller to fit 4 in 368px width (85*4 + margins)
  lv_obj_set_flex_flow(card, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(card, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(card, 2, 0);
  lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

  // Value (Large)
  *val_label = lv_label_create(card);
  lv_obj_set_style_text_font(*val_label, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(*val_label, COLOR_NEON_GREEN, 0);
  lv_label_set_text_fmt(*val_label, "%lu", (unsigned long)val_default);

  // Icon + Label (Small)
  lv_obj_t *lbl = lv_label_create(card);
  lv_label_set_text_fmt(lbl, "%s %s", icon, label);
  lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12,
                             0); // 10 might not be enabled
  lv_obj_set_style_text_color(lbl, lv_color_hex(0xaaaaaa), 0);
}

void ui_home_init() { _initialized = true; }

void ui_home_show() {
  lv_obj_t *content = ui_get_content_area();
  if (!content)
    return;

  ui_clear_content_area();

  // Layout: Column
  lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(content, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(content, 0, 0);

  // 1. Dragon Area (Top ~70%)
  lv_obj_t *avatarWrapper = lv_obj_create(content);
  lv_obj_set_size(avatarWrapper, 260, 260);
  lv_obj_set_style_bg_opa(avatarWrapper, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(avatarWrapper, 0, 0);
  lv_obj_clear_flag(avatarWrapper, LV_OBJ_FLAG_SCROLLABLE);

  voiceAvatar.create(avatarWrapper);

  // 2. Stats Row (Bottom)
  _statsContainer = lv_obj_create(content);
  lv_obj_set_size(_statsContainer, LV_PCT(100), 100);
  lv_obj_set_style_bg_opa(_statsContainer, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(_statsContainer, 0, 0);
  lv_obj_set_flex_flow(_statsContainer, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(_statsContainer, LV_FLEX_ALIGN_SPACE_EVENLY,
                        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_clear_flag(_statsContainer, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_pad_all(_statsContainer, 0, 0);

  // Card 1: Networks
  // Use generic symbols if specific ones undefined, ensuring code compiles
  create_stat_card(_statsContainer, LV_SYMBOL_WIFI, "Redes", &_lblNetworks,
                   g_state.networks_seen);

  // Card 2: Handshakes
  create_stat_card(
      _statsContainer, LV_SYMBOL_SD_CARD, "Hands",
      &_lblHandshakes, // SD_CARD as generic storage icon or KEY if available
      g_state.handshakes_captured);

  // Card 3: PMKID
  create_stat_card(_statsContainer, LV_SYMBOL_FILE, "PMKID", &_lblPmkid, 0);

  // Card 4: Uptime
  create_stat_card(_statsContainer, LV_SYMBOL_LOOP, "Tempo", &_lblUptime,
                   g_state.uptime_seconds / 60);
}

void ui_home_update() {
  if (!_initialized || !_lblNetworks)
    return;

  // Update Stats
  lv_label_set_text_fmt(_lblNetworks, "%lu",
                        (unsigned long)g_state.networks_seen);
  lv_label_set_text_fmt(_lblHandshakes, "%u",
                        (unsigned int)g_state.handshakes_captured);
  lv_label_set_text_fmt(_lblPmkid, "%u", 0);

  // Uptime
  uint32_t uptime_min = 0;
  if (g_state.uptime_seconds > 0)
    uptime_min = g_state.uptime_seconds / 60;

  if (uptime_min < 60)
    lv_label_set_text_fmt(_lblUptime, "%lum", (unsigned long)uptime_min);
  else
    lv_label_set_text_fmt(_lblUptime, "%luh", (unsigned long)(uptime_min / 60));

  // Update Avatar
  voiceAvatar.update();
}

void ui_home_set_assistant_text(const char *text) { voiceAvatar.setText(text); }

void ui_home_open_menu(HomeMenuCategory category) {
  // Placeholder
  (void)category;
}
