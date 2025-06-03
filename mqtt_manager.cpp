#include "mqtt_manager.h"
#include <Arduino.h>
#include <cstdarg>  // pour va_list
#include <cstdio>   // pour vsnprintf
#include "log_http.h" 
// Externe
extern const int RELAY_PINS[16];

MQTTManager* MQTTManager::instance = nullptr;

MQTTManager::MQTTManager(const char* server, uint16_t port)
  : _server(server), _port(port), _client(_espClient) {
  instance = this;
}

void MQTTManager::begin() {
  _client.setServer(_server, _port);
  _client.setCallback(mqttCallback);  // Utilisation du callback encapsulé
}

void MQTTManager::mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (instance) {
    String msg;
    for (unsigned int i = 0; i < length; i++) {
      msg += (char)payload[i];
    }
    instance->handleMessage(String(topic), msg);
  }
}

void MQTTManager::handleMessage(const String& topic, const String& message) {
  Serial.print("MQTT -> Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);

  if (message == "TEST") {
    for (int i = 0; i < 16; i++) {
      digitalWrite(RELAY_PINS[i], LOW);
      delay(300);
    }
    for (int i = 0; i < 16; i++) {
      digitalWrite(RELAY_PINS[i], HIGH);
      delay(300);
    }
    return;
  }


  if (message == "TEST2") {
    for (int i = 0; i < 15; i+=2) {
      digitalWrite(RELAY_PINS[i], LOW);
      digitalWrite(RELAY_PINS[i+1], LOW);
      delay(300);
    }
    for (int i = 0; i < 15; i+=2) {
      digitalWrite(RELAY_PINS[i], HIGH);
      digitalWrite(RELAY_PINS[i+1], HIGH);
      delay(300);
    }
    return;
  }

  for (int i = 0; i < 16; i++) {
    if (message == "ON" + String(i + 1)) {
      digitalWrite(RELAY_PINS[i], LOW);
      sendFormattedLog("Command MQTT : Relais %d ON", i + 1 );
      return;
    }
    if (message == "OFF" + String(i + 1)) {
      digitalWrite(RELAY_PINS[i], HIGH);
      sendFormattedLog("Command MQTT : Relais %d OFF", i + 1 );
      return;
    }
  }

}


void MQTTManager::reconnect() {
  while (!_client.connected()) {
    Serial.print("Connexion à MQTT...");
    if (_client.connect("ESP32Client")) {
      Serial.println(" ✅ Connecté !");
      sendLogHttp( "Connecté en MQTT");
      _client.subscribe(MQTT_TOPIC);
    } else {
      Serial.print(" ❌ Échec, code : ");
      sendLogHttp( "Échec connexion MQTT");
      Serial.print(_client.state());
      Serial.println(" -> Nouvelle tentative dans 10s");
      delay(10000);
    }
  }
}

bool MQTTManager::connected() {
  return _client.connected();
}

void MQTTManager::loop() {
  _client.loop();
}

void MQTTManager::publish(const char* topic, const char* payload) {
  _client.publish(topic, payload);
}

void MQTTManager::subscribe(const char* topic) {
  _client.subscribe(topic);
}
