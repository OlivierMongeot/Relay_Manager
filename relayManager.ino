#define ENABLE_WEB_FEATURES  
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
#include "globals.h"

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
RelayStateManager relayStateManager(RELAY_PINS, RELAY_COUNT);


void setup() {
  delay(500);
  Serial.begin(115200);
  sendFormattedLog("--- Démarrage ESP32 ---");
  setupRelays();
  wifiManager.begin();
  for (int i = 0; i < 8; i++) {
  delay(100);
  yield();
  }
  #ifdef ENABLE_WEB_FEATURES
    relayManager.begin();
    delay(100); 
    webHandler.setupRoutes();
    TimeManager::setupTime();
    if (!SPIFFS.begin(true)) sendFormattedLog("❌ SPIFFS mount failed");
    scheduler.loadSchedules();
    scheduler.syncRelaysWithCurrentTime();
    delay(100); 
  #endif
  relayStateManager.begin();
  mqttManager.begin();
  delay(500); 
  mqttManager.reconnect();
  ElegantOTA.begin(&server);  
  server.begin();

  esp_reset_reason_t reason = esp_reset_reason();
  sendFormattedLog("Fin du démarrage : Boot reason code: %d", reason);
  yield();
}

void loop() {

  esp_task_wdt_reset();  
  if (!mqttManager.connected()) mqttManager.reconnect();
  mqttManager.loop();
  yield();
  mqttManager.processQueue();
  ElegantOTA.loop();
  yield();
  #ifdef ENABLE_WEB_FEATURES
   scheduler.processUpdate();
  #endif
  static unsigned long lastLogTime = 0;
    if (millis() - lastLogTime > 3000) {
      processLogQueue();  
      lastLogTime = millis();
      yield();
    }
}

