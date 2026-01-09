#pragma once

/**
 * @file signal_aura.h
 * @brief Aura visual ao redor do mascote indicando força do sinal Wi-Fi
 */

#include <lvgl.h>

/**
 * @brief Classe para exibir aura de sinal ao redor do mascote
 */
class SignalAura {
public:
  SignalAura();

  /**
   * @brief Inicializa a aura em um container pai
   */
  void init(lv_obj_t *parent);

  /**
   * @brief Atualiza a aura (chamar no loop)
   */
  void update();

  /**
   * @brief Define intensidade do sinal (0-100)
   */
  void setSignalStrength(uint8_t strength);

  /**
   * @brief Define RSSI diretamente (-100 a 0 dBm)
   */
  void setRSSI(int8_t rssi);

  /**
   * @brief Mostra/esconde a aura
   */
  void show();
  void hide();
  bool isVisible() const { return _visible; }

  /**
   * @brief Define cor base da aura
   */
  void setColor(uint32_t color);

  /**
   * @brief Ativa efeito pulsante
   */
  void setPulsing(bool enable);

private:
  lv_obj_t *_container;
  lv_obj_t *_auraRings[3]; // 3 anéis concêntricos
  uint8_t _strength;
  uint32_t _color;
  bool _visible;
  bool _pulsing;
  int _frameCount;

  void updateRings();
  uint32_t getColorForStrength(uint8_t strength);
};

extern SignalAura signalAura;
