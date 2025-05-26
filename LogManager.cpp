#include "LogManager.h"

String LogManager::logBuffer[LogManager::MAX_LOGS];
size_t LogManager::logIndex = 0;

void LogManager::log(const String& msg) {
    Serial.println(msg);
    addToBuffer(msg);

    if (MQTTManager::instance && MQTTManager::instance->connected()) {
        MQTTManager::instance->publish("maison/system/logs", msg.c_str());
    }
}

void LogManager::logf(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    log(String(buffer));
}

void LogManager::addToBuffer(const String& msg) {
    logBuffer[logIndex] = msg;
    logIndex = (logIndex + 1) % MAX_LOGS;
}

String LogManager::getLogs() {
    String result;
    for (size_t i = 0; i < MAX_LOGS; ++i) {
        size_t idx = (logIndex + i) % MAX_LOGS;
        if (logBuffer[idx].length() > 0) {
            result += logBuffer[idx] + "\n";
        }
    }
    return result;
}