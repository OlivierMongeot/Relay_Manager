#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;

class WifiManager {
public:
  WifiManager(int timeoutSeconds = 15);
  void begin();

private:
  const int _timeoutSeconds;

  void startConfigPortal();
  void connectToWiFi(const char* ssid, const char* password);
  String getWifiStatus(int status);
};

#endif
