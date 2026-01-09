#pragma once
/**
 * @file ui_themes.h
 * @brief Sistema de temas para WavePwn
 *
 * Define as cores e estilos para cada tema disponível
 */

#include "../core/globals.h"
#include <lvgl.h>

// Estrutura de cores do tema
// Estrutura de cores do tema
struct ThemeColors {
  lv_color_t bg;         // Fundo principal
  lv_color_t panel;      // Painéis/cards
  lv_color_t primary;    // Cor primária (botões, destaques)
  lv_color_t secondary;  // Cor secundária
  lv_color_t accent;     // Cor de destaque
  lv_color_t danger;     // Cor de perigo/erro
  lv_color_t warning;    // Cor de aviso
  lv_color_t success;    // Cor de sucesso
  lv_color_t text;       // Texto principal
  lv_color_t text_dim;   // Texto secundário
  uint8_t radius;        // Raio de borda (0=Rect, >0=Rounded)
  uint8_t glass_opacity; // Opacidade do vidro (0-255)
};

// Font Macros - Standardized per Tip 13, 14, 15
// Note: As valid binary fonts were not provided, we map to closest available
// LVGL built-ins. JetBrains Mono Bold 24-32px -> Montserrat 28
#define FONT_TITLE &lv_font_montserrat_28
// Roboto Black 64-80px -> Montserrat 48 (largest standard)
#define FONT_LARGE_NUM &lv_font_montserrat_48
// Inter Medium 18-20px -> Montserrat 20
#define FONT_TEXT_SEC &lv_font_montserrat_20
// Small text for status bar etc
#define FONT_SMALL &lv_font_montserrat_12
// Icons
#define FONT_ICON_MD &lv_font_montserrat_24
#define FONT_ICON_LG &lv_font_montserrat_32

// Layout Constants (Tips 1, 9, 10)
#define STATUS_BAR_HEIGHT 40
#define NAV_BAR_HEIGHT 40
#define CONTENT_HEIGHT (LCD_HEIGHT - STATUS_BAR_HEIGHT - NAV_BAR_HEIGHT) // 368
#define GRID_UNIT 8
#define MARGIN_SIDE 12
#define MARGIN_TOP_BOTTOM 8

// Neon Palette Constants
#define COLOR_NEON_GREEN LV_COLOR_MAKE(0x00, 0xFF, 0x88)
#define COLOR_NEON_RED LV_COLOR_MAKE(0xFF, 0x00, 0x44)
#define COLOR_NEON_BLUE LV_COLOR_MAKE(0x00, 0xDD, 0xFF)
#define COLOR_NEON_PURPLE LV_COLOR_MAKE(0xBC, 0x13, 0xFE)
#define COLOR_AMOLED_BLACK LV_COLOR_MAKE(0x00, 0x00, 0x00)

