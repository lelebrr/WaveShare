/**
 * ═══════════════════════════════════════════════════════════════════════════
 * WAVEPWN - Sistema de Múltiplos Mascotes
 * Implementação do gerenciador de mascotes
 * ═══════════════════════════════════════════════════════════════════════════
 */

#include "mascot_manager.h"

// ═══════════════════════════════════════════════════════════════════════════
// MASCOT DATABASE
// ═══════════════════════════════════════════════════════════════════════════

const MascotInfo MascotManager::_mascots[MASCOT_COUNT] = {
    // MASCOT_DRAGON
    {
        "dragon", "Neura", "🐉", "Dragão cyber guardião das redes",
        0x07FF, // Cyan
        0xF81F  // Magenta
    },
    // MASCOT_CAT
    {
        "cat", "Hackito", "🐱", "Gato ninja das sombras digitais",
        0xFFE0, // Yellow
        0x001F  // Blue
    },
    // MASCOT_FOX
    {
        "fox", "Vulpix", "🦊", "Raposa astuta do ciberespaço",
        0xFD20, // Orange
        0xFFFF  // White
    },
    // MASCOT_WOLF
    {
        "wolf", "Byte", "🐺", "Lobo alfa do pack digital",
        0x7BEF, // Gray
        0x001F  // Blue
    },
    // MASCOT_LIZARD
    {
        "lizard", "Pixel", "🦎", "Lagarto pixel art old school",
        0x07E0, // Green
        0xF800  // Red
    },
    // MASCOT_OCTOPUS
    {
        "octopus", "Octo", "🐙", "Polvo multitarefa das profundezas",
        0xF81F, // Magenta
        0x07FF  // Cyan
    },
    // MASCOT_ROBOT
    {
        "robot", "Robo-9", "🤖", "Robô retrô com alma hacker",
        0xC618, // Silver
        0xF800  // Red
    },
    // MASCOT_ALIEN
    {
        "alien", "Zyx", "👾", "Visitante extraterrestre 8-bit",
        0x07E0, // Green
        0xFFE0  // Yellow
    }};

// Global instance
MascotManager mascot_manager;

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════

MascotManager::MascotManager() { resetToDefaults(); }

void MascotManager::begin() {
  loadConfig();
  Serial.printf("[Mascot] Loaded: %s (%s)\n", _config.mascot_name,
                _mascots[_config.mascot_id].emoji);
}

void MascotManager::setMascot(MascotType type) {
  if (type >= MASCOT_COUNT)
    return;

  _config.mascot_id = type;
  // Set default name if current is empty
  if (_config.mascot_name[0] == '\0') {
    strncpy(_config.mascot_name, _mascots[type].name_default, 16);
    _config.mascot_name[16] = '\0';
  }

  saveConfig();
  Serial.printf("[Mascot] Changed to: %s %s\n", _mascots[type].emoji,
                _mascots[type].name_default);
}

void MascotManager::setName(const char *name) {
  if (name == nullptr)
    return;

  strncpy(_config.mascot_name, name, 16);
  _config.mascot_name[16] = '\0';

  saveConfig();
  Serial.printf("[Mascot] Renamed to: %s\n", _config.mascot_name);
}

void MascotManager::setAnimationSpeed(uint8_t speed) {
  _config.animation_speed = constrain(speed, 1, 10);
  saveConfig();
}

void MascotManager::setSpeechBubbles(bool enabled) {
  _config.show_speech_bubbles = enabled;
  saveConfig();
}

void MascotManager::setMood(uint8_t mood) {
  _config.mood = mood % 4;
  saveConfig();
}

const MascotInfo &MascotManager::getMascotInfo(MascotType type) const {
  if (type >= MASCOT_COUNT)
    type = MASCOT_DRAGON;
  return _mascots[type];
}

const MascotInfo &MascotManager::getCurrentMascotInfo() const {
  return _mascots[_config.mascot_id];
}

void MascotManager::saveConfig() {
  _prefs.begin("mascot", false);
  _prefs.putBytes("config", &_config, sizeof(MascotConfig));
  _prefs.end();
}

void MascotManager::loadConfig() {
  _prefs.begin("mascot", true);
  size_t len = _prefs.getBytes("config", &_config, sizeof(MascotConfig));
  _prefs.end();

  // Validate
  if (len != sizeof(MascotConfig) || _config.mascot_id >= MASCOT_COUNT) {
    resetToDefaults();
  }
}

void MascotManager::resetToDefaults() {
  _config.mascot_id = MASCOT_DRAGON;
  strncpy(_config.mascot_name, _mascots[MASCOT_DRAGON].name_default, 16);
  _config.mascot_name[16] = '\0';
  _config.animation_speed = 5;
  _config.show_speech_bubbles = true;
  _config.mood = 0; // happy
}
