/**
 * @file lv_conf.h
 * Configuration file for v8.3.11
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 0

/*=========================
   MEMORY SETTINGS
 *=========================*/

#define LV_MEM_CUSTOM 1
#if LV_MEM_CUSTOM
#define LV_MEM_CUSTOM_INCLUDE "esp_heap_caps.h"
#define LV_MEM_CUSTOM_ALLOC(size) heap_caps_malloc(size, MALLOC_CAP_SPIRAM)
#define LV_MEM_CUSTOM_FREE(ptr) heap_caps_free(ptr)
#define LV_MEM_CUSTOM_REALLOC(ptr, new_size)                                   \
  heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM)
#endif
// #define LV_MEM_SIZE (128 * 1024U) /* Unused when CUSTOM=1 */
#define LV_MEM_ADR 0

/*====================
   HAL SETTINGS
 *====================*/

#define LV_DISP_DEF_REFR_PERIOD 16  /*[ms]*/
#define LV_INDEV_DEF_READ_PERIOD 16 /*[ms]*/

#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

/*====================
   FEATURE CONFIGURATION
 *====================*/

#define LV_DRAW_COMPLEX 1
#define LV_SHADOW_CACHE_SIZE 1

#define LV_ENABLE_GC 0

/*====================
   FONT SETTINGS
 *====================*/
// Tip 16: Disable anti-aliasing for small text/fonts
#define LV_FONT_ANTIALIAS 0
#define LV_ANTIALIAS 0 /* General anti-aliasing off */

/*==================
 *   FONT USAGE
 *================*/

// Enabled widely to prevent errors with imported UI libs
#define LV_FONT_MONTSERRAT_8 0
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 1

typedef void *lv_font_user_data_t;

/*=====================
 *  COMPILER SETTINGS
 *====================*/

#define LV_BIG_ENDIAN_SYSTEM 0
#define LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TASK_HANDLER
#define LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1
#define LV_ATTRIBUTE_LARGE_CONST
#define LV_GLOBAL_PREFIX

/*==================
 *   MENTORING
 *================*/

#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR 0

#endif /*LV_CONF_H*/
