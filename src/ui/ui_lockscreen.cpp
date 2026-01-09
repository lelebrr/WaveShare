#include "ui_lockscreen.h"
#include "../core/globals.h"
#include "../hardware/system_hardware.h"
#include "mascot_faces.h"
#include "ui_helpers.h"
#include "ui_main.h"
#include "ui_themes.h"

static lv_obj_t *scr_lock = nullptr;
static lv_obj_t *lbl_clock = nullptr;
static lv_obj_t *lbl_date = nullptr;
static lv_timer_t *lock_timer = nullptr;

static void lock_timer_cb(lv_timer_t *t) {
  // Update Clock (Tip 34)
  if (lbl_clock) {
    unsigned long secs = millis() / 1000; // Replace with RTC
    int hrs = (secs / 3600) % 24;
    int mins = (secs / 60) % 60;
    lv_label_set_text_fmt(lbl_clock, "%02d:%02d", hrs, mins);
  }
  // Update Date...
}

static void unlock_cb(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_GESTURE) {
    if (lv_indev_get_gesture_dir(lv_indev_get_act()) == LV_DIR_TOP) {
      // Swipe Up to Unlock (Tip 35)
      ui_set_screen(UI_SCREEN_MAIN);
    }
  }
}

void ui_lockscreen_show() {
  if (scr_lock) {
    lv_scr_load(scr_lock);
    return;
  }

  scr_lock = lv_obj_create(nullptr);
  lv_obj_set_style_bg_color(scr_lock, lv_color_black(), 0);
  lv_obj_add_event_cb(scr_lock, unlock_cb, LV_EVENT_GESTURE, nullptr);
  lv_obj_clear_flag(scr_lock, LV_OBJ_FLAG_SCROLLABLE);

  // Giant Clock (Tip 34)
  lbl_clock = lv_label_create(scr_lock);
  lv_label_set_text(lbl_clock, "12:00");
  lv_obj_set_style_text_font(lbl_clock, &lv_font_montserrat_48,
                             0); // Need larger font
  // Ideally use a very large custom font if available, trying 48 for now.
  lv_obj_set_style_text_color(lbl_clock, lv_color_white(), 0);
  lv_obj_align(lbl_clock, LV_ALIGN_CENTER, 0, -40);

  // Sleeping Dragon
  lv_obj_t *dragon = lv_label_create(scr_lock);
  lv_label_set_text(dragon, "( -_-) Zzz..."); // Sleeping face
  lv_obj_set_style_text_font(dragon, &lv_font_montserrat_24, 0);
  lv_obj_set_style_text_color(dragon, COLOR_NEON_BLUE, 0);
  lv_obj_align(dragon, LV_ALIGN_CENTER, 0, 20);

  // Unlock Hint
  lv_obj_t *hint = lv_label_create(scr_lock);
  lv_label_set_text(hint,
                    LV_SYMBOL_UP " Deslize para desbloquear " LV_SYMBOL_UP);
  lv_obj_set_style_text_font(hint, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(hint, lv_color_hex(0x666666), 0);
  lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -20);

  // Battery
  lv_obj_t *batt = lv_label_create(scr_lock);
  lv_label_set_text_fmt(batt, "%d%%", 100);
  lv_obj_align(batt, LV_ALIGN_TOP_RIGHT, -10, 10);

  lock_timer = lv_timer_create(lock_timer_cb, 1000, nullptr);

  lv_scr_load(scr_lock);
}
