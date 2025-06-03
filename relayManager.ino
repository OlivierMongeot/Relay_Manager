// #define ENABLE_WEB_FEATURES  // ⛔️ Commente cette ligne pour désactiver les trucs lourds
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
#endif

#ifdef ENABLE_WEB_FEATURES
RelayScheduler scheduler(RELAY_PINS, RELAY_COUNT);
RelayNameManager relayManager; 
WebHandler webHandler(server, scheduler, relayManager, RELAY_PINS, RELAY_COUNT); 
#endif

WifiManager wifiManager(15);  // timeout 15s
MQTTManager mqttManager(MQTT_SERVER_IP);


#ifdef ENABLE_WEB_FEATURES
bool isAuthenticated(AsyncWebServerRequest *request) {
  if (!request->authenticate(HTTP_USERNAME, HTTP_PASSWORD)) {
    request->requestAuthentication();
    return false;
  }
  return true;
}
#endif

void setup() {
  delay(100);
  Serial.begin(115200);
  setupRelays();
  wifiManager.begin();
  mqttManager.begin();
  mqttManager.reconnect();
  
  #ifdef ENABLE_WEB_FEATURES
  relayManager.begin();  
  webHandler.setupRoutes();
  TimeManager::setupTime();
  scheduler.loadSchedules();
  scheduler.syncRelaysWithCurrentTime();  // ✅ force la cohérence
  #endif

  ElegantOTA.begin(&server);  
  yield();
  server.begin();
  // sendLogHttp("Système démarré");
  // Serial.print("Free heap: ");
  // Serial.println(ESP.getFreeHeap());
  //  sendFormattedLog("Système démarréFree heap:" , Relais %d ON", s.relayIndex + 1);
  sendFormattedLog("Système démarré, Free heap: %d", ESP.getFreeHeap());

}

void loop() {

  esp_task_wdt_reset();  

  if (!mqttManager.connected()) mqttManager.reconnect();
  mqttManager.loop();
  ElegantOTA.loop();

  #ifdef ENABLE_WEB_FEATURES
  static unsigned long lastUpdate = 0;
  const unsigned long interval = 20000; // 20 secondes 
  unsigned long now = millis();
  if (now - lastUpdate >= interval) {
      lastUpdate = now;
      scheduler.update();
    }
  #endif
}