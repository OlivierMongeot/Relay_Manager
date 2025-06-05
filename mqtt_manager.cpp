#include "mqtt_manager.h"
#include <Arduino.h>
#include <cstdarg> 
#include <cstdio>   
#include "log_http.h" 
#include "esp_task_wdt.h"
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

void MQTTManager::processQueue() {
  if (!commandQueue.empty()) {
    Command cmd = commandQueue.front();
    commandQueue.pop();  // ✅ On dépile

    handleMessage(cmd.topic, cmd.message);
  }
}



void MQTTManager::mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (instance) {
    String msg;
    for (unsigned int i = 0; i < length; i++) {
      msg += (char)payload[i];
    }
    Command cmd = { String(topic), msg };
    instance->commandQueue.push(cmd);  // ✅ On empile
    // instance->handleMessage(String(topic), msg);
  }
}

void MQTTManager::handleMessage(const String& topic, const String& message) {
  // Serial.print("MQTT -> Topic: ");
  // Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);


  if (message == "TEST2") {
    for (int i = 0; i < 15; i+=2) {
      digitalWrite(RELAY_PINS[i], LOW);
      digitalWrite(RELAY_PINS[i+1], LOW);
      esp_task_wdt_reset();  
      delay(130);
    }
    for (int i = 0; i < 15; i+=2) {
      digitalWrite(RELAY_PINS[i], HIGH);
      digitalWrite(RELAY_PINS[i+1], HIGH);
      esp_task_wdt_reset(); 
      delay(130);
    }
    return;
  }

  for (int i = 0; i < 16; i++) {
    if (message == "ON" + String(i + 1)) {
      digitalWrite(RELAY_PINS[i], LOW);
      // sendFormattedLog("Command MQTT : Relais %d ON", i + 1 );
      return;
    }
    if (message == "OFF" + String(i + 1)) {
      digitalWrite(RELAY_PINS[i], HIGH);
      // sendFormattedLog("Command MQTT : Relais %d OFF", i + 1 );
      return;
    }
  }

}

unsigned long lastAttempt = 0;

void MQTTManager::reconnect() {
  if (!_client.connected()) {
    unsigned long now = millis();
    if (now - lastAttempt > 10000) {
      lastAttempt = now;
      Serial.print("Connexion à MQTT...");
      if (_client.connect("ESP32Client")) {
        Serial.println(" ✅ Connecté !");
        _client.subscribe(MQTT_TOPIC);
        sendFormattedLog("Connecté en MQTT");
      } else {
        Serial.print(" ❌ Échec, code : ");
        Serial.println(_client.state());
        sendFormattedLog("Échec connexion MQTT");
      }
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
