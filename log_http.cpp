#include "log_http.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char* serverName = "http://192.168.0.145/post-esp-data.php";  
String apiKeyValue = "tPmAT5Ab3j7F9";                              

std::queue<String> logQueue;

void enqueueLog(const String& message) {
  if (logQueue.size() < 20) { // limite pour éviter les débordements mémoire
    logQueue.push(message);
  } else {
    Serial.println("⚠️ File de logs pleine, log ignoré.");
  }
}

void processLogQueue() {
  if (WiFi.status() == WL_CONNECTED && !logQueue.empty()) {
    String msg = logQueue.front();
    logQueue.pop();
    sendLogHttp(msg.c_str());
  }
}


void sendLogHttp(const char* message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "api_key=" + apiKeyValue +
                      "&location=ESP32" +                
                      "&value=" + message;

    int httpResponseCode = http.POST(postData);
    if (httpResponseCode > 0) {
      Serial.printf("✅ Log envoyé: %d\n", httpResponseCode);
    } else {
      Serial.printf("❌ Erreur HTTP: %d\n", httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("⚠️ WiFi non connecté, log non envoyé.");
  }
}

void sendFormattedLog(const char* format, ...) {
    char logBuffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(logBuffer, sizeof(logBuffer), format, args);
    va_end(args);
    // sendLogHttp(logBuffer);  // Ta fonction d'envoi HTTP
    enqueueLog(String(logBuffer)); // ➕ Ajout à la file
}