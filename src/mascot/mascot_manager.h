/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Sistema de Múltiplos Mascotes (ETAPA 4)
 * Gerenciador de mascotes customizáveis
 * ═══════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <Arduino.h>
#include <Preferences.h>

// ═══════════════════════════════════════════════════════════════════════════
// MASCOT DEFINITIONS
// ═══════════════════════════════════════════════════════════════════════════

enum MascotType : uint8_t {
  MASCOT_DRAGON = 0, // 🐉 Dragão (padrão)
  MASCOT_CAT,        // 🐱 Gato Hacker
  MASCOT_FOX,        // 🦊 Raposa Cyber
  MASCOT_WOLF,       // 🐺 Lobo Digital
  MASCOT_LIZARD,     // 🦎 Lagarto Pixel
  MASCOT_OCTOPUS,    // 🐙 Polvo Tentáculos
  MASCOT_ROBOT,      // 🤖 Robô Retro
  MASCOT_ALIEN,      // 👾 Alien 8-bit
  MASCOT_COUNT
};

// Mascot info structure
struct MascotInfo {
  const char *id;
  const char *name_default;
  const char *emoji;
  const char *description;
  uint16_t primary_color;
  uint16_t secondary_color;
};

// Mascot configuration (saved to NVS)
struct MascotConfig {
  uint8_t mascot_id;        // 0-7 (índice do mascote)
  char mascot_name[17];     // Nome personalizado (16 + null)
  uint8_t animation_speed;  // 1-10 velocidade das animações
  bool show_speech_bubbles; // Mostrar balões de fala
  uint8_t mood;             // 0=happy, 1=angry, 2=sleepy, 3=excited
};

// ═══════════════════════════════════════════════════════════════════════════
// MASCOT MANAGER CLASS
// ═══════════════════════════════════════════════════════════════════════════

class MascotManager {
public:
  MascotManager();

  // Initialization
  void begin();

  // Mascot selection
  void setMascot(MascotType type);
  MascotType getMascot() const { return (MascotType)_config.mascot_id; }

  // Name management
  void setName(const char *name);
  const char *getName() const { return _config.mascot_name; }

  // Animation settings
  void setAnimationSpeed(uint8_t speed);
  uint8_t getAnimationSpeed() const { return _config.animation_speed; }

  // Speech bubbles
  void setSpeechBubbles(bool enabled);
  bool getSpeechBubbles() const { return _config.show_speech_bubbles; }

  // Mood
  void setMood(uint8_t mood);
  uint8_t getMood() const { return _config.mood; }

  // Talking State (Tip 19)
  void setTalking(bool isTalking) { _talking = isTalking; }
  bool isTalking() const { return _talking; }

  // Get info
  const MascotInfo &getMascotInfo(MascotType type) const;
  const MascotInfo &getCurrentMascotInfo() const;

  // Configuration
  MascotConfig &getConfig() { return _config; }
  void saveConfig();
  void loadConfig();
  void resetToDefaults();

  // Get all mascots list
  static const MascotInfo *getAllMascots() { return _mascots; }
  static uint8_t getMascotCount() { return MASCOT_COUNT; }

private:
  MascotConfig _config;
  Preferences _prefs;
  bool _talking = false; // Tip 19

  static const MascotInfo _mascots[MASCOT_COUNT];
};

extern MascotManager mascot_manager;
