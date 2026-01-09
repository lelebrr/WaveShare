/**
 * @file ui_language.cpp
 * @brief Implementação do sistema de idiomas
 */

#include "ui_language.h"

LanguageManager lang;

// ═══════════════════════════════════════════════════════════════════════════
// PORTUGUÊS BRASILEIRO
// ═══════════════════════════════════════════════════════════════════════════
const char *LanguageManager::_strings_ptbr[] = {
    // Menu Principal
    "Scan WiFi",
    "Scan BLE",
    "Ataques",
    "Ferramentas",
    "Configuracoes",
    "Assistente IA",

    // Ações
    "Iniciar",
    "Parar",
    "Escanear",
    "Atacar",
    "Capturar",
    "Salvar",
    "Cancelar",
    "Voltar",
    "Proximo",
    "Confirmar",

    // Status
    "Escaneando...",
    "Atacando...",
    "Ocioso",
    "Conectado",
    "Desconectado",
    "Handshake capturado!",
    "redes encontradas",
    "dispositivos encontrados",

    // Configurações
    "Display",
    "Rede",
    "Som",
    "Sistema",
    "Dragao",
    "Brilho",
    "Tema",
    "Idioma",
    "Volume",

    // Dragão
    "Nome do Dragao",
    "Personalidade",
    "Humor",

    // Diversos
    "Carregando...",
    "Erro",
    "Sucesso",
    "Aviso",
    "Bateria",
    "Hora",
    "Data",
    "Versao",

    // Lock Screen
    "Deslize para desbloquear",
    "Digite o PIN",
    "Desenhe o padrao",
    "PIN incorreto",
    "Padrao incorreto",
};

// ═══════════════════════════════════════════════════════════════════════════
// ENGLISH
// ═══════════════════════════════════════════════════════════════════════════
const char *LanguageManager::_strings_en[] = {
    // Main Menu
    "WiFi Scan",
    "BLE Scan",
    "Attacks",
    "Tools",
    "Settings",
    "AI Assistant",

    // Actions
    "Start",
    "Stop",
    "Scan",
    "Attack",
    "Capture",
    "Save",
    "Cancel",
    "Back",
    "Next",
    "Confirm",

    // Status
    "Scanning...",
    "Attacking...",
    "Idle",
    "Connected",
    "Disconnected",
    "Handshake captured!",
    "networks found",
    "devices found",

    // Settings
    "Display",
    "Network",
    "Sound",
    "System",
    "Dragon",
    "Brightness",
    "Theme",
    "Language",
    "Volume",

    // Dragon
    "Dragon Name",
    "Personality",
    "Mood",

    // Misc
    "Loading...",
    "Error",
    "Success",
    "Warning",
    "Battery",
    "Time",
    "Date",
    "Version",

    // Lock Screen
    "Swipe to unlock",
    "Enter PIN",
    "Draw pattern",
    "Wrong PIN",
    "Wrong pattern",
};

// ═══════════════════════════════════════════════════════════════════════════
// ESPAÑOL
// ═══════════════════════════════════════════════════════════════════════════
const char *LanguageManager::_strings_es[] = {
    // Menu Principal
    "Escanear WiFi",
    "Escanear BLE",
    "Ataques",
    "Herramientas",
    "Configuracion",
    "Asistente IA",

    // Acciones
    "Iniciar",
    "Detener",
    "Escanear",
    "Atacar",
    "Capturar",
    "Guardar",
    "Cancelar",
    "Volver",
    "Siguiente",
    "Confirmar",

    // Estado
    "Escaneando...",
    "Atacando...",
    "Inactivo",
    "Conectado",
    "Desconectado",
    "Handshake capturado!",
    "redes encontradas",
    "dispositivos encontrados",

    // Configuracion
    "Pantalla",
    "Red",
    "Sonido",
    "Sistema",
    "Dragon",
    "Brillo",
    "Tema",
    "Idioma",
    "Volumen",

    // Dragon
    "Nombre del Dragon",
    "Personalidad",
    "Estado de animo",

    // Varios
    "Cargando...",
    "Error",
    "Exito",
    "Advertencia",
    "Bateria",
    "Hora",
    "Fecha",
    "Version",

    // Lock Screen
    "Desliza para desbloquear",
    "Ingresa el PIN",
    "Dibuja el patron",
    "PIN incorrecto",
    "Patron incorrecto",
};

// ═══════════════════════════════════════════════════════════════════════════
// IMPLEMENTAÇÃO
// ═══════════════════════════════════════════════════════════════════════════

LanguageManager::LanguageManager() : _currentLang(LANG_PT_BR) {}

void LanguageManager::begin() {
  // Poderia carregar do NVS aqui
}

void LanguageManager::setLanguage(Language lang) {
  if (lang < LANG_COUNT) {
    _currentLang = lang;
  }
}

const char *LanguageManager::get(StringId id) const {
  if (id >= STR_COUNT)
    return "???";

  const char **table = getTable();
  return table[id];
}

const char **LanguageManager::getTable() const {
  switch (_currentLang) {
  case LANG_EN_US:
    return _strings_en;
  case LANG_ES:
    return _strings_es;
  case LANG_PT_BR:
  default:
    return _strings_ptbr;
  }
}

const char *LanguageManager::getLanguageName() const {
  switch (_currentLang) {
  case LANG_EN_US:
    return "English";
  case LANG_ES:
    return "Espanol";
  case LANG_PT_BR:
  default:
    return "Portugues";
  }
}

const char *LanguageManager::getLanguageCode() const {
  switch (_currentLang) {
  case LANG_EN_US:
    return "en-US";
  case LANG_ES:
    return "es";
  case LANG_PT_BR:
  default:
    return "pt-BR";
  }
}
