#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WifiManager {
public:
  WifiManager(const char* ssid, const char* password, int timeoutSeconds = 30);
  void begin();
  String getWifiStatus(int status);

private:
  const char* _ssid;
  const char* _password;
  int _timeoutSeconds;
};

#endif
