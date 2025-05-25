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
#include "WsManager.h"
#include "LogManager.h"

RelayScheduler scheduler(RELAY_PINS, RELAY_COUNT);
// AsyncWebServer server(80);
RelayNameManager relayManager;
// WifiManager wifiManager(SSID, PASSWORD,30); 
WifiManager wifiManager(15);  // timeout 15s

MQTTManager mqttManager(MQTT_SERVER_IP);
WebHandler webHandler(server, scheduler, relayManager, RELAY_PINS, RELAY_COUNT); 

AsyncWebSocket ws("/ws");

bool isAuthenticated(AsyncWebServerRequest *request) {
  if (!request->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
    request->requestAuthentication();
    return false;
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("Démarrage...");
  setupRelays();
  wifiManager.begin();
  mqttManager.begin();
  mqttManager.reconnect();
  relayManager.begin();  
  webHandler.setupRoutes();
  delay(50);
  ElegantOTA.begin(&server);  

  server.begin();
  Serial.println("Serveur Web démarré");

  TimeManager::setupTime();
  
  SPIFFS.begin(true);

  scheduler.loadSchedules();

  ws.onEvent(WsManager::onWsEvent);
  server.addHandler(&ws);

  LogManager::begin();
  LogManager::log("Système démarré.");

    if (!SPIFFS.exists("/logs.txt")) {
    Serial.println("⚠️ Fichier /logs.txt inexistant !");
  }

}


void loop() {

  if (!mqttManager.connected()) mqttManager.reconnect();

  mqttManager.loop();
  ElegantOTA.loop();

  static unsigned long lastUpdate = 0;
  const unsigned long interval = 10000;
  unsigned long now = millis();

  if (now - lastUpdate >= interval) {
    lastUpdate = now;
    scheduler.update();
  
  }

 
}