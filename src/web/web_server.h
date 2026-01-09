#pragma once

#include "../core/config.h"
#include "../core/globals.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class WebInterface {
public:
  WebInterface();
  void begin();
  void update();

private:
  AsyncWebServer server;
  AsyncWebSocket ws;
  unsigned long _lastUpdate;

  void setupRoutes();
  void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                        AwsEventType type, void *arg, uint8_t *data,
                        size_t len);
};

extern WebInterface web_interface;
