#include "RelayScheduler.h"
#include <time.h>
#include "mqtt_manager.h"
#include "log_http.h"


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
            sendFormattedLog("Scheduler : Relais %d ON", s.relayIndex + 1);
          
        } else if (hour == s.hourOff && minute == s.minOff && s.isOn) {
            digitalWrite(pin, HIGH); // OFF
            s.isOn = false;
            sendFormattedLog("Scheduler : Relais %d OFF", s.relayIndex + 1);
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


void RelayScheduler::syncRelaysWithCurrentTime() {

    sendLogHttp("Sync Relays WithCurrentTime");

    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    int hour = timeinfo.tm_hour;
    int minute = timeinfo.tm_min;

    sendFormattedLog("[SYNC] Il y a %d plannings enregistrés\n", _schedules.size());

    for (auto& s : _schedules) {

        int pin = _relayPins[s.relayIndex];

        bool shouldBeOn = false;

        // Cas général : même jour (heure on < heure off)
        if ((s.hourOn < s.hourOff) || (s.hourOn == s.hourOff && s.minOn < s.minOff)) {
        shouldBeOn = (hour > s.hourOn || (hour == s.hourOn && minute >= s.minOn)) &&
                    (hour < s.hourOff || (hour == s.hourOff && minute < s.minOff));
        } 
        // Cas où le OFF est le lendemain (ex: ON 22:00 OFF 06:00)
        else {
        shouldBeOn = (hour > s.hourOn || (hour == s.hourOn && minute >= s.minOn)) ||
                    (hour < s.hourOff || (hour == s.hourOff && minute < s.minOff));
        }

        digitalWrite(pin, shouldBeOn ? LOW : HIGH);
        s.isOn = shouldBeOn;

        sendFormattedLog("Relais %d %s à %02d:%02d\n", s.relayIndex + 1, shouldBeOn ? "restauré ON" : "restauré OFF", hour, minute);
        yield();
    }
}


