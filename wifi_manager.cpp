#include "wifi_manager.h"

WifiManager::WifiManager(const char* ssid, const char* password, int timeoutSeconds)
  : _ssid(ssid), _password(password), _timeoutSeconds(timeoutSeconds) {}

void WifiManager::begin() {
  int status = WL_IDLE_STATUS;
  delay(700);
  Serial.print("\nConnexion WiFi ");
  Serial.print("\nStatut : ");
  Serial.println(getWifiStatus(status));
  WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid, _password);

  int timeout_counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    timeout_counter++;
    if (timeout_counter >= _timeoutSeconds * 2) { // 2 x 500ms = 1s
      ESP.restart();
    }
  }

  Serial.println("\nWiFi connecté !");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

String WifiManager::getWifiStatus(int status) {
  switch (status) {
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
    default: return "UNKNOWN";
  }
}
