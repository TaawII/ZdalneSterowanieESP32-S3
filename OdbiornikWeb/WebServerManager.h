#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

#include "Types.h"

class WebServerManager {

public:

    WebServerManager(WebData &webData);

    void begin();
    void loop();
    unsigned long getLastPhoneReceive();

private:

    WebServer server;
    WebSocketsServer webSocket;
    WebData &webData;

    unsigned long lastSend;
    unsigned long lastPhoneReceive;

    void handleRoot();

    void sendData();

    void webSocketEvent(
        uint8_t num,
        WStype_t type,
        uint8_t *payload,
        size_t length
    );

    static WebServerManager* instance;

    static void handleRootStatic();

    static void webSocketEventStatic(
        uint8_t num,
        WStype_t type,
        uint8_t *payload,
        size_t length
    );
};

#endif