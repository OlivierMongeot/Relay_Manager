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
  // static MQTTManager* getInstance();    
  void publish(const char* topic, const char* payload);
  void subscribe(const char* topic);

private:
  const char* _server;
  uint16_t _port;
  WiFiClient _espClient;
  PubSubClient _client;
  static MQTTManager* instance;                   
  void handleMessage(const String& topic, const String& message); 
  static void mqttCallback(char* topic, byte* payload, unsigned int length);
};

#endif
