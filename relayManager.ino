#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#define ELEGANTOTA_USE_ASYNC_WEBSERVER 1
#include <ElegantOTA.h>
#include "secret.h"
#include "RelayNames.h"
#include "html_pages.h"
#include "style.css.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "relay_control.h"
#include "RelayScheduler.h"
#include "WebHandler.h" 
#include "TimeManager.h"
#include "LogManager.h"


RelayScheduler scheduler(RELAY_PINS, RELAY_COUNT);

RelayNameManager relayManager; 
WifiManager wifiManager(15);  // timeout 15s

MQTTManager mqttManager(MQTT_SERVER_IP);
WebHandler webHandler(server, scheduler, relayManager, RELAY_PINS, RELAY_COUNT); 


bool isAuthenticated(AsyncWebServerRequest *request) {
  if (!request->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
    request->requestAuthentication();
    return false;
  }
  return true;
}

void setup() {
  delay(10);
  Serial.begin(115200);
  delay(1000);
  Serial.println("Démarrage du setup");
  
  setupRelays();
  wifiManager.begin();
  yield();
  mqttManager.begin();
  mqttManager.reconnect();
  yield();
  relayManager.begin();  
  webHandler.setupRoutes();
  yield();
  ElegantOTA.begin(&server);  
  yield();
  server.begin();
  TimeManager::setupTime();
  yield();
  scheduler.loadSchedules();
  yield();

  LogManager::log("Système démarré.");

}

void loop() {

  if (!mqttManager.connected()) mqttManager.reconnect();
    mqttManager.loop();
    ElegantOTA.loop();
    static unsigned long lastUpdate = 0;
    const unsigned long interval = 20000;
    unsigned long now = millis();
    if (now - lastUpdate >= interval) {
      lastUpdate = now;
      scheduler.update();
    }

}