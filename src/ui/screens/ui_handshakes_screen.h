#pragma once

/**
 * @file ui_handshakes_screen.h
 * @brief Tela de handshakes capturados
 */

#include "../../plugins/plugin_base.h"
#include <Arduino.h>
#include <lvgl.h>


class HandshakesScreen {
public:
  HandshakesScreen();

  void create(lv_obj_t *parent);
  void show();
  void hide();
  void update();

  void addHandshake(const HandshakeData &hs);
  void clearAll();

private:
  lv_obj_t *_screen;
  lv_obj_t *_list;
  lv_obj_t *_lblCount;
  lv_obj_t *_btnExport;
  lv_obj_t *_btnClear;
  lv_obj_t *_btnBack;

  HandshakeData _handshakes[100];
  int _count;

  static void onExportClick(lv_event_t *e);
  static void onClearClick(lv_event_t *e);
  static void onBackClick(lv_event_t *e);

  void populateList();
};

extern HandshakesScreen handshakesScreen;
