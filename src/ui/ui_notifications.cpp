#include "ui_notifications.h"
#include "../hardware/audio_driver.h"
#include "ui_helpers.h"
#include "ui_themes.h"
#include <string>
#include <vector>

struct NotifItem {
  String title;
  String msg;
  NotificationType type;
  unsigned long time;
};
static std::vector<NotifItem> history;
static lv_obj_t *toast_obj = nullptr;
static lv_timer_t *toast_timer = nullptr;

static void toast_anim_cb(void *var, int32_t v) {
  lv_obj_set_y((lv_obj_t *)var, v);
}

static void toast_close_timer_cb(lv_timer_t *t) {
  if (toast_obj) {
    // Slide out
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, toast_obj);
    lv_anim_set_values(&a, 10, -100);
    lv_anim_set_time(&a, 500);
    lv_anim_set_exec_cb(&a, toast_anim_cb);
    lv_anim_set_deleted_cb(&a, [](lv_anim_t *a) {
      lv_obj_del((lv_obj_t *)a->var);
      toast_obj = nullptr;
    });
    lv_anim_start(&a);
  }
  lv_timer_del(t);
  toast_timer = nullptr;
}

void ui_notification_push(const char *title, const char *msg,
                          NotificationType type) {
  // Add to history
  if (history.size() > 20)
    history.erase(history.begin());
  history.push_back({String(title), String(msg), type, millis()});

  // Play Sound (Tip 33)
  switch (type) {
  case NOTIFY_SUCCESS:
    audioDriver.playSound(SOUND_SUCCESS);
    break;
  case NOTIFY_ERROR:
    audioDriver.playSound(SOUND_ERROR);
    break;
  case NOTIFY_ATTACK:
    audioDriver.playSound(SOUND_ALERT);
    break;
  default:
    audioDriver.playSound(SOUND_CLICK);
    break;
  }

  // Show Toast (Tip 32)
  if (toast_obj)
    lv_obj_del(toast_obj);
  if (toast_timer)
    lv_timer_del(toast_timer);

  toast_obj = lv_obj_create(lv_layer_top());
  lv_obj_set_size(toast_obj, LCD_WIDTH - 20, 70);
  lv_obj_align(toast_obj, LV_ALIGN_TOP_MID, 0, -100); // Start hidden above

  lv_color_t color;
  const char *icon;
  switch (type) {
  case NOTIFY_INFO:
    color = COLOR_NEON_BLUE;
    icon = LV_SYMBOL_BELL;
    break;
  case NOTIFY_SUCCESS:
    color = COLOR_NEON_GREEN;
    icon = LV_SYMBOL_OK;
    break;
  case NOTIFY_WARNING:
    color = getTheme().warning;
    icon = LV_SYMBOL_WARNING;
    break;
  case NOTIFY_ERROR:
    color = COLOR_NEON_RED;
    icon = LV_SYMBOL_CLOSE;
    break;
  case NOTIFY_ATTACK:
    color = COLOR_NEON_RED;
    icon = "\xEF\x97\x9B";
    break; // Skull or similar
  }

  lv_obj_set_style_bg_color(toast_obj, lv_color_hex(0x222222), 0);
  lv_obj_set_style_border_color(toast_obj, color, 0);
  lv_obj_set_style_border_width(toast_obj, 2, 0);
  lv_obj_set_style_radius(toast_obj, 12, 0);
  lv_obj_set_style_shadow_width(toast_obj, 20, 0);
  lv_obj_set_style_shadow_color(toast_obj, color, 0);
  lv_obj_set_style_pad_all(toast_obj, 10, 0);

  lv_obj_t *lbl_icon = lv_label_create(toast_obj);
  lv_label_set_text(lbl_icon, icon);
  lv_obj_set_style_text_color(lbl_icon, color, 0);
  lv_obj_set_style_text_font(lbl_icon, &lv_font_montserrat_24, 0);
  lv_obj_align(lbl_icon, LV_ALIGN_LEFT_MID, 0, 0);

  lv_obj_t *lbl_title = lv_label_create(toast_obj);
  lv_label_set_text(lbl_title, title);
  lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(lbl_title, lv_color_white(), 0);
  lv_obj_align(lbl_title, LV_ALIGN_TOP_LEFT, 40, 0);

  lv_obj_t *lbl_msg = lv_label_create(toast_obj);
  lv_label_set_text(lbl_msg, msg);
  lv_obj_set_style_text_font(lbl_msg, &lv_font_montserrat_12, 0);
  lv_obj_set_style_text_color(lbl_msg, lv_color_hex(0xcccccc), 0);
  lv_obj_align(lbl_msg, LV_ALIGN_BOTTOM_LEFT, 40, 0);

  // Slide In
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, toast_obj);
  lv_anim_set_values(&a, -100, 10);
  lv_anim_set_time(&a, 300);
  lv_anim_set_exec_cb(&a, toast_anim_cb);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
  lv_anim_start(&a);

  // Auto close
  toast_timer = lv_timer_create(toast_close_timer_cb, 3000, nullptr);
}

