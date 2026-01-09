#pragma once

/**
 * Wrapper de compatibilidade para WiFiServer.h
 *
 * Este arquivo serve como ponte de compatibilidade para código que
 * inclui "WiFiServer.h" diretamente. No framework Arduino ESP32,
 * todas as classes WiFi (incluindo WiFiServer) são incluídas
 * automaticamente quando você inclui Arduino.h ou WiFi.h.
 *
 * A classe WiFiServer está disponível globalmente após o include
 * do framework principal.
 */

// O Arduino.h do ESP32 já inclui tudo necessário
// Não precisamos incluir <WiFi.h> explicitamente porque
// ele já está disponível através do framework