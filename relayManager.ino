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
#include "log_http.h" 


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
  delay(100);
  Serial.begin(115200);
  Serial.println("Démarrage du setup");
  setupRelays();
  wifiManager.begin();
  mqttManager.begin();
  mqttManager.reconnect();
  relayManager.begin();  
  webHandler.setupRoutes();
  ElegantOTA.begin(&server);  
  yield();
  server.begin();
  TimeManager::setupTime();
  yield();
  scheduler.loadSchedules();

  yield();

  sendLogHttp("Système démarré");
  yield();
  
  scheduler.syncRelaysWithCurrentTime();  // ✅ force la cohérence
}

void loop() {

  if (!mqttManager.connected()) mqttManager.reconnect();

  mqttManager.loop();
  ElegantOTA.loop();
  
  static unsigned long lastUpdate = 0;
  const unsigned long interval = 20000; // 20 secondes 
  unsigned long now = millis();
  if (now - lastUpdate >= interval) {
      lastUpdate = now;
      scheduler.update();
    }
}