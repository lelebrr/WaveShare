/**
 * @file screens.cpp
 * @brief Inicialização de todas as telas
 */

#include "screens.h"

void initAllScreens(lv_obj_t *parent) {
  menuGrid.create(parent);
  statsScreen.create(parent);
  networksScreen.create(parent);
  handshakesScreen.create(parent);

  Serial.println("[Screens] Todas as telas inicializadas");
}
