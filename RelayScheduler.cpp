#include "RelayScheduler.h"
#include <time.h>
// #include "WsManager.h"

RelayScheduler::RelayScheduler(const int* relayPins, int relayCount)
    : _relayPins(relayPins), _relayCount(relayCount) {}


void RelayScheduler::addRelay(uint8_t relayIndex, uint8_t hourOn, uint8_t minOn, uint8_t hourOff, uint8_t minOff) {
    RelaySchedule schedule = { relayIndex, hourOn, minOn, hourOff, minOff, false };
    _schedules.push_back(schedule);
   saveSchedules();
}

void RelayScheduler::update() {
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    int hour = timeinfo.tm_hour;
    int minute = timeinfo.tm_min;

    for (auto& s : _schedules) {
        int pin = _relayPins[s.relayIndex];
        if (hour == s.hourOn && minute == s.minOn && !s.isOn) {
            digitalWrite(pin, LOW); // ON
            s.isOn = true;
            Serial.printf("Relais %d ON à %02d:%02d\n", s.relayIndex + 1, hour, minute);
            // WsManager::notifyAllClientsRelayStates();
        } else if (hour == s.hourOff && minute == s.minOff && s.isOn) {
            digitalWrite(pin, HIGH); // OFF
            s.isOn = false;
            Serial.printf("Relais %d OFF à %02d:%02d\n", s.relayIndex + 1, hour, minute);
            // WsManager::notifyAllClientsRelayStates();
        }
    }
}

void RelayScheduler::removeSchedule(int index) {
    if (index >= 0 && index < _schedules.size()) {
        _schedules.erase(_schedules.begin() + index);
        saveSchedules();
    }
}

const std::vector<RelaySchedule>& RelayScheduler::getSchedules() const {
    return _schedules;
}

void RelayScheduler::saveSchedules() {
    StaticJsonDocument<2048> doc;
    JsonArray array = doc.to<JsonArray>();

    for (const auto& s : _schedules) {
        JsonObject obj = array.createNestedObject();
        obj["relayIndex"] = s.relayIndex;
        obj["hourOn"] = s.hourOn;
        obj["minOn"] = s.minOn;
        obj["hourOff"] = s.hourOff;
        obj["minOff"] = s.minOff;
    }

    File file = SPIFFS.open("/schedules.json", FILE_WRITE);
    if (file) {
        serializeJson(doc, file);
        file.close();
    }
}

void RelayScheduler::loadSchedules() {
    File file = SPIFFS.open("/schedules.json", FILE_READ);
    if (!file) return;

    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        file.close();
        return;
    }

    _schedules.clear();
    for (JsonObject obj : doc.as<JsonArray>()) {
        RelaySchedule s;
        s.relayIndex = obj["relayIndex"];
        s.hourOn = obj["hourOn"];
        s.minOn = obj["minOn"];
        s.hourOff = obj["hourOff"];
        s.minOff = obj["minOff"];
        s.isOn = false;
        _schedules.push_back(s);
    }

    file.close();
}   

