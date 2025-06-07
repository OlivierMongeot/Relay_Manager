#include "RelayStateManager.h"
#include "log_http.h"

RelayStateManager::RelayStateManager(const int* relayPins, int relayCount)
  : _relayPins(relayPins), _relayCount(relayCount) {
    for (int i = 0; i < _relayCount; ++i) {
        _states[i] = false;
    }
}

void RelayStateManager::begin() {
    load();
    applyStates();
}

void RelayStateManager::set(int index, bool on) {
    if (index < 0 || index >= _relayCount) {
      sendFormattedLog("❌ set: index %d out of bounds\n", index);
      return;
    }
    _states[index] = on;

    digitalWrite(_relayPins[index], on ? LOW : HIGH); // LOW = ON
    // sendFormattedLog(" set: relay[%d] - %s (pin %d)\n", index, on ? "ON" : "OFF", _relayPins[index]);
    save();
}

bool RelayStateManager::get(int index) const {
    if (index < 0 || index >= _relayCount) return false;
    return _states[index];
}

void RelayStateManager::applyStates() {
    for (int i = 0; i < _relayCount; ++i) {
        digitalWrite(_relayPins[i], _states[i] ? LOW : HIGH);
    }
}

void RelayStateManager::save() {
    StaticJsonDocument<512> doc;
    JsonArray arr = doc.to<JsonArray>();
    for (int i = 0; i < _relayCount; ++i) {
        arr.add(_states[i]);
    }

    File f = SPIFFS.open("/relay_states.json", FILE_WRITE);
    if (f) {
        serializeJson(doc, f);
        f.close();
    }
}

void RelayStateManager::load() {
    File f = SPIFFS.open("/relay_states.json", FILE_READ);
    if (!f) return;

    StaticJsonDocument<512> doc;
    DeserializationError err = deserializeJson(doc, f);
    f.close();
    if (err) return;

    JsonArray arr = doc.as<JsonArray>();
    for (int i = 0; i < _relayCount && i < arr.size(); ++i) {
        _states[i] = arr[i];
    }
}

