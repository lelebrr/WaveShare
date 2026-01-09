/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Modo Minimal (UI Simplificada)
 * Interface minimalista para economia de bateria
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <lvgl.h>

class MinimalMode {
public:
  MinimalMode();

  void begin();
  void enter();
  void exit();
  void update();

  bool isActive() const { return _active; }

  // Configurações
  void setShowTime(bool show);
  void setShowStats(bool show);
  void setShowMascot(bool show);

private:
  bool _active;
  lv_obj_t *_screen;
  lv_obj_t *_timeLabel;
  lv_obj_t *_statsLabel;
  lv_obj_t *_mascotEmoji;

  bool _showTime;
  bool _showStats;
  bool _showMascot;

  uint32_t _lastUpdate;

  void createUI();
  void updateTime();
  void updateStats();
};

extern MinimalMode minimal_mode;
