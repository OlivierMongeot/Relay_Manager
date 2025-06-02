#ifndef RELAY_SCHEDULER_H
#define RELAY_SCHEDULER_H

#include <Arduino.h>
#include <vector>
#include <SPIFFS.h>
#include <ArduinoJson.h>

struct RelaySchedule {
    uint8_t relayIndex;
    uint8_t hourOn;
    uint8_t minOn;
    uint8_t hourOff;
    uint8_t minOff;
    bool isOn;
};

class RelayScheduler {
public:
    RelayScheduler(const int* relayPins, int relayCount);
    void addRelay(uint8_t relayIndex, uint8_t hourOn, uint8_t minOn, uint8_t hourOff, uint8_t minOff);
    void update();
    void removeSchedule(int index);
    const std::vector<RelaySchedule>& getSchedules() const;
    void saveSchedules();
    void loadSchedules();
    void syncRelaysWithCurrentTime();

private:
    const int* _relayPins;
    int _relayCount;
    std::vector<RelaySchedule> _schedules;
};

#endif