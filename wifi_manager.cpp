#include "wifi_manager.h"
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include "mqtt_manager.h"


Preferences preferences;
AsyncWebServer server(80);

const char* ap_ssid = "Relay_Manager_Config";
const char* ap_password = "12345678";

WifiManager::WifiManager(int timeoutSeconds) 
  : _timeoutSeconds(timeoutSeconds) {}

void WifiManager::begin() {
  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  preferences.end();

  if (ssid == "" || password == "") {
    Serial.println("Aucune configuration WiFi trouvée. Démarrage du portail AP...");
    startConfigPortal();
  } else {
    connectToWiFi(ssid.c_str(), password.c_str());
  }
}

void WifiManager::connectToWiFi(const char* ssid, const char* password) {
  int status = WL_IDLE_STATUS;
  delay(100);
  Serial.print("\nConnexion WiFi ");
  Serial.print("\nStatut : ");
  Serial.println(getWifiStatus(status));
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int timeout_counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    timeout_counter++;
    if (timeout_counter >= _timeoutSeconds * 2) { // 2 x 500ms = 1s
      Serial.println("\nÉchec de connexion WiFi. Lancement du portail AP...");
      startConfigPortal();
      return; // ne pas continuer
    }
  }

  Serial.println("\nWiFi connecté !");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void WifiManager::startConfigPortal() {

  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("AP lancé : connectez-vous à " + String(ap_ssid));
  IPAddress IP = WiFi.softAPIP();
  Serial.println("IP : " + IP.toString());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html",
      "<h2>Configuration WiFi Relay Manager</h2>"
      "<form action=\"/save\" method=\"GET\">"
      "SSID: <input name=\"ssid\"><br>"
      "Password: <input name=\"password\" type=\"password\"><br>"
      "<input type=\"submit\" value=\"Sauvegarder\"></form>");
  });

  server.on("/save", [](AsyncWebServerRequest *request){
    String ssid = request->hasParam("ssid") ? request->getParam("ssid")->value() : "";
    String password = request->hasParam("password") ? request->getParam("password")->value() : "";

    if(ssid.length() > 0){
      preferences.begin("wifi", false);
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.end();

      request->send(200, "text/html", "<p>Sauvegardé. Redémarrage...</p>");
      delay(2000);
      ESP.restart();
    } else {
      request->send(200, "text/html", "<p>SSID invalide, veuillez réessayer.</p>");
    }
  });

  server.begin();
  // Boucle infinie pour rester en mode config jusqu'à ce que l'ESP redémarre
  while (true) {
    delay(100);
  }
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

