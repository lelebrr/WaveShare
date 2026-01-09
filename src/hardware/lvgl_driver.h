#pragma once

/**
 * @file lvgl_driver.h
 * @brief Header do driver LVGL para Waveshare ESP32-S3-Touch-AMOLED-1.8
 */

#include "../core/config.h"
#include "../core/pin_definitions.h"
#include "Arduino_GFX_Library.h"
#include <Arduino.h>
#include <lvgl.h>

/**
 * @brief Inicializa o driver LVGL completo
 * @param display Ponteiro para o display Arduino_GFX
 * @return true se inicializado com sucesso
 */
bool lvgl_driver_init(Arduino_GFX *display);

/**
 * @brief Processa tarefas do LVGL (chamar no loop principal)
 */
void lvgl_driver_loop();

/**
 * @brief Obtém o display LVGL principal
 * @return Ponteiro para o display
 */
lv_disp_t *lvgl_get_display();