// Temas predefinidos
static const ThemeColors themes[] = {
    // THEME_HACKER_DARK - Verde neon em fundo preto
    {.bg = LV_COLOR_MAKE(0x0D, 0x11, 0x17),
     .panel = LV_COLOR_MAKE(0x16, 0x1B, 0x22),
     .primary = LV_COLOR_MAKE(0x23, 0x86, 0x36),
     .secondary = LV_COLOR_MAKE(0x1F, 0x6F, 0xEB),
     .accent = LV_COLOR_MAKE(0x00, 0xFF, 0x41),
     .danger = LV_COLOR_MAKE(0xDA, 0x36, 0x33),
     .warning = LV_COLOR_MAKE(0xD2, 0x99, 0x22),
     .success = LV_COLOR_MAKE(0x23, 0x86, 0x36),
     .text = LV_COLOR_MAKE(0xC9, 0xD1, 0xD9),
     .text_dim = LV_COLOR_MAKE(0x6E, 0x76, 0x81),
     .radius = 4,
     .glass_opacity = 220},

    // THEME_CYBERPUNK - Rosa/roxo neon (Cyber Purple)
    {.bg = LV_COLOR_MAKE(0x0A, 0x0A, 0x1A),
     .panel = LV_COLOR_MAKE(0x1A, 0x1A, 0x2E),
     .primary = LV_COLOR_MAKE(0xFF, 0x00, 0x7F),
     .secondary = LV_COLOR_MAKE(0x00, 0xD4, 0xFF),
     .accent = LV_COLOR_MAKE(0x9D, 0x00, 0xFF),
     .danger = LV_COLOR_MAKE(0xFF, 0x00, 0x00),
     .warning = LV_COLOR_MAKE(0xFF, 0xD9, 0x00),
     .success = LV_COLOR_MAKE(0x00, 0xFF, 0x9F),
     .text = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF),
     .text_dim = LV_COLOR_MAKE(0x80, 0x80, 0xA0),
     .radius = 8,
     .glass_opacity = 200},

    // THEME_MATRIX - Verde matrix clássico
    {.bg = LV_COLOR_MAKE(0x00, 0x00, 0x00),
     .panel = LV_COLOR_MAKE(0x00, 0x1A, 0x00),
     .primary = LV_COLOR_MAKE(0x00, 0xFF, 0x00),
     .secondary = LV_COLOR_MAKE(0x00, 0xAA, 0x00),
     .accent = LV_COLOR_MAKE(0x00, 0xFF, 0x00),
     .danger = LV_COLOR_MAKE(0xFF, 0x00, 0x00),
     .warning = LV_COLOR_MAKE(0xAA, 0xAA, 0x00),
     .success = LV_COLOR_MAKE(0x00, 0xFF, 0x00),
     .text = LV_COLOR_MAKE(0x00, 0xFF, 0x00),
     .text_dim = LV_COLOR_MAKE(0x00, 0x80, 0x00),
     .radius = 0,
     .glass_opacity = 240},

    // THEME_OCEAN - Azul ciano
    {.bg = LV_COLOR_MAKE(0x0A, 0x14, 0x1E),
     .panel = LV_COLOR_MAKE(0x10, 0x20, 0x30),
     .primary = LV_COLOR_MAKE(0x00, 0xBF, 0xFF),
     .secondary = LV_COLOR_MAKE(0x00, 0xFF, 0xFF),
     .accent = LV_COLOR_MAKE(0x00, 0x80, 0xFF),
     .danger = LV_COLOR_MAKE(0xFF, 0x44, 0x44),
     .warning = LV_COLOR_MAKE(0xFF, 0xAA, 0x00),
     .success = LV_COLOR_MAKE(0x00, 0xDD, 0xAA),
     .text = LV_COLOR_MAKE(0xE0, 0xF0, 0xFF),
     .text_dim = LV_COLOR_MAKE(0x60, 0x80, 0xA0),
     .radius = 6,
     .glass_opacity = 210},

    // THEME_BLOOD - Vermelho escuro
    {.bg = LV_COLOR_MAKE(0x0A, 0x00, 0x00),
     .panel = LV_COLOR_MAKE(0x1A, 0x05, 0x05),
     .primary = LV_COLOR_MAKE(0xCC, 0x00, 0x00),
     .secondary = LV_COLOR_MAKE(0xFF, 0x44, 0x44),
     .accent = LV_COLOR_MAKE(0xFF, 0x00, 0x00),
     .danger = LV_COLOR_MAKE(0xFF, 0x00, 0x00),
     .warning = LV_COLOR_MAKE(0xFF, 0x66, 0x00),
     .success = LV_COLOR_MAKE(0x88, 0xFF, 0x00),
     .text = LV_COLOR_MAKE(0xFF, 0xDD, 0xDD),
     .text_dim = LV_COLOR_MAKE(0x80, 0x40, 0x40),
     .radius = 2,
     .glass_opacity = 230},

    // THEME_DARK_DRAGON - Padrão (Roxo escuro e vermelho)
    {.bg = LV_COLOR_MAKE(0x05, 0x05, 0x05),
     .panel = LV_COLOR_MAKE(0x10, 0x05, 0x10),
     .primary = LV_COLOR_MAKE(0x88, 0x00, 0xFF),   // Roxo
     .secondary = LV_COLOR_MAKE(0xFF, 0x00, 0x44), // Vermelho
     .accent = LV_COLOR_MAKE(0xFF, 0x00, 0x88),    // Magenta
     .danger = LV_COLOR_MAKE(0xFF, 0x00, 0x00),
     .warning = LV_COLOR_MAKE(0xFF, 0x88, 0x00),
     .success = LV_COLOR_MAKE(0x00, 0xFF, 0x00),
     .text = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF),
     .text_dim = LV_COLOR_MAKE(0xAA, 0x88, 0xAA),
     .radius = 12,
     .glass_opacity = 180},

    // THEME_TRUE_BLACK - AMOLED Saver (Contraste máximo)
    {.bg = LV_COLOR_MAKE(0x00, 0x00, 0x00),
     .panel = LV_COLOR_MAKE(0x00, 0x00, 0x00),     // Painel preto também
     .primary = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF),   // Branco puro
     .secondary = LV_COLOR_MAKE(0x80, 0x80, 0x80), // Cinza
     .accent = LV_COLOR_MAKE(0xFF, 0xFF, 0x00),    // Amarelo (visibilidade)
     .danger = LV_COLOR_MAKE(0xFF, 0x00, 0x00),
     .warning = LV_COLOR_MAKE(0xFF, 0xFF, 0x00),
     .success = LV_COLOR_MAKE(0x00, 0xFF, 0x00),
     .text = LV_COLOR_MAKE(0xFF, 0xFF, 0xFF),
     .text_dim = LV_COLOR_MAKE(0x80, 0x80, 0x80),
     .radius = 0, // Reto para economizar processamento de AA (teoria)
     .glass_opacity = 255},

    // THEME_RETRO_MIAMI - Neon Pastel
    {.bg = LV_COLOR_MAKE(0x20, 0x10, 0x24),
     .panel = LV_COLOR_MAKE(0x30, 0x18, 0x38),
     .primary = LV_COLOR_MAKE(0xFF, 0x00, 0xAA),   // Rosa Choque
     .secondary = LV_COLOR_MAKE(0x00, 0xEE, 0xFF), // Ciano
     .accent = LV_COLOR_MAKE(0xFF, 0xAA, 0x00),    // Laranja
     .danger = LV_COLOR_MAKE(0xFF, 0x44, 0x44),
     .warning = LV_COLOR_MAKE(0xFF, 0xEE, 0x00),
     .success = LV_COLOR_MAKE(0x44, 0xFF, 0x88),
     .text = LV_COLOR_MAKE(0xFF, 0xEE, 0xFF),
     .text_dim = LV_COLOR_MAKE(0xAA, 0x88, 0xAA),
     .radius = 12, // Bem arredondado
     .glass_opacity = 210},

    // THEME_ICE_BLUE - Frio e Clean
    {.bg = LV_COLOR_MAKE(0x0A, 0x11, 0x1A),
     .panel = LV_COLOR_MAKE(0x14, 0x22, 0x33),
     .primary = LV_COLOR_MAKE(0x66, 0xCC, 0xFF), // Azul Gelo
     .secondary = LV_COLOR_MAKE(0x33, 0x99, 0xFF),
     .accent = LV_COLOR_MAKE(0xCC, 0xFF, 0xFF), // Branco Gelo
     .danger = LV_COLOR_MAKE(0xFF, 0x55, 0x55),
     .warning = LV_COLOR_MAKE(0xFF, 0xCC, 0x00),
     .success = LV_COLOR_MAKE(0x55, 0xFF, 0x99),
     .text = LV_COLOR_MAKE(0xEE, 0xF5, 0xFF),
     .text_dim = LV_COLOR_MAKE(0x88, 0xAA, 0xCC),
     .radius = 6,
     .glass_opacity = 200},

    // THEME_STEALTH - Modo noturno/discreto (preto puro + vermelho minimo)
    {.bg = LV_COLOR_MAKE(0x00, 0x00, 0x00),
     .panel = LV_COLOR_MAKE(0x00, 0x00, 0x00),
     .primary = LV_COLOR_MAKE(0x66, 0x00, 0x00), // Vermelho escuro
     .secondary = LV_COLOR_MAKE(0x44, 0x00, 0x00),
     .accent = LV_COLOR_MAKE(0xFF, 0x00, 0x00), // Vermelho puro (contornos)
     .danger = LV_COLOR_MAKE(0xFF, 0x00, 0x00),
     .warning = LV_COLOR_MAKE(0x88, 0x44, 0x00),
     .success = LV_COLOR_MAKE(0x00, 0x44, 0x00),
     .text = LV_COLOR_MAKE(0x88, 0x00, 0x00), // Texto vermelho escuro
     .text_dim = LV_COLOR_MAKE(0x44, 0x00, 0x00),
     .radius = 0,
     .glass_opacity = 255},

    // THEME_HACKER_HOLLYWOOD - Scanlines, glitch, OCR-A style
    {.bg = LV_COLOR_MAKE(0x00, 0x08, 0x00), // Quase preto esverdeado
     .panel = LV_COLOR_MAKE(0x00, 0x10, 0x00),
     .primary = LV_COLOR_MAKE(0x00, 0xFF, 0x00), // Verde puro
     .secondary = LV_COLOR_MAKE(0x00, 0xCC, 0x00),
     .accent = LV_COLOR_MAKE(0x88, 0xFF, 0x88), // Verde claro
     .danger = LV_COLOR_MAKE(0xFF, 0x00, 0x00),
     .warning = LV_COLOR_MAKE(0xFF, 0xFF, 0x00),
     .success = LV_COLOR_MAKE(0x00, 0xFF, 0x00),
     .text = LV_COLOR_MAKE(0x00, 0xFF, 0x00), // Texto verde puro
     .text_dim = LV_COLOR_MAKE(0x00, 0x88, 0x00),
     .radius = 0, // Cantos retos
     .glass_opacity = 240}};

