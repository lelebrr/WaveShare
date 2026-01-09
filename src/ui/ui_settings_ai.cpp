#include "ui_settings_ai.h"
#include "../core/config_manager.h"
#include "../core/globals.h"
#include "ui_settings.h"
#include "ui_themes.h"
#include "widgets/ui_settings_widgets.h"

static lv_obj_t *scr_ai = nullptr;
static bool is_active = false;
static lv_obj_t *lbl_apikey_status = nullptr;

static void on_enabled_change(bool checked) {
  g_state.chatgpt_enabled = checked;
  config_manager.saveAiSettings();
}

static void on_voice_change(uint16_t selected) {
  // Map index to string
  const char *voices[] = {"alloy",   "echo",   "fable",   "onyx",  "nova",
                          "shimmer", "ash",    "ballad",  "coral", "sage",
                          "verse",   "breeze", "jupiter", "sol",   "ember"};
  if (selected < 15) {
    strcpy(g_state.openai_voice, voices[selected]);
    config_manager.saveAiSettings();
  }
}

static void on_model_change(uint16_t selected) {
  const char *models[] = {"gpt-4o-mini", "gpt-4o", "gpt-4o-audio-preview"};
  if (selected < 3) {
    strcpy(g_state.openai_model, models[selected]);
    config_manager.saveAiSettings();
  }
}

static void on_back_click(lv_event_t *e) {
  ui_settings_ai_hide();
  ui_settings_show();
}

static void create_screen() {
  lv_obj_t *content = ui_create_settings_screen(
      &scr_ai, LV_SYMBOL_SETTINGS " Inteligencia Artificial", on_back_click);

  ui_create_switch_row(content, "Habilitar ChatGPT", LV_SYMBOL_POWER,
                       g_state.chatgpt_enabled, on_enabled_change);

  // API Key Status
  const char *status =
      (strlen(g_state.openai_api_key) > 5) ? "Configurada" : "Nao Configurada";
  lbl_apikey_status =
      ui_create_info_row(content, "API Key", LV_SYMBOL_KEYBOARD, status);

  // Voice Dropdown
  // Index finding
  int current_voice_idx = 0;
  const char *voices[] = {"alloy",   "echo",   "fable",   "onyx",  "nova",
                          "shimmer", "ash",    "ballad",  "coral", "sage",
                          "verse",   "breeze", "jupiter", "sol",   "ember"};
  for (int i = 0; i < 15; i++) {
    if (strcmp(g_state.openai_voice, voices[i]) == 0) {
      current_voice_idx = i;
      break;
    }
  }

  ui_create_dropdown_row(content, "Voz", LV_SYMBOL_AUDIO,
                         "Alloy\nEcho\nFable\nOnyx\nNova\nShimmer\nAsh\nBallad"
                         "\nCoral\nSage\nVerse\nBreeze\nJupiter\nSol\nEmber",
                         current_voice_idx, on_voice_change);

  // Model Dropdown
  int current_model_idx = 0;
  if (strcmp(g_state.openai_model, "gpt-4o") == 0)
    current_model_idx = 1;
  else if (strcmp(g_state.openai_model, "gpt-4o-audio-preview") == 0)
    current_model_idx = 2;

  ui_create_dropdown_row(content, "Modelo", LV_SYMBOL_COPY,
                         "GPT-4o Mini\nGPT-4o\nGPT-4o Audio", current_model_idx,
                         on_model_change);

  lv_obj_t *tips = lv_label_create(content);
  lv_label_set_text(tips, "Configure a API Key pela Interface Web.");
  lv_obj_set_style_text_font(tips, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(tips, lv_color_hex(0x888888), 0);
  lv_obj_set_width(tips, lv_pct(90));
  lv_label_set_long_mode(tips, LV_LABEL_LONG_WRAP);
}

void ui_settings_ai_show() {
  if (!scr_ai)
    create_screen();
  is_active = true;
  lv_scr_load(scr_ai);
}

void ui_settings_ai_hide() {
  is_active = false;
  if (scr_ai) {
    lv_obj_del(scr_ai);
    scr_ai = nullptr;
  }
}

bool ui_settings_ai_is_active() { return is_active; }
