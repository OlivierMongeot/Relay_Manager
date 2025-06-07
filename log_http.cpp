#include "log_http.h"
#include <WiFi.h>
#include <HTTPClient.h>

const char* serverName = "http://192.168.0.145/post-esp-data.php";  
String apiKeyValue = "tPmAT5Ab3j7F9";                              


const int LOG_QUEUE_SIZE = 20;
const int LOG_MSG_SIZE = 128;

char logBuffer[LOG_QUEUE_SIZE][LOG_MSG_SIZE];
int head = 0;
int tail = 0;
bool isFull = false;

bool isLogQueueEmpty() {
  return (!isFull && head == tail);
}

bool isLogQueueFull() {
  return isFull;
}

void enqueueLog(const char* message) {
  if (isLogQueueFull()) {
    // Optionnel : tu peux garder une trace en Serial, ou compter les logs perdus
    Serial.println(F("⚠️ File circulaire pleine, log ignoré."));
    return;
  }
  // Copie sécurisée de la chaîne dans le buffer circulaire
  strncpy(logBuffer[tail], message, LOG_MSG_SIZE - 1);
  logBuffer[tail][LOG_MSG_SIZE - 1] = '\0'; // assure la terminaison
  tail = (tail + 1) % LOG_QUEUE_SIZE;
  if (tail == head) {
    isFull = true;
  }
}

void processLogQueue() {
  if (WiFi.status() != WL_CONNECTED || isLogQueueEmpty()) return;

  // Récupère le message à la tête
  const char* message = logBuffer[head];
  head = (head + 1) % LOG_QUEUE_SIZE;
  isFull = false;

  sendLogHttp(message);
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
    http.end();
  } 
}

void sendFormattedLog(const char* format, ...) {
  char logBufferTmp[LOG_MSG_SIZE];
  va_list args;
  va_start(args, format);
  vsnprintf(logBufferTmp, sizeof(logBufferTmp), format, args);
  va_end(args);
  enqueueLog(logBufferTmp);
}
