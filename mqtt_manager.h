#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFiClient.h>
#include <PubSubClient.h>

#define MQTT_TOPIC "jardin/relay16"

class MQTTManager {
public:
  MQTTManager(const char* server, uint16_t port = 1883);
  void begin();
  void reconnect();
  bool connected();
  void loop();
  void publish(const char* topic, const char* payload);
  void subscribe(const char* topic);
  static MQTTManager* instance; // ✅ pour accéder à l’objet dans le callback statique

private:
  const char* _server;
  uint16_t _port;
  WiFiClient _espClient;
  PubSubClient _client;
  void handleMessage(const String& topic, const String& message);  // ✅ logique encapsulée ici
  static void mqttCallback(char* topic, byte* payload, unsigned int length); // ✅ appelé par PubSubClient
};

#endif