#define THEME_CUSTOM_DRAGON 11
#define THEME_COUNT 12

// Dynamic theme storage
extern ThemeColors g_dynamic_theme;
void ui_update_theme_from_dragon_color(uint32_t color);

// Função para obter cores do tema atual
inline const ThemeColors &getTheme() {
  if (g_state.current_theme == THEME_CUSTOM_DRAGON) {
    return g_dynamic_theme;
  }
  if (g_state.current_theme >= THEME_COUNT)
    g_state.current_theme = THEME_DARK_DRAGON;
  return themes[g_state.current_theme];
}

// Macros de conveniência para usar o tema atual
#define THEME_BG getTheme().bg
#define THEME_PANEL getTheme().panel
#define THEME_PRIMARY getTheme().primary
#define THEME_SECONDARY getTheme().secondary
#define THEME_ACCENT getTheme().accent
#define THEME_DANGER getTheme().danger
#define THEME_WARNING getTheme().warning
#define THEME_SUCCESS getTheme().success
#define THEME_TEXT getTheme().text
#define THEME_TEXT_DIM getTheme().text_dim
#define THEME_RADIUS getTheme().radius

// Nomes dos temas para UI
static const char *theme_names[] = {
    "Hacker Dark", "CyberPunk",   "Matrix",           "Ocean",
    "Blood",       "Dark Dragon", "True Black",       "Retro Miami",
    "Ice Blue",    "Stealth",     "Hacker Hollywood", "Dragon Link"};
