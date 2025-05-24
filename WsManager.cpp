#include "WsManager.h"
#include <Arduino.h>
#include "relay_control.h"
#include <AsyncWebSocket.h>
#include <LogManager.h>

// Déclare le ws en externe (si défini dans un autre fichier)
extern AsyncWebSocket ws;

void WsManager::notifyAllClientsRelayStates() {
  // Utiliser un buffer statique au lieu d'un String dynamique
  constexpr size_t BUFFER_SIZE = 1024;
  char json[BUFFER_SIZE];
  size_t offset = 0;

  offset += snprintf(json + offset, BUFFER_SIZE - offset, "{\"relays\":[");
  
  for (int i = 0; i < RELAY_COUNT; i++) {
    bool isOn = digitalRead(RELAY_PINS[i]) == LOW;
    offset += snprintf(json + offset, BUFFER_SIZE - offset,
                       "{\"id\":%d,\"state\":%s}%s",
                       i + 1,
                       isOn ? "true" : "false",
                       (i < RELAY_COUNT - 1) ? "," : "");
  }

  offset += snprintf(json + offset, BUFFER_SIZE - offset, "]}");

  // Sécurité : ne pas envoyer si aucun client ou JSON mal formé
  if (ws.count() > 0 && offset < BUFFER_SIZE) {
    ws.textAll(json);
    // Serial.println("[WS] États des relais envoyés à tous les clients.");
    LogManager::log("[WS] États des relais envoyés à tous les clients.");
   
  } else {
    // Serial.println("[WS] Aucun client ou erreur JSON, rien envoyé.");
    LogManager::log("[WS] Aucun client ou erreur JSON, rien envoyé.");
  }

  // Serial.print("Free Heap après WebSocket : ");
  // Serial.println(ESP.getFreeHeap());
  LogManager::log(String("FreeHeap after WS Notiftcation all clients : ") + String(ESP.getFreeHeap()));
}

void WsManager::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                          AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT: {
        LogManager::logf("[WS] Tentative de connexion du client %u depuis %s\n",
                    client->id(), client->remoteIP().toString().c_str());

    uint8_t connected = 0;
    for (auto &c : ws.getClients()) {
      if (c.status() == WS_CONNECTED) {
        connected++;
      }
    }
    LogManager::logf("[WS] Nombre actuel de clients connectés : %u", connected);

    if (connected >= 4) {
      LogManager::log("[WS] Trop de clients connectés, connexion refusée.");
      client->close();
      return;
    }

    LogManager::logf("[WS] Client %u connecté avec succès", client->id());
    notifyAllClientsRelayStates();
    break;
    }

    case WS_EVT_DISCONNECT:
      LogManager::logf("[WS] Client %u déconnecté\n", client->id());
      break;

    case WS_EVT_ERROR:
      LogManager::logf("[WS] Erreur WebSocket avec client %u\n", client->id());
      break;

    case WS_EVT_DATA:
      LogManager::logf("[WS] Messages entrants avec client %u\n", client->id());
      break;
  }
}
