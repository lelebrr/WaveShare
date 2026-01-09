#pragma once

/**
 * @file exp_system.h
 * @brief Sistema de experiência e níveis (inspirado no Pwnagotchi exp.py)
 *
 * Pontuação:
 * - Handshake capturado: +100 XP
 * - Rede nova descoberta: +10 XP
 * - Deauth bem-sucedido: +5 XP
 * - 1 hora online: +50 XP
 * - Senha cracked: +500 XP
 */

#include "plugin_base.h"
#include <Preferences.h>

// Níveis e títulos
struct LevelInfo {
  uint8_t level;
  uint32_t xp_required;
  const char *title;
};

static const LevelInfo LEVELS[] = {
    {1, 0, "Hatchling"},   {2, 100, "Wyrmling"},    {3, 300, "Drake"},
    {4, 600, "Wyvern"},    {5, 1000, "Dragon"},     {6, 2000, "Elder Dragon"},
    {7, 5000, "Ancient"},  {8, 10000, "Legendary"}, {9, 25000, "Mythic"},
    {10, 50000, "Godlike"}};

static const int NUM_LEVELS = sizeof(LEVELS) / sizeof(LEVELS[0]);

// Pontos por ação
enum XPAction {
  XP_HANDSHAKE = 100,
  XP_NETWORK_NEW = 10,
  XP_DEAUTH = 5,
  XP_BEACON_FLOOD = 2,
  XP_BLE_DEVICE = 5,
  XP_HOUR_ONLINE = 50,
  XP_PASSWORD_CRACKED = 500,
  XP_PMKID = 150,
  XP_EVIL_TWIN = 20
};

class ExpSystemPlugin : public PluginBase {
public:
  ExpSystemPlugin();

  // PluginBase interface
  const char *getName() const override { return "exp"; }
  const char *getAuthor() const override { return "WavePwn"; }
  const char *getVersion() const override { return "1.0.0"; }

  bool onLoad() override;
  void onUnload() override;
  void update() override;

  // Eventos que dão XP
  void onHandshake(const HandshakeData &hs) override;
  void onWiFiUpdate(const PwnNetwork *networks, int count) override;

  // Métodos públicos
  void addXP(uint32_t amount, const char *reason = nullptr);
  void addXP(XPAction action);

  // Getters
  uint32_t getTotalXP() const { return _totalXP; }
  uint8_t getLevel() const { return _level; }
  const char *getTitle() const;
  uint32_t getXPForNextLevel() const;
  uint32_t getXPProgress() const;
  uint8_t getProgressPercent() const;
  uint16_t getHandshakeCount() const { return _handshakeCount; }
  uint16_t getNetworkCount() const { return _networkCount; }

  // Persistência
  void save();
  void load();
  void reset();

private:
  uint32_t _totalXP;
  uint8_t _level;
  uint16_t _handshakeCount;
  uint16_t _networkCount;
  uint32_t _hoursOnline;
  unsigned long _lastHourCheck;
  unsigned long _bootTime;

  Preferences _prefs;

  void calculateLevel();
  void checkLevelUp();
  void onLevelUp(uint8_t oldLevel, uint8_t newLevel);
};

// Instância global
extern ExpSystemPlugin expPlugin;

// Helper function for external access to dragon level
inline uint8_t getDragonLevel() { return expPlugin.getLevel(); }
