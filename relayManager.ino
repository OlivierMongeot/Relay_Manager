#define ENABLE_WEB_FEATURES  // ⛔️ Commente cette ligne pour désactiver les trucs lourds
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp_task_wdt.h"
#define ELEGANTOTA_USE_ASYNC_WEBSERVER 1
#include <ElegantOTA.h>
#include "secret.h" 
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "relay_control.h"
#include "log_http.h"

#ifdef ENABLE_WEB_FEATURES
  #include "RelayNames.h"
  #include "html_pages.h"
  #include "style.css.h"
  #include "RelayScheduler.h"
  #include "TimeManager.h"
  #include "WebHandler.h"

  RelayScheduler scheduler(RELAY_PINS, RELAY_COUNT);
  RelayNameManager relayManager; 
  WebHandler webHandler(server, scheduler, relayManager, RELAY_PINS, RELAY_COUNT); 
#endif

WifiManager wifiManager(15);  // timeout 15s
MQTTManager mqttManager(MQTT_SERVER_IP);


void setup() {
  delay(1000);
  Serial.begin(115200);
  delay(10);
  setupRelays();
  wifiManager.begin();
  delay(500);
  mqttManager.begin();
  delay(500);
  mqttManager.reconnect();
  
  #ifdef ENABLE_WEB_FEATURES
    relayManager.begin();
    delay(50);
    webHandler.setupRoutes();
    delay(50);
    TimeManager::setupTime();
    delay(50);
    scheduler.loadSchedules();
    delay(50);
    scheduler.syncRelaysWithCurrentTime();
  #endif

  ElegantOTA.begin(&server);  
  server.begin();
  esp_reset_reason_t reason = esp_reset_reason();
  sendFormattedLog("Boot reason code: %d", reason);
  delay(100);
  sendFormattedLog("Fin du démarrage, Free heap:  %d", ESP.getFreeHeap());
  delay(100);
}

void loop() {

  esp_task_wdt_reset();  

  if (!mqttManager.connected()) mqttManager.reconnect();

  mqttManager.loop();
  mqttManager.processQueue();
  ElegantOTA.loop();

  #ifdef ENABLE_WEB_FEATURES
   scheduler.processUpdate();
  #endif

  processLogQueue();  // à la fin de loop()
}