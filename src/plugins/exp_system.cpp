/**
 * @file exp_system.cpp
 * @brief Implementação do sistema de experiência
 */

#include "exp_system.h"
#include "../hardware/audio_driver.h"
#include "../ui/notifications_engine.h"

// Instância global
ExpSystemPlugin expPlugin;

ExpSystemPlugin::ExpSystemPlugin()
    : _totalXP(0), _level(1), _handshakeCount(0), _networkCount(0),
      _hoursOnline(0), _lastHourCheck(0), _bootTime(0) {}

bool ExpSystemPlugin::onLoad() {
  _bootTime = millis();
  _lastHourCheck = _bootTime;

  // Carrega dados salvos
  load();

  Serial.printf("[exp] XP: %u, Level: %d (%s)\n", _totalXP, _level, getTitle());

  return true;
}

void ExpSystemPlugin::onUnload() { save(); }

void ExpSystemPlugin::update() {
  // Verifica se passou 1 hora desde última verificação
  unsigned long now = millis();
  if (now - _lastHourCheck >= 3600000) { // 1 hora
    _lastHourCheck = now;
    _hoursOnline++;
    addXP(XP_HOUR_ONLINE);
    Serial.println("[exp] +50 XP por 1 hora online!");
  }
}

void ExpSystemPlugin::onHandshake(const HandshakeData &hs) {
  _handshakeCount++;

  if (hs.pmkid) {
    addXP(XP_PMKID);
    Serial.printf("[exp] +%d XP: PMKID capturado!\n", XP_PMKID);
  } else {
    addXP(XP_HANDSHAKE);
    Serial.printf("[exp] +%d XP: Handshake capturado!\n", XP_HANDSHAKE);
  }
}

void ExpSystemPlugin::onWiFiUpdate(const PwnNetwork *networks, int count) {
  static int lastCount = 0;

  // XP por novas redes descobertas
  if (count > lastCount) {
    int newNets = count - lastCount;
    _networkCount += newNets;
    addXP(XP_NETWORK_NEW * newNets);
  }
  lastCount = count;
}

void ExpSystemPlugin::addXP(uint32_t amount, const char *reason) {
  uint8_t oldLevel = _level;
  _totalXP += amount;

  if (reason) {
    Serial.printf("[exp] +%u XP: %s\n", amount, reason);
  }

  calculateLevel();
  checkLevelUp();

  if (_level != oldLevel) {
    onLevelUp(oldLevel, _level);
  }

  // Salva periodicamente (a cada 100 XP ganhos)
  static uint32_t lastSaveXP = 0;
  if (_totalXP - lastSaveXP >= 100) {
    save();
    lastSaveXP = _totalXP;
  }
}

void ExpSystemPlugin::addXP(XPAction action) {
  addXP(static_cast<uint32_t>(action));
}

const char *ExpSystemPlugin::getTitle() const {
  if (_level >= 1 && _level <= NUM_LEVELS) {
    return LEVELS[_level - 1].title;
  }
  return "Unknown";
}

uint32_t ExpSystemPlugin::getXPForNextLevel() const {
  if (_level >= NUM_LEVELS) {
    return _totalXP; // Max level
  }
  return LEVELS[_level].xp_required;
}

uint32_t ExpSystemPlugin::getXPProgress() const {
  if (_level >= NUM_LEVELS) {
    return _totalXP - LEVELS[_level - 1].xp_required;
  }
  return _totalXP - LEVELS[_level - 1].xp_required;
}

uint8_t ExpSystemPlugin::getProgressPercent() const {
  if (_level >= NUM_LEVELS)
    return 100;

  uint32_t currentLevelXP = LEVELS[_level - 1].xp_required;
  uint32_t nextLevelXP = LEVELS[_level].xp_required;
  uint32_t range = nextLevelXP - currentLevelXP;
  uint32_t progress = _totalXP - currentLevelXP;

  if (range == 0)
    return 100;
  return (progress * 100) / range;
}

void ExpSystemPlugin::calculateLevel() {
  _level = 1;
  for (int i = NUM_LEVELS - 1; i >= 0; i--) {
    if (_totalXP >= LEVELS[i].xp_required) {
      _level = LEVELS[i].level;
      break;
    }
  }
}

void ExpSystemPlugin::checkLevelUp() {
  // Já tratado em addXP
}

void ExpSystemPlugin::onLevelUp(uint8_t oldLevel, uint8_t newLevel) {
  Serial.println();
  Serial.println("╔═══════════════════════════════════════╗");
  Serial.printf("║        🎉 LEVEL UP! %d → %d            ║\n", oldLevel,
                newLevel);
  Serial.printf("║           %s                    \n", getTitle());
  Serial.println("╚═══════════════════════════════════════╝");
  Serial.println();

  // Play level-up celebratory sound
  // Custom melody: ascending notes for achievement
  audioDriver.playTone(523, 80); // C5
  delay(50);
  audioDriver.playTone(659, 80); // E5
  delay(50);
  audioDriver.playTone(784, 80); // G5
  delay(50);
  audioDriver.playTone(1047, 200); // C6 - hold

  // Show level-up notification via notifications engine
  notifications_engine.notifyCustom(NOTIFY_EFFECT_CENTER_PULSE, 0x00FF00, 1500);
}

void ExpSystemPlugin::save() {
  if (!_prefs.begin("wavepwn_exp", false)) {
    Serial.println("[exp] Erro ao abrir NVS!");
    return;
  }

  _prefs.putUInt("xp", _totalXP);
  _prefs.putUShort("hs_count", _handshakeCount);
  _prefs.putUShort("net_count", _networkCount);
  _prefs.putUInt("hours", _hoursOnline);
  _prefs.end();

  Serial.println("[exp] Dados salvos no NVS");
}

void ExpSystemPlugin::load() {
  if (!_prefs.begin("wavepwn_exp", true)) {
    Serial.println("[exp] Primeiro boot - inicializando XP");
    return;
  }

  _totalXP = _prefs.getUInt("xp", 0);
  _handshakeCount = _prefs.getUShort("hs_count", 0);
  _networkCount = _prefs.getUShort("net_count", 0);
  _hoursOnline = _prefs.getUInt("hours", 0);
  _prefs.end();

  calculateLevel();
  Serial.println("[exp] Dados carregados do NVS");
}

void ExpSystemPlugin::reset() {
  _totalXP = 0;
  _level = 1;
  _handshakeCount = 0;
  _networkCount = 0;
  _hoursOnline = 0;
  save();
  Serial.println("[exp] Dados resetados!");
}
