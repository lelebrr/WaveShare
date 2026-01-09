/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Tela de Seleção de Mascotes (Header)
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <lvgl.h>

// Initialize mascot selection screen
void ui_mascot_select_init();

// Create and return the screen object
lv_obj_t *ui_mascot_select_create();

// Show the mascot selection screen
void ui_mascot_select_show();
