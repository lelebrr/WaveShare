/**
 * @file ui_main.cpp
 * @brief Interface principal WavePwn com LVGL
 */

#include "ui_main.h"
#include "../core/globals.h"
#include "../hardware/system_hardware.h"
#include "../hardware/wifi_driver.h"
#include "gesture_handler.h"
#include "mascot_faces.h"
#include "status_bar.h"
#include "ui_attacks.h"
#include "ui_ble_chaos.h"
#include "ui_captures.h"
#include "ui_helpers.h"
#include "ui_home.h"
#include "ui_lockscreen.h"
#include "ui_notifications.h"
#include "ui_settings.h"


// Objetos LVGL
static lv_obj_t *scr_main = nullptr;
static lv_obj_t *lbl_mood = nullptr;
static lv_obj_t *mascot_container = nullptr;
static lv_obj_t *dragon_emoji = nullptr;

// Estado da UI
static UIScreen current_screen = UI_SCREEN_MAIN;
static bool ui_initialized = false;
static lv_timer_t *update_timer = nullptr;

// Cores do tema UI
#define UI_COLOR_BG lv_color_black()
#define UI_COLOR_PANEL lv_color_hex(0x111111)

// Forward declarations
static void ui_update_timer_cb(lv_timer_t *timer);
static void init_styles();

// Global Content Area (Zone 2)
static lv_obj_t *content_area = nullptr;
lv_obj_t *ui_get_content_area() { return content_area; }
void ui_clear_content_area() {
  if (content_area)
    lv_obj_clean(content_area);
}

// Timer Callback
static void ui_update_timer_cb(lv_timer_t *timer) {
  if (!ui_initialized)
    return;

  // Mascot updates
  mascotFaces.update();
  // We can update mascot elements here if references are kept,
  // but ui_home handles the mascot instance now.
  // Kept legacy logic if needed or for global overlay.
}

static void init_styles() { ui_init_global_styles(); }

bool ui_main_init() {
  if (ui_initialized)
    return true;

  Serial.println("[UI] Iniciando interface principal...");
  init_styles();

  scr_main = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr_main, UI_COLOR_BG, 0);
  lv_obj_clear_flag(scr_main, LV_OBJ_FLAG_SCROLLABLE);

  // Status Bar
  statusBar.create(scr_main);

  // Content Area & Nav
  ui_create_3_zone_layout(scr_main, &content_area);

  // Default Home
  ui_home_show();

  lv_scr_load(scr_main);
  update_timer = lv_timer_create(ui_update_timer_cb, 500, nullptr);

  // Gestures
  gesture_handler.begin();
  gesture_handler.setCallback([](GestureType gesture, int16_t x, int16_t y) {
    switch (gesture) {
    case GESTURE_SWIPE_UP:
      Serial.println("[Gesture] Swipe Up -> Notifications");
      ui_notification_center_show();
      break;
    case GESTURE_SWIPE_DOWN:
      Serial.println("[Gesture] Swipe Down -> Settings");
      ui_settings_show();
      break;
    case GESTURE_SWIPE_LEFT:
      if (ui_get_current_screen() == UI_SCREEN_MAIN) {
        ui_set_screen(UI_SCREEN_WIFI_CHAOS); // Or attacks
      } else if (ui_get_current_screen() == UI_SCREEN_WIFI_CHAOS) {
        ui_set_screen(UI_SCREEN_BLE_CHAOS);
      }
      break;
    case GESTURE_SWIPE_RIGHT:
      if (ui_get_current_screen() == UI_SCREEN_BLE_CHAOS) {
        ui_set_screen(UI_SCREEN_WIFI_CHAOS);
      } else if (ui_get_current_screen() == UI_SCREEN_WIFI_CHAOS) {
        ui_set_screen(UI_SCREEN_MAIN);
      }
      break;
    default:
      break;
    }
  });

  lv_timer_create([](lv_timer_t *t) { gesture_handler.update(); }, 20, nullptr);

  ui_initialized = true;
  current_screen = UI_SCREEN_MAIN;
  Serial.println("[UI] Interface pronta!");
  return true;
}

