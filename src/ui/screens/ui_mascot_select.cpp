/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Tela de Seleção de Mascotes (LVGL)
 * Interface para selecionar e nomear mascotes
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "ui_mascot_select.h"
#include "../../core/globals.h"
#include "../../mascot/mascot_manager.h"
#include <lvgl.h>

// Forward declarations
static void mascot_card_clicked(lv_event_t *e);
static void name_input_changed(lv_event_t *e);
static void save_btn_clicked(lv_event_t *e);
static void back_btn_clicked(lv_event_t *e);

// Screen objects
static lv_obj_t *screen = nullptr;
static lv_obj_t *name_input = nullptr;
static lv_obj_t *preview_label = nullptr;
static uint8_t selected_mascot = 0;

// ═══════════════════════════════════════════════════════════════════════════
// PUBLIC FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void ui_mascot_select_init() {
  // Nothing to init
}

lv_obj_t *ui_mascot_select_create() {
  screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x0a0a0f), 0);

  // Header
  lv_obj_t *header = lv_label_create(screen);
  lv_label_set_text(header, "🐉 Escolha seu Mascote");
  lv_obj_set_style_text_color(header, lv_color_hex(0x00fff5), 0);
  lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 10);

  // Preview area
  lv_obj_t *preview_cont = lv_obj_create(screen);
  lv_obj_set_size(preview_cont, 200, 100);
  lv_obj_align(preview_cont, LV_ALIGN_TOP_MID, 0, 40);
  lv_obj_set_style_bg_color(preview_cont, lv_color_hex(0x1a1a2e), 0);
  lv_obj_set_style_border_color(preview_cont, lv_color_hex(0x00fff5), 0);
  lv_obj_set_style_border_width(preview_cont, 2, 0);
  lv_obj_set_style_radius(preview_cont, 10, 0);

  // Current mascot emoji (big)
  preview_label = lv_label_create(preview_cont);
  lv_label_set_text(preview_label, mascot_manager.getCurrentMascotInfo().emoji);
  lv_obj_set_style_text_font(preview_label, &lv_font_montserrat_48, 0);
  lv_obj_center(preview_label);

  // Name input
  name_input = lv_textarea_create(screen);
  lv_textarea_set_one_line(name_input, true);
  lv_textarea_set_max_length(name_input, 16);
  lv_textarea_set_placeholder_text(name_input, "Nome do mascote");
  lv_textarea_set_text(name_input, mascot_manager.getName());
  lv_obj_set_width(name_input, 180);
  lv_obj_align(name_input, LV_ALIGN_TOP_MID, 0, 150);
  lv_obj_add_event_cb(name_input, name_input_changed, LV_EVENT_VALUE_CHANGED,
                      nullptr);

  // Mascot grid
  lv_obj_t *grid = lv_obj_create(screen);
  lv_obj_set_size(grid, 220, 140);
  lv_obj_align(grid, LV_ALIGN_CENTER, 0, 30);
  lv_obj_set_style_bg_opa(grid, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_opa(grid, LV_OPA_TRANSP, 0);
  lv_obj_set_layout(grid, LV_LAYOUT_GRID);

  // 4x2 grid for 8 mascots
  static lv_coord_t col_dsc[] = {50, 50, 50, 50, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {55, 55, LV_GRID_TEMPLATE_LAST};
  lv_obj_set_grid_dsc_array(grid, col_dsc, row_dsc);

  // Create mascot cards
  for (uint8_t i = 0; i < MascotManager::getMascotCount(); i++) {
    const MascotInfo &info = mascot_manager.getMascotInfo((MascotType)i);

    lv_obj_t *card = lv_btn_create(grid);
    lv_obj_set_size(card, 45, 50);
    lv_obj_set_grid_cell(card, LV_GRID_ALIGN_CENTER, i % 4, 1,
                         LV_GRID_ALIGN_CENTER, i / 4, 1);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_radius(card, 8, 0);
    lv_obj_add_event_cb(card, mascot_card_clicked, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)i);

    // Highlight current
    if (i == mascot_manager.getMascot()) {
      lv_obj_set_style_border_color(card, lv_color_hex(0x00fff5), 0);
      lv_obj_set_style_border_width(card, 2, 0);
      selected_mascot = i;
    }

    lv_obj_t *emoji = lv_label_create(card);
    lv_label_set_text(emoji, info.emoji);
    lv_obj_center(emoji);
  }

  // Save button
  lv_obj_t *save_btn = lv_btn_create(screen);
  lv_obj_set_size(save_btn, 100, 35);
  lv_obj_align(save_btn, LV_ALIGN_BOTTOM_MID, 0, -40);
  lv_obj_set_style_bg_color(save_btn, lv_color_hex(0x00fff5), 0);
  lv_obj_add_event_cb(save_btn, save_btn_clicked, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *save_label = lv_label_create(save_btn);
  lv_label_set_text(save_label, "Salvar");
  lv_obj_set_style_text_color(save_label, lv_color_black(), 0);
  lv_obj_center(save_label);

  // Back button
  lv_obj_t *back_btn = lv_btn_create(screen);
  lv_obj_set_size(back_btn, 40, 30);
  lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 10, 5);
  lv_obj_set_style_bg_color(back_btn, lv_color_hex(0x333344), 0);
  lv_obj_add_event_cb(back_btn, back_btn_clicked, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *back_label = lv_label_create(back_btn);
  lv_label_set_text(back_label, "<");
  lv_obj_center(back_label);

  return screen;
}

void ui_mascot_select_show() {
  if (!screen) {
    ui_mascot_select_create();
  }
  lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_FADE_IN, 200, 0, false);
}

// ═══════════════════════════════════════════════════════════════════════════
// EVENT HANDLERS
// ═══════════════════════════════════════════════════════════════════════════

static void mascot_card_clicked(lv_event_t *e) {
  uint8_t id = (uint8_t)(uintptr_t)lv_event_get_user_data(e);
  selected_mascot = id;

  // Update preview
  const MascotInfo &info = mascot_manager.getMascotInfo((MascotType)id);
  lv_label_set_text(preview_label, info.emoji);

  // Update visual selection (rebuild screen)
  ui_mascot_select_show();
}

static void name_input_changed(lv_event_t *e) {
  // Just track changes, save on button click
}

static void save_btn_clicked(lv_event_t *e) {
  // Set mascot
  mascot_manager.setMascot((MascotType)selected_mascot);

  // Set name
  const char *name = lv_textarea_get_text(name_input);
  if (name && name[0] != '\0') {
    mascot_manager.setName(name);
  }

  // Go back
  extern void ui_show_settings();
  ui_show_settings();
}

static void back_btn_clicked(lv_event_t *e) {
  extern void ui_show_settings();
  ui_show_settings();
}
