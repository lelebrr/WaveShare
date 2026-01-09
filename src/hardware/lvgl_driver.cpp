/**
 * @file lvgl_driver.cpp
 * @brief Driver LVGL completo para Waveshare ESP32-S3-Touch-AMOLED-1.8
 *
 * Implementa:
 * - Display buffer e flush callback
 * - Touch input driver
 * - Timer tick para animações
 */

#include "lvgl_driver.h"
#include "../core/globals.h"
#include "system_hardware.h"

// Buffer de display LVGL (em PSRAM)
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf1 = nullptr;
static lv_color_t *buf2 = nullptr;

// Display e input drivers
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;

// Ponteiro para Arduino_GFX
static Arduino_GFX *gfx = nullptr;

/**
 * @brief Callback de flush do display para LVGL
 */
static void lvgl_display_flush(lv_disp_drv_t *drv, const lv_area_t *area,
                               lv_color_t *color_p) {
  if (!gfx) {
    lv_disp_flush_ready(drv);
    return;
  }

  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)color_p, w, h);

  lv_disp_flush_ready(drv);
}

/**
 * @brief Callback de leitura do touch para LVGL
 */
static void lvgl_touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  SystemHardware::TouchPoint tp = sys_hw.getTouch();

  if (tp.touched) {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = tp.x;
    data->point.y = tp.y;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

/**
 * @brief Tick do LVGL é automático via LV_TICK_CUSTOM no lv_conf.h
 * O LVGL usa millis() diretamente através de LV_TICK_CUSTOM_SYS_TIME_EXPR
 */

/**
 * @brief Inicializa o driver LVGL completo
 * @param display Ponteiro para o display Arduino_GFX
 * @return true se inicializado com sucesso
 */
bool lvgl_driver_init(Arduino_GFX *display) {
  if (!display) {
    Serial.println("[LVGL] Display nulo!");
    return false;
  }

  gfx = display;

  // Inicializa LVGL
  lv_init();
  Serial.println("[LVGL] Core inicializado");

  // Tip 13/42: Upgrade to Full Screen Double Buffer for max smoothness (PSRAM)
  size_t buf_size = LCD_WIDTH * LCD_HEIGHT;

  buf1 = (lv_color_t *)heap_caps_malloc(buf_size * sizeof(lv_color_t),
                                        MALLOC_CAP_SPIRAM);
  if (!buf1) {
    // Fallback para DRAM
    buf1 = (lv_color_t *)malloc(buf_size * sizeof(lv_color_t));
  }

  buf2 = (lv_color_t *)heap_caps_malloc(buf_size * sizeof(lv_color_t),
                                        MALLOC_CAP_SPIRAM);
  if (!buf2) {
    // Fallback para DRAM
    buf2 = (lv_color_t *)malloc(buf_size * sizeof(lv_color_t));
  }

  if (!buf1) {
    Serial.println("[LVGL] Falha ao alocar buffer!");
    return false;
  }

  // Inicializa draw buffer (double buffer se possível)
  if (buf2) {
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, buf_size);
    Serial.println("[LVGL] Double buffer ativado");
  } else {
    lv_disp_draw_buf_init(&draw_buf, buf1, nullptr, buf_size);
    Serial.println("[LVGL] Single buffer");
  }

  // Configura display driver
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = lvgl_display_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);
  Serial.printf("[LVGL] Display registrado: %dx%d\n", LCD_WIDTH, LCD_HEIGHT);

  // Configura input driver (touch)
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = lvgl_touchpad_read;
  lv_indev_drv_register(&indev_drv);
  Serial.println("[LVGL] Touch registrado");

  // Tick é automático via LV_TICK_CUSTOM no lv_conf.h
  Serial.println("[LVGL] Tick automático (millis)");

  Serial.println("[LVGL] Inicialização completa!");
  return true;
}

/**
 * @brief Processa tarefas do LVGL (chamar no loop)
 */
void lvgl_driver_loop() {
  lv_timer_handler(); // Processa tarefas LVGL
}

/**
 * @brief Obtém o display LVGL principal
 * @return Ponteiro para o display
 */
lv_disp_t *lvgl_get_display() { return lv_disp_get_default(); }
