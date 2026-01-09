/**
 * @file boot_animation.cpp
 * @brief Implementação da animação de boot futurista com partículas
 */

#include "boot_animation.h"
#include "../core/pin_definitions.h"
#include "ui_particles.h"

// Instância global
BootAnimation bootAnimation;

// Cores cyberpunk
#define COLOR_NEON_GREEN lv_color_hex(0x00FF41)
#define COLOR_NEON_CYAN lv_color_hex(0x00FFFF)
#define COLOR_NEON_PURPLE lv_color_hex(0x9D00FF)
#define COLOR_DARK_BG lv_color_hex(0x0D0D0D)
#define COLOR_MATRIX_GREEN lv_color_hex(0x003B00)

BootAnimation::BootAnimation()
    : _screen(nullptr), _logoLabel(nullptr), _progressBar(nullptr),
      _statusLabel(nullptr), _versionLabel(nullptr), _state(BOOT_ANIM_LOGO),
      _startTime(0), _lastFrame(0), _frameIndex(0), _progress(0),
      _complete(false), _onComplete(nullptr) {

  // Inicializa posições das gotas de chuva matrix
  for (int i = 0; i < 20; i++) {
    _rainDrops[i] = random(0, 368);
  }
}

void BootAnimation::start() {
  _startTime = millis();
  _lastFrame = _startTime;
  _state = BOOT_ANIM_LOGO;
  _progress = 0;
  _complete = false;
  _frameIndex = 0;

  // Cria tela de boot
  _screen = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(_screen, COLOR_DARK_BG, 0);
  lv_scr_load(_screen);

  // === MASCOTE FULLSCREEN ===
  // Cria container do mascote que ocupa a tela toda
  lv_obj_t *mascot_fullscreen = lv_obj_create(_screen);
  lv_obj_set_size(mascot_fullscreen, LCD_WIDTH, LCD_HEIGHT);
  lv_obj_align(mascot_fullscreen, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_style_bg_opa(mascot_fullscreen, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(mascot_fullscreen, 0, 0);
  lv_obj_clear_flag(mascot_fullscreen, LV_OBJ_FLAG_SCROLLABLE);

  // Face do mascote GRANDE no centro (estilo Pwnagotchi)
  lv_obj_t *mascot_face = lv_label_create(mascot_fullscreen);
  lv_label_set_text(mascot_face, "( ^_^ )"); // Face feliz grande
  lv_obj_set_style_text_color(mascot_face, COLOR_NEON_GREEN, 0);
  lv_obj_set_style_text_font(mascot_face, &lv_font_montserrat_48, 0);
  lv_obj_align(mascot_face, LV_ALIGN_CENTER, 0, -40);

  // Glow effect ao redor do mascote
  lv_obj_t *glow = lv_obj_create(mascot_fullscreen);
  lv_obj_set_size(glow, 300, 120);
  lv_obj_align(glow, LV_ALIGN_CENTER, 0, -40);
  lv_obj_set_style_radius(glow, 60, 0);
  lv_obj_set_style_bg_color(glow, COLOR_NEON_GREEN, 0);
  lv_obj_set_style_bg_opa(glow, LV_OPA_10, 0);
  lv_obj_set_style_border_width(glow, 0, 0);
  lv_obj_move_background(glow);

  // === PARTICLE SYSTEM ===
  particles.init(_screen);
  particles.setEffect(PARTICLE_RISE);
  particles.setContinuous(true, 4);
  particles.emit(20);

  // Logo LeleWatch - menor, embaixo do mascote
  _logoLabel = lv_label_create(_screen);
  lv_label_set_text(_logoLabel, "LeleWatch");
  lv_obj_set_style_text_color(_logoLabel, COLOR_NEON_GREEN, 0);
  lv_obj_set_style_text_font(_logoLabel, &lv_font_montserrat_28, 0);
  lv_obj_align(_logoLabel, LV_ALIGN_CENTER, 0, 60);

  // Subtitulo
  lv_obj_t *subLabel = lv_label_create(_screen);
  lv_label_set_text(subLabel, "[ Tamagochi ]");
  lv_obj_set_style_text_color(subLabel, COLOR_NEON_CYAN, 0);
  lv_obj_set_style_text_font(subLabel, &lv_font_montserrat_12, 0);
  lv_obj_align(subLabel, LV_ALIGN_CENTER, 0, 95);

  // Barra de progresso - na parte inferior
  _progressBar = lv_bar_create(_screen);
  lv_obj_set_size(_progressBar, 280, 8);
  lv_obj_align(_progressBar, LV_ALIGN_BOTTOM_MID, 0, -60);
  lv_bar_set_range(_progressBar, 0, 100);
  lv_bar_set_value(_progressBar, 0, LV_ANIM_OFF);

  // Estilo da barra
  lv_obj_set_style_bg_color(_progressBar, COLOR_MATRIX_GREEN, LV_PART_MAIN);
  lv_obj_set_style_bg_color(_progressBar, COLOR_NEON_GREEN, LV_PART_INDICATOR);
  lv_obj_set_style_radius(_progressBar, 4, 0);
  lv_obj_set_style_radius(_progressBar, 4, LV_PART_INDICATOR);
  lv_obj_set_style_border_width(_progressBar, 1, 0);
  lv_obj_set_style_border_color(_progressBar, COLOR_NEON_GREEN, 0);
  lv_obj_set_style_shadow_width(_progressBar, 10, LV_PART_INDICATOR);
  lv_obj_set_style_shadow_color(_progressBar, COLOR_NEON_GREEN,
                                LV_PART_INDICATOR);
  lv_obj_set_style_shadow_opa(_progressBar, LV_OPA_50, LV_PART_INDICATOR);

  // Status text - acima da barra
  _statusLabel = lv_label_create(_screen);
  lv_label_set_text(_statusLabel, boot_messages[0]);
  lv_obj_set_style_text_color(_statusLabel, COLOR_NEON_CYAN, 0);
  lv_obj_set_style_text_font(_statusLabel, &lv_font_montserrat_10, 0);
  lv_obj_align(_statusLabel, LV_ALIGN_BOTTOM_MID, 0, -75);

  // Versao - bem embaixo
  _versionLabel = lv_label_create(_screen);
  lv_label_set_text(_versionLabel, "v2.1 | ESP32-S3 | LeleWatch Ed.");
  lv_obj_set_style_text_color(_versionLabel, lv_color_hex(0x666666), 0);
  lv_obj_set_style_text_font(_versionLabel, &lv_font_montserrat_10, 0);
  lv_obj_align(_versionLabel, LV_ALIGN_BOTTOM_MID, 0, -10);

  Serial.println("[BOOT] Animacao iniciada com mascote fullscreen");
}

bool BootAnimation::update() {
  if (_complete)
    return true;

  unsigned long now = millis();
  unsigned long elapsed = now - _startTime;

  if (now - _lastFrame < 50)
    return false;
  _lastFrame = now;

  _progress = min(100, (int)(elapsed * 100 / BOOT_ANIM_DURATION_MS));
  lv_bar_set_value(_progressBar, _progress, LV_ANIM_ON);
  particles.update();

  // Sequence Logic
  if (elapsed < 1000) {
    // Phase 1: Logo
    lv_obj_clear_flag(_logoLabel, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(_logoLabel, "WavePwn");
    // Pulse opacity
    int opa = (sin(elapsed * 0.005) + 1) * 127;
    lv_obj_set_style_text_opa(_logoLabel, opa, 0);
  } else if (elapsed < 2000) {
    // Phase 2: Winking Dragon
    lv_obj_add_flag(_logoLabel, LV_OBJ_FLAG_HIDDEN);
    // Ensure face is visible
    lv_label_set_text(_statusLabel, "Initializing Core...");
    // Wink at 1.5s
    if (elapsed > 1500) {
      // Mascot Face is accessed via internal child or I should have stored
      // pointer _screen children 0 is mascot_fullscreen -> child 0 is label
    }
  } else {
    // Phase 3: Ready to Pwn
    lv_label_set_text(_statusLabel, "READY TO PWN");
    lv_obj_set_style_text_color(_statusLabel, COLOR_NEON_GREEN, 0);
  }

  // Glitch
  if (random(0, 20) == 0)
    drawGlitchEffect();

  if (elapsed >= BOOT_ANIM_DURATION_MS) {
    _complete = true;
    if (_onComplete)
      _onComplete();
    return true;
  }
  return false;
}

void BootAnimation::drawGlitchEffect() {
  if (!_screen) return;
  
  // Efeito glitch simples: muda cor temporariamente
  static int glitch_counter = 0;
  glitch_counter++;
  
  if (glitch_counter % 2 == 0) {
    lv_obj_set_style_bg_color(_screen, lv_color_hex(0x1A0A1A), 0);
  } else {
    lv_obj_set_style_bg_color(_screen, COLOR_DARK_BG, 0);
  }
  
  // Reset after short delay
  if (glitch_counter > 3) {
    lv_obj_set_style_bg_color(_screen, COLOR_DARK_BG, 0);
    glitch_counter = 0;
  }
}