void ui_set_mood_text(const char *text) {
  if (lbl_mood)
    lv_label_set_text(lbl_mood, text);
}

UIScreen ui_get_current_screen() { return current_screen; }

static lv_scr_load_anim_t current_transition_anim = LV_SCR_LOAD_ANIM_FADE_IN;

void ui_set_screen(UIScreen screen) {
  current_screen = screen;
  lv_obj_t *target = nullptr;

  switch (screen) {
  case UI_SCREEN_MAIN:
    target = scr_main;
    break;
  case UI_SCREEN_ATTACKS:
  case UI_SCREEN_WIFI_CHAOS:
    ui_attacks_show(); // Mapped to attacks for now
    return;
  case UI_SCREEN_BLE_CHAOS:
    ui_ble_chaos_show();
    return;
  case UI_SCREEN_SETTINGS:
    ui_settings_show();
    return;
  case UI_SCREEN_LOCK:
    ui_lockscreen_show();
    return;
  case UI_SCREEN_CAPTURES:
    ui_captures_show();
    return;
  case UI_SCREEN_PLUGINS:
    // ui_plugins_show();
    return;
  default:
    target = scr_main;
    break;
  }

  if (target) {
    ui_switch_screen(target, current_transition_anim);
  }
}

void ui_set_transition_animation(uint8_t anim_type) {
  switch (anim_type) {
  case 0:
    current_transition_anim = LV_SCR_LOAD_ANIM_NONE;
    break;
  case 1:
    current_transition_anim = LV_SCR_LOAD_ANIM_FADE_IN;
    break;
  case 2:
    current_transition_anim = LV_SCR_LOAD_ANIM_MOVE_LEFT;
    break;
  case 3:
    current_transition_anim = LV_SCR_LOAD_ANIM_MOVE_RIGHT;
    break;
  case 4:
    current_transition_anim = LV_SCR_LOAD_ANIM_MOVE_TOP;
    break;
  case 5:
    current_transition_anim = LV_SCR_LOAD_ANIM_MOVE_BOTTOM;
    break;
  case 6:
    current_transition_anim = LV_SCR_LOAD_ANIM_OVER_LEFT;
    break;
  case 7:
    current_transition_anim = LV_SCR_LOAD_ANIM_OVER_RIGHT;
    break;
  default:
    current_transition_anim = LV_SCR_LOAD_ANIM_FADE_IN;
    break;
  }
}

void ui_main_show() {
  // If we are already on main screen object, ensuring zones are cleared or
  // reset might be needed, but usually ui_home_show() handles content area. We
  // just switch back to scr_main.
  if (scr_main) {
    ui_switch_screen(scr_main, current_transition_anim);
    current_screen = UI_SCREEN_MAIN;
    // Refresh home content if needed
    ui_home_show();
  }
}

void ui_set_mascot_face(MascotFace face) {
  if (!g_state.mascot_enabled)
    return;

  // Delegate to global helper or updated logic
  MascotFaceType newFace = FACE_HAPPY;
  switch (face) {
  case MASCOT_FACE_NORMAL:
    newFace = FACE_HAPPY;
    break;
  case MASCOT_FACE_HAPPY:
    newFace = FACE_EXCITED;
    break;
  case MASCOT_FACE_ANGRY:
    newFace = FACE_ANGRY;
    break;
  case MASCOT_FACE_SLEEP:
    newFace = FACE_SLEEP;
    break;
  case MASCOT_FACE_CONFUSED:
    newFace = FACE_CONFUSED;
    break;
  case MASCOT_FACE_COOL:
    newFace = FACE_COOL;
    break;
  }
  mascotFaces.setFace(newFace);
}
