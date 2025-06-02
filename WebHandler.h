#ifndef WEB_HANDLER_H
#define WEB_HANDLER_H

#include <ESPAsyncWebServer.h>
#include "RelayScheduler.h"
#include "RelayNames.h"

class WebHandler {
public:
    public:
    WebHandler(
      AsyncWebServer& _server,
      RelayScheduler& _scheduler,
      RelayNameManager& _relayManager,
      const int* _relayPins,
      int _relayCount
    );

    void setupRoutes();

private:
    AsyncWebServer& _server;
    RelayScheduler& _scheduler;
    RelayNameManager& _relayManager;
    const int* _relayPins;
    int _relayCount;
    String formatTime(int hour, int minute);
    bool isAuthenticated(AsyncWebServerRequest *request);
    void handleRoot(AsyncWebServerRequest *request);
    void handleConfig(AsyncWebServerRequest *request);
    void handleSave(AsyncWebServerRequest *request);
    void handleSchedule(AsyncWebServerRequest *request);
    void handleDeleteSchedule(AsyncWebServerRequest *request);
    void handleAddSchedule(AsyncWebServerRequest *request);
    void handleRelayStatus(AsyncWebServerRequest *request);
};

#endif