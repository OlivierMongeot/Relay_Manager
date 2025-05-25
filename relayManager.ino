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
#include <esp_system.h>

RelayScheduler scheduler(RELAY_PINS, RELAY_COUNT);

RelayNameManager relayManager; 
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
  delay(1000);

  if (!SPIFFS.exists("/logs.txt")) {
      Serial.println("⚠️ Fichier /logs.txt inexistant !");
  }
  SPIFFS.begin(true);
  LogManager::begin();
  LogManager::log("Système démarré.");

  esp_reset_reason_t reason = esp_reset_reason();
  LogManager::logf("[BOOT] Reset Reason: %d\n", reason);
  LogManager::logf("[BOOT] Uptime actuel (ms) : %lu\n", millis());

  setupRelays();
  wifiManager.begin();
  delay(200);
  mqttManager.begin();
  mqttManager.reconnect();
  delay(100);
  relayManager.begin();  
  webHandler.setupRoutes();
  delay(100);
  ElegantOTA.begin(&server);  
  delay(10);
  server.begin();
  LogManager::log("Serveur Web démarré");
  TimeManager::setupTime();
  delay(100);
  scheduler.loadSchedules();
  delay(100);
  ws.onEvent(WsManager::onWsEvent);
  server.addHandler(&ws);
  LogManager::log("Setup done");
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