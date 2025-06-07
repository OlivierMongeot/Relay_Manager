#ifndef RELAY_STATE_MANAGER_H
#define RELAY_STATE_MANAGER_H

#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

class RelayStateManager {
public:
    RelayStateManager(const int* pins, int count); 
    void begin();
    void set(int index, bool on);
    bool get(int index) const;
    void applyStates();
    void save();
    void load();

private:
    const int* _relayPins;
    int _relayCount;
    bool _states[16]; 
};

#endif