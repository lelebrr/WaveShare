#pragma once

/**
 * @file mascot_faces.h
 * @brief 25 expressões faciais do mascote (inspirado no Pwnagotchi)
 */

#include <Arduino.h>

/**
 * @brief Enum de todas as faces disponíveis
 */
enum MascotFaceType {
  FACE_LOOK_R = 0,   // Olhando direita
  FACE_LOOK_L,       // Olhando esquerda
  FACE_LOOK_R_HAPPY, // Direita feliz
  FACE_LOOK_L_HAPPY, // Esquerda feliz
  FACE_SLEEP,        // Dormindo
  FACE_SLEEP2,       // Dormindo profundo
  FACE_AWAKE,        // Acordando
  FACE_BORED,        // Entediado
  FACE_INTENSE,      // Intenso
  FACE_COOL,         // Legal
  FACE_HAPPY,        // Feliz
  FACE_GRATEFUL,     // Grato
  FACE_EXCITED,      // Empolgado
  FACE_MOTIVATED,    // Motivado
  FACE_DEMOTIVATED,  // Desmotivado
  FACE_LONELY,       // Solitário
  FACE_SAD,          // Triste
  FACE_ANGRY,        // Raivoso
  FACE_FRIEND,       // Amigo detectado
  FACE_BROKEN,       // Quebrado
  FACE_DEBUG,        // Debug mode
  FACE_UPLOAD,       // Enviando
  FACE_UPLOAD1,      // Enviando 1
  FACE_UPLOAD2,      // Enviando 2

  // === NOVO: 20+ EXPRESSÕES EXTRAS ===
  FACE_SMART,      // Óculos de intelectual
  FACE_MASK,       // Máscara de gás/hacker
  FACE_KING,       // Coroa Wi-Fi
  FACE_ATTACK,     // Cospindo fogo
  FACE_VICTORY,    // Mãozinha de vitória
  FACE_SKEPTICAL,  // Olhar desconfiado
  FACE_LOVE,       // Apaixonado
  FACE_SHOCK,      // Choque
  FACE_DEAD,       // Morto/K.O.
  FACE_MUSIC,      // Ouvindo música
  FACE_GAMER,      // Fone de ouvido
  FACE_SICK,       // Doente
  FACE_RICH,       // Dinheiro
  FACE_EVIL,       // Sorriso maligno
  FACE_CONFUSED,   // Confuso
  FACE_BLINK,      // Piscando (Olhos fechados)
  FACE_SUGGESTIVE, // Carinha aquela
  FACE_WINK,       // Piscadinha
  FACE_CRYING,     // Chorando muito
  FACE_GLITCH,     // Bugado/Glitch
  FACE_MATRIX,     // Matrix code eyes

  FACE_ICON, // Ícone
  FACE_COUNT // Total de faces
};

/**
 * @brief Representação ASCII de cada face
 */
static const char *FACE_ASCII[] = {
    "( ⚆_⚆)",   // LOOK_R
    "(⚆_⚆ )",   // LOOK_L
    "( ◠‿◠)",   // LOOK_R_HAPPY
    "(◠‿◠ )",   // LOOK_L_HAPPY
    "(-.-)zzZ", // SLEEP
    "(-.-)...", // SLEEP2
    "(⊙_⊙)!",   // AWAKE
    "(-_-)",    // BORED
    "(°▃°)",    // INTENSE
    "(⌐■_■)",   // COOL
    "(•‿•)",    // HAPPY
    "(^‿^)",    // GRATEFUL
    "(ᗒᗨᗕ)",    // EXCITED
    "(ง •̀_•́)ง", // MOTIVATED
    "(╯︵╰,)",  // DEMOTIVATED
    "(ب_ب)",    // LONELY
    "(◕︵◕)",   // SAD
    "(╬ಠ益ಠ)",  // ANGRY
    "(♥‿♥)",    // FRIEND
    "(✖_✖)",    // BROKEN
    "[DEBUG]",  // DEBUG
    "(↑_↑)",    // UPLOAD
    "(↑ ↑)",    // UPLOAD1
    "(↟ ↟)",    // UPLOAD2

    // NOVOS
    "(⚯_⚯)",     // SMART
    "(😷_😷)",   // MASK
    "♔(•‿•)♔",   // KING
    "🔥(ÒДÓ)🔥", // ATTACK
    "✌(◕‿-)✌",   // VICTORY
    "(¬_¬)",     // SKEPTICAL
    "(😍_😍)",   // LOVE
    "(O_O;)",    // SHOCK
    "(x_x)",     // DEAD
    "♫(•‿•)♫",   // MUSIC
    "🎧(•_•)🎧", // GAMER
    "(+_+)",     // SICK
    "($-$)",     // RICH
    "(😈_😈)",   // EVIL
    "(?_?)",     // CONFUSED
    "(-_-)",     // BLINK (Igual BORED mas usado pra transição)
    "(͡° ͜ʖ ͡°)",   // SUGGESTIVE
    "(>‿o)",     // WINK
    "(T_T)",     // CRYING
    "⑀⑆⑀⑆",      // GLITCH
    "010101",    // MATRIX

    "🐉" // ICON
};

