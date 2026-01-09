#include "ui_debug_screen.h"
#include "../core/config.h"
#include "../hardware/system_hardware.h"
#include <Arduino.h>


static lv_obj_t *debug_scr = nullptr;
static lv_timer_t *debug_timer = nullptr;
static lv_obj_t *label_info = nullptr;

static void debug_timer_cb(lv_timer_t *timer) {
  if (!label_info)
    return;

  uint32_t free_heap = ESP.getFreeHeap();
  uint32_t free_psram = ESP.getFreePsram();
  float temp = temperatureRead();
  uint32_t uptime = millis() / 1000;

  // Power Info
  float vbat = sys_hw.getBatteryVoltage();
  int bat_pct = sys_hw.getBatteryPercent();

  String info = "=== DEBUG MODE ===\n";
  info += "CPU: " + String(ESP.getCpuFreqMHz()) + " MHz\n";
  info += "Heap: " + String(free_heap / 1024) + " KB\n";
  info += "PSRAM: " + String(free_psram / 1024) + " KB\n";
  info += "Temp: " + String(temp, 1) + " C\n";
  info += "VBat: " + String(vbat, 2) + "V (" + String(bat_pct) + "%)\n";
  info += "Uptime: " + String(uptime) + " s\n";
  info += "Core: " + String(ARDUINO_RUNNING_CORE) + "\n";
  info += "SDK: " + String(ESP.getSdkVersion());

  lv_label_set_text(label_info, info.c_str());
}

static void close_event_cb(lv_event_t *e) { ui_debug_screen_hide(); }

void ui_debug_screen_show(void) {
  if (debug_scr)
    return;

  debug_scr = lv_obj_create(lv_scr_act());
  lv_obj_set_size(debug_scr, 200, 260);
  lv_obj_center(debug_scr);
  lv_obj_set_style_bg_color(debug_scr, lv_color_hex(0x000000), 0);
  lv_obj_set_style_border_color(debug_scr, lv_color_hex(0xFF0000), 0);
  lv_obj_set_style_border_width(debug_scr, 2, 0);

  label_info = lv_label_create(debug_scr);
  lv_obj_set_width(label_info, 180);
  lv_label_set_long_mode(label_info, LV_LABEL_LONG_WRAP);
  lv_obj_align(label_info, LV_ALIGN_TOP_LEFT, 5, 5);
  lv_obj_set_style_text_color(label_info, lv_color_hex(0x00FF00), 0);
  lv_obj_set_style_text_font(label_info, &lv_font_montserrat_10,
                             0); // Tiny font

  // Close button
  lv_obj_t *btn_close = lv_btn_create(debug_scr);
  lv_obj_set_size(btn_close, 60, 30);
  lv_obj_align(btn_close, LV_ALIGN_BOTTOM_MID, 0, -5);
  lv_obj_add_event_cb(btn_close, close_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *lbl_close = lv_label_create(btn_close);
  lv_label_set_text(lbl_close, "CLOSE");
  lv_obj_center(lbl_close);

  // Update every 500ms
  debug_timer = lv_timer_create(debug_timer_cb, 500, NULL);
  debug_timer_cb(debug_timer); // First update
}

void ui_debug_screen_hide(void) {
  if (debug_timer) {
    lv_timer_del(debug_timer);
    debug_timer = nullptr;
  }
  if (debug_scr) {
    lv_obj_del(debug_scr);
    debug_scr = nullptr;
    label_info = nullptr;
  }
}
