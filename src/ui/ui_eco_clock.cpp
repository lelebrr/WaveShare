#include "ui_eco_clock.h"
#include "ui_helpers.h"
#include "ui_home.h"
#include "../core/globals.h"
#include "../hardware/system_hardware.h"
#include "../hardware/wifi_driver.h"
#include "../hardware/ble_driver.h"
#include "../wifi/wifi_attacks.h"
#include <Arduino.h>

static lv_obj_t *_screen = nullptr;
static lv_obj_t *_timeLabel = nullptr;
static lv_obj_t *_dateLabel = nullptr;
static lv_obj_t *_holdBar = nullptr;
static lv_timer_t *_updateTimer = nullptr;
static lv_timer_t *_holdTimer = nullptr;
static uint32_t _holdStart = 0;
static bool _isHolding = false;

// Updates clock text
static void update_clock(lv_timer_t *t) {
    if (!_timeLabel) return;
    String timeStr = sys_hw.getTimeString(); 
    // Format: "YYYY-MM-DD HH:MM:SS" -> We want "HH:MM"
    // Extract substring indices 11 to 16 for HH:MM
    if (timeStr.length() >= 16) {
        String clockText = timeStr.substring(11, 16);
        lv_label_set_text(_timeLabel, clockText.c_str());
        
        String dateText = timeStr.substring(0, 10);
        lv_label_set_text(_dateLabel, dateText.c_str()); // Optional Date
    }
}

static void exit_eco_mode() {
    // 1. Restore Hardware
    sys_hw.enableBatterySaver(false); // Restore 240MHz
    sys_hw.setDisplayBrightness(g_state.screen_brightness); // Restore brightness
    
    // 2. UI Switch
    ui_eco_clock_hide();
    ui_home_show();
}

static void hold_timer_cb(lv_timer_t *t) {
    if (!_isHolding) {
        lv_bar_set_value(_holdBar, 0, LV_ANIM_OFF);
        return;
    }
    
    uint32_t elapsed = millis() - _holdStart;
    int pct = (elapsed * 100) / 10000; // 10000ms = 10s
    
    if (pct >= 100) {
        // TRIGGER EXIT
        _isHolding = false;
        lv_timer_pause(_holdTimer);
        exit_eco_mode();
    } else {
        lv_bar_set_value(_holdBar, pct, LV_ANIM_ON);
    }
}

static void touch_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_PRESSED) {
        _isHolding = true;
        _holdStart = millis();
        lv_obj_clear_flag(_holdBar, LV_OBJ_FLAG_HIDDEN);
        if (_holdTimer) lv_timer_resume(_holdTimer);
    } 
    else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        _isHolding = false;
        lv_obj_add_flag(_holdBar, LV_OBJ_FLAG_HIDDEN);
        lv_bar_set_value(_holdBar, 0, LV_ANIM_OFF);
        if (_holdTimer) lv_timer_pause(_holdTimer);
    }
}

void ui_eco_clock_show() {
    if (_screen) return;
    
    // 1. Create Minimal UI
    _screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(_screen, lv_color_black(), 0);
    lv_obj_clear_flag(_screen, LV_OBJ_FLAG_SCROLLABLE);
    
    // Touch Handler (Whole Screen)
    lv_obj_add_event_cb(_screen, touch_event_cb, LV_EVENT_ALL, NULL);

    _timeLabel = lv_label_create(_screen);
    lv_obj_set_style_text_font(_timeLabel, &lv_font_montserrat_40, 0); // Large clock (might need 32 if 40 not enabled)
    lv_obj_set_style_text_color(_timeLabel, lv_color_white(), 0);
    lv_obj_align(_timeLabel, LV_ALIGN_CENTER, 0, -20);
    
    _dateLabel = lv_label_create(_screen);
    lv_obj_set_style_text_font(_dateLabel, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(_dateLabel, lv_color_hex(0x666666), 0);
    lv_obj_align(_dateLabel, LV_ALIGN_CENTER, 0, 30);

    _holdBar = lv_bar_create(_screen);
    lv_obj_set_size(_holdBar, 200, 10);
    lv_obj_align(_holdBar, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_color(_holdBar, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_color(_holdBar, lv_color_hex(0x00ff88), LV_PART_INDICATOR);
    lv_obj_add_flag(_holdBar, LV_OBJ_FLAG_HIDDEN);
    
    // Instruction visual
    lv_obj_t* hint = lv_label_create(_screen);
    lv_label_set_text(hint, "Segure 10s para Sair");
    lv_obj_set_style_text_color(hint, lv_color_hex(0x444444), 0);
    lv_obj_align(hint, LV_ALIGN_BOTTOM_MID, 0, -35);

    // 2. Hardware Power Down
    wifi_attacks.stopAttack();
    ble_driver.stopSpam();
    wifi_driver.stopAP(); // Ensure AP is off
    
    // 3. Timers
    update_clock(NULL);
    _updateTimer = lv_timer_create(update_clock, 10000, NULL); // Update every 10s
    _holdTimer = lv_timer_create(hold_timer_cb, 100, NULL);
    lv_timer_pause(_holdTimer);

    // 4. Activate Low Power
    sys_hw.enableBatterySaver(true); // 80MHz
    sys_hw.setDisplayBrightness(10); // Dim
    
    lv_scr_load(_screen);
}

void ui_eco_clock_hide() {
    if (_screen) {
        lv_obj_del(_screen);
        _screen = nullptr;
        _timeLabel = nullptr;
        if (_updateTimer) lv_timer_del(_updateTimer);
        if (_holdTimer) lv_timer_del(_holdTimer);
        _updateTimer = nullptr;
        _holdTimer = nullptr;
    }
}
