#pragma once

/**
 * @file ui_networks_screen.h
 * @brief Tela de lista de redes WiFi encontradas
 */

#include "../../plugins/plugin_base.h"
#include <Arduino.h>
#include <lvgl.h>

class NetworksScreen {
public:
  NetworksScreen();

  void create(lv_obj_t *parent);
  void show();
  void hide();
  void update();

  // Atualiza lista com redes
  void setNetworks(const PwnNetwork *networks, int count);

  // Callback quando rede é selecionada
  typedef void (*NetworkSelectCallback)(const PwnNetwork *network);
  void onNetworkSelect(NetworkSelectCallback cb) { _onSelect = cb; }

private:
  lv_obj_t *_screen;
  lv_obj_t *_list;
  lv_obj_t *_lblCount;
  lv_obj_t *_btnScan;
  lv_obj_t *_btnBack;

  PwnNetwork _networks[50];
  int _networkCount;
  NetworkSelectCallback _onSelect;

  static void onItemClick(lv_event_t *e);
  static void onScanClick(lv_event_t *e);
  static void onBackClick(lv_event_t *e);

  void populateList();
  const char *getSecurityIcon(uint8_t enc);
  lv_color_t getRSSIColor(int8_t rssi);
};

extern NetworksScreen networksScreen;