void ui_notification_center_show() {
  // Overlay for History (Tip 31)
  lv_obj_t *overlay = lv_obj_create(lv_layer_top());
  lv_obj_set_size(overlay, LCD_WIDTH, LCD_HEIGHT);
  lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
  lv_obj_set_style_bg_opa(overlay, 200, 0);
  lv_obj_add_event_cb(
      overlay,
      [](lv_event_t *e) { lv_obj_del((lv_obj_t *)lv_event_get_target(e)); },
      LV_EVENT_CLICKED, nullptr);

  lv_obj_t *panel = lv_obj_create(overlay);
  lv_obj_set_size(panel, 300, 400);
  lv_obj_center(panel);
  ui_apply_glass_effect(panel);

  lv_obj_t *title = lv_label_create(panel);
  lv_label_set_text(title, "NOTIFICATIONS");
  lv_obj_set_style_text_font(title, FONT_TITLE, 0);
  lv_obj_set_style_text_color(title, COLOR_NEON_BLUE, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 0);

  lv_obj_t *list = lv_obj_create(panel);
  lv_obj_set_size(list, lv_pct(100), 340);
  lv_obj_align(list, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(list, 0, 0);
  lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);

  // Iterate reverse to show newest first
  if (history.empty()) {
    lv_obj_t *emptyLabel = lv_label_create(list);
    lv_label_set_text(emptyLabel, "Sem notificações");
    lv_obj_set_style_text_font(emptyLabel, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(emptyLabel, lv_color_hex(0x666666), 0);
    lv_obj_center(emptyLabel);
  } else {
    for (int i = history.size() - 1; i >= 0; i--) {
      NotifItem &item = history[i];

      lv_obj_t *card = lv_obj_create(list);
      lv_obj_set_width(card, lv_pct(100));
      lv_obj_set_height(card, LV_SIZE_CONTENT);
      lv_obj_set_style_bg_color(card, lv_color_hex(0x202020), 0);
      lv_obj_set_style_pad_all(card, 10, 0);

      lv_obj_t *t = lv_label_create(card);
      lv_label_set_text(t, item.title.c_str());
      lv_obj_set_style_text_font(t, &lv_font_montserrat_14, 0);
      lv_obj_set_style_text_color(t, lv_color_white(), 0);

      lv_obj_t *m = lv_label_create(card);
      lv_label_set_text(m, item.msg.c_str());
      lv_obj_set_style_text_font(m, &lv_font_montserrat_12, 0);
      lv_obj_set_style_text_color(m, lv_color_hex(0xaaaaaa), 0);
      lv_obj_align(m, LV_ALIGN_TOP_LEFT, 0, 20);
    }
  }
}

void ui_notification_init() {
  // Pre-alloc/Reserve if needed
  history.reserve(20);
}
