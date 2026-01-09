#pragma once

/**
 * @file ui_avatar.h
 * @brief Avatar visual do assistente de voz (Dragão animado)
 */

#include "mascot_faces.h"
#include <Arduino.h>
#include <lvgl.h>

/**
 * @brief Estados do avatar
 */
enum AvatarState {
  AVATAR_IDLE,      // Normal, piscando ocasionalmente
  AVATAR_LISTENING, // Ouvindo (aura pulsante)
  AVATAR_SPEAKING,  // Falando (boca animada)
  AVATAR_THINKING,  // Processando (loading spinner)
  AVATAR_HAPPY,     // Feliz (animação de comemoração)
  AVATAR_SLEEPING   // Dormindo (Zzz)
};

/**
 * @brief Componente visual do assistente de voz
 */
class VoiceAvatar {
public:
  VoiceAvatar();

  /**
   * @brief Cria o avatar em um container pai
   */
  void create(lv_obj_t *parent);

  /**
   * @brief Atualiza animações
   */
  void update();

  /**
   * @brief Define o estado do avatar
   */
  void setState(AvatarState state);
  AvatarState getState() const { return _state; }

  /**
   * @brief Define a expressão facial
   */
  void setFace(MascotFaceType face);

  /**
   * @brief Define o texto de fala/humor
   */
  void setText(const char *text);
  void setSubtext(const char *text);

  /**
   * @brief Ativa efeito de glow
   */
  void setGlowColor(uint32_t color);
  void setGlowEnabled(bool enable);

  /**
   * @brief Define nome do assistente
   */
  void setName(const char *name);

  /**
   * @brief Animações especiais
   */
  void playWaveAnimation();
  void playSuccessAnimation();
  void playErrorAnimation();

  /**
   * @brief Mostra/oculta
   */
  void show();
  void hide();

  /**
   * @brief Interatividade (Tip 29)
   */
  void setFocus(int x, int y);     // Olhos seguem (x,y) relativo ao centro
  void reactToTouch();             // Reage ao toque (pulo/expressão)
  void checkNoseTap(int x, int y); // Item 50: Easter Egg

private:
  lv_obj_t *_container;
  lv_obj_t *_avatarCircle;     // Círculo de fundo com glow
  lv_obj_t *_faceLabel;        // Emoji/texto do rosto
  lv_obj_t *_nameLabel;        // Nome "DRAGON"
  lv_obj_t *_textLabel;        // Texto principal
  lv_obj_t *_subtextLabel;     // Subtexto
  lv_obj_t *_auraRing;         // Anel de aura pulsante
  lv_obj_t *_listeningDots[3]; // Pontos de "ouvindo"

  AvatarState _state;
  MascotFaceType _currentFace;
  uint32_t _glowColor;
  bool _glowEnabled;

  int _frameCount;
  int _blinkTimer;
  int _dotAnimFrame;
  int _noseTapCount;
  uint32_t _lastTapTime;

  void updateIdleAnimation();
  void updateListeningAnimation();
  void updateSpeakingAnimation();
  void updateThinkingAnimation();
  void createAuraEffect();
};

extern VoiceAvatar voiceAvatar;
