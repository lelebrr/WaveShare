/**
 * @file mascot_faces.cpp
 * @brief Implementação do gerenciador de faces do mascote
 */

#include "mascot_faces.h"

MascotFaces mascotFaces;

MascotFaces::MascotFaces()
    : _currentFace(FACE_HAPPY), _targetFace(FACE_HAPPY), _transitioning(false),
      _transitionStart(0), _transitionDuration(300), _blinkEnabled(true),
      _lastBlink(0), _idleStart(0), _isBlinking(false),
      _preBlinkFace(FACE_HAPPY) {}

void MascotFaces::setFace(MascotFaceType face) {
  if (face < FACE_COUNT) {
    _currentFace = face;
    _targetFace = face;
    _transitioning = false;
    _isBlinking = false;   // Interrompe blink se forçar nova face
    _idleStart = millis(); // Reset idle timer
  }
}

void MascotFaces::setFaceByName(const char *name) {
  for (int i = 0; i < FACE_COUNT; i++) {
    if (strcmp(FACE_NAMES[i], name) == 0) {
      setFace(static_cast<MascotFaceType>(i));
      return;
    }
  }
}

void MascotFaces::transitionTo(MascotFaceType face, uint16_t durationMs) {
  if (face < FACE_COUNT && face != _currentFace) {
    _targetFace = face;
    _transitioning = true;
    _transitionStart = millis();
    _transitionDuration = durationMs;
    _isBlinking = false; // Interrompe blink
  }
}

int8_t MascotFaces::getBreathingOffset() {
  // Ciclo de ~3 segundos
  // Retorna -1, 0, 1, 0, -1...
  float val = sin(millis() / 500.0) * 1.5;
  return (int8_t)val;
}

void MascotFaces::update() {
  unsigned long now = millis();

  // Processa transição
  if (_transitioning) {
    if (now - _transitionStart >= _transitionDuration) {
      _currentFace = _targetFace;
      _transitioning = false;
    }
  }

  // Blink automático
  if (_blinkEnabled && !_transitioning) {
    if (!_isBlinking) {
      // Iniciar blink aleatório
      if (now - _lastBlink > 4000 + random(6000)) { // 4-10 segundos
        _lastBlink = now;
        _preBlinkFace = _currentFace; // Salva face atual

        // Só pisca se não estiver dormindo ou ocupado
        if (_currentFace != FACE_SLEEP && _currentFace != FACE_SLEEP2 &&
            _currentFace != FACE_INTENSE && _currentFace != FACE_ATTACK) {
          _currentFace = FACE_BLINK;
          _isBlinking = true;
        }
      }
    } else {
      // Terminando blink (200ms)
      if (now - _lastBlink > 200) {
        _currentFace = _preBlinkFace; // Restaura face
        _isBlinking = false;
        _lastBlink = now; // Reseta timer do próximo
      }
    }
  }

  // Transição automática para BORED/SLEEP baseado em inatividade
  if (!_isBlinking && !_transitioning && _currentFace != FACE_SLEEP &&
      _currentFace != FACE_SLEEP2 && _currentFace != FACE_BORED) {

    unsigned long idleTime = now - _idleStart;

    if (idleTime > SLEEP_TIMEOUT) {
      transitionTo(FACE_SLEEP);
    } else if (idleTime > IDLE_TIMEOUT) {
      transitionTo(FACE_BORED);
    }
  }
}

void MascotFaces::setIdle() {
  unsigned long now = millis();
  unsigned long idleTime = now - _idleStart;

  if (idleTime > SLEEP_TIMEOUT) {
    setFace(FACE_SLEEP);
  } else if (idleTime > IDLE_TIMEOUT) {
    setFace(FACE_BORED);
  } else {
    // Face normal aleatória
    MascotFaceType normalFaces[] = {FACE_LOOK_R, FACE_LOOK_L, FACE_HAPPY};
    setFace(normalFaces[random(3)]);
  }
}

void MascotFaces::setScanning() {
  _idleStart = millis();
  setFace(FACE_INTENSE);
}

void MascotFaces::setSuccess() {
  _idleStart = millis();
  transitionTo(FACE_EXCITED, 500);
}

void MascotFaces::setError() {
  _idleStart = millis();
  transitionTo(FACE_SAD, 300);
}

void MascotFaces::setSleeping() {
  // Alterna entre SLEEP e SLEEP2
  if (_currentFace == FACE_SLEEP) {
    setFace(FACE_SLEEP2);
  } else {
    setFace(FACE_SLEEP);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// EYE-TRACKING SIMULADO - Dragão olha para direção do toque
// ═══════════════════════════════════════════════════════════════════════════

void MascotFaces::lookAt(int16_t touchX, int16_t screenWidth) {
  _idleStart = millis(); // Reset idle timer

  // Divide a tela em 3 zonas
  int16_t zone = touchX * 3 / screenWidth;

  if (zone == 0) {
    // Zona esquerda - olha para esquerda
    setFace(FACE_LOOK_L);
  } else if (zone == 2) {
    // Zona direita - olha para direita
    setFace(FACE_LOOK_R);
  } else {
    // Centro - olha para frente (feliz)
    setFace(FACE_HAPPY);
  }
}

void MascotFaces::lookAtHappy(int16_t touchX, int16_t screenWidth) {
  _idleStart = millis();

  int16_t zone = touchX * 3 / screenWidth;

  if (zone == 0) {
    setFace(FACE_LOOK_L_HAPPY);
  } else if (zone == 2) {
    setFace(FACE_LOOK_R_HAPPY);
  } else {
    setFace(FACE_GRATEFUL);
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// COR POR NÍVEL - Dragão muda de cor conforme sobe de nível
// ═══════════════════════════════════════════════════════════════════════════

uint32_t MascotFaces::getLevelColor(uint8_t level) {
  // Níveis: 0-9=Branco, 10-24=Azul, 25-49=Roxo, 50-74=Dourado, 75+=Vermelho
  // Demoníaco
  if (level < 10) {
    return 0xFFFFFF; // Branco
  } else if (level < 25) {
    return 0x00DDFF; // Azul ciano
  } else if (level < 50) {
    return 0x9933FF; // Roxo
  } else if (level < 75) {
    return 0xFFD700; // Dourado
  } else {
    return 0xFF0044; // Vermelho demoníaco
  }
}

const char *MascotFaces::getLevelTitle(uint8_t level) {
  if (level < 10) {
    return "Filhote";
  } else if (level < 25) {
    return "Aprendiz";
  } else if (level < 50) {
    return "Guerreiro";
  } else if (level < 75) {
    return "Mestre";
  } else {
    return "Lenda";
  }
}