/**
 * @brief Nome de cada face (para debug/config)
 */
static const char *FACE_NAMES[] = {
    "look_r", "look_l", "look_r_happy", "look_l_happy", "sleep", "sleep2",
    "awake", "bored", "intense", "cool", "happy", "grateful", "excited",
    "motivated", "demotivated", "lonely", "sad", "angry", "friend", "broken",
    "debug", "upload", "upload1", "upload2",

    // NOVOS
    "smart", "mask", "king", "attack", "victory", "skeptical", "love", "shock",
    "dead", "music", "gamer", "sick", "rich", "evil", "confused", "blink",
    "suggestive", "wink", "crying", "glitch", "matrix",

    "icon"};

/**
 * @brief Classe para gerenciar as faces do mascote
 */
class MascotFaces {
public:
  MascotFaces();

  // Obtém a face atual
  MascotFaceType getCurrentFace() const { return _currentFace; }
  const char *getCurrentFaceASCII() const { return FACE_ASCII[_currentFace]; }
  const char *getCurrentFaceName() const { return FACE_NAMES[_currentFace]; }

  // Define a face
  void setFace(MascotFaceType face);
  void setFaceByName(const char *name);

  // Animação automática entre faces
  void update();

  // Transição suave entre faces
  void transitionTo(MascotFaceType face, uint16_t durationMs = 300);
  bool isTransitioning() const { return _transitioning; }

  // Faces baseadas em estado
  void setIdle();     // Vai para BORED após tempo ocioso
  void setScanning(); // INTENSE durante scan
  void setSuccess();  // EXCITED quando captura handshake
  void setError();    // SAD/BROKEN quando erro
  void setSleeping(); // SLEEP quando ocioso muito tempo

  // Animação de piscar
  void enableBlink(bool enable) { _blinkEnabled = enable; }

  // Offset Y para efeito de respiração (retorna -2 a +2 px)
  int8_t getBreathingOffset();

  // Eye-tracking simulado - dragão olha para direção do toque
  void lookAt(int16_t touchX, int16_t screenWidth);
  void lookAtHappy(int16_t touchX, int16_t screenWidth);

  // Cor por nível - dragão muda de cor conforme evolui
  uint32_t getLevelColor(uint8_t level);
  const char *getLevelTitle(uint8_t level);

private:
  MascotFaceType _currentFace;
  MascotFaceType _targetFace;
  bool _transitioning;
  unsigned long _transitionStart;
  uint16_t _transitionDuration;

  bool _blinkEnabled;
  unsigned long _lastBlink;
  unsigned long _idleStart;

  // Estado do Blink
  bool _isBlinking;
  MascotFaceType _preBlinkFace;

  static const unsigned long IDLE_TIMEOUT = 30000;   // 30s para BORED
  static const unsigned long SLEEP_TIMEOUT = 180000; // 3min para SLEEP
};

extern MascotFaces mascotFaces;
