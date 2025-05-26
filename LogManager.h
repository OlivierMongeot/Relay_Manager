#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include "mqtt_manager.h"

class LogManager {
public:
    static void log(const String& msg);
    static void logf(const char* format, ...);
    static String getLogs(); // Pour affichage via Web

private:
    static void addToBuffer(const String& msg);
    static constexpr size_t MAX_LOGS = 50;
    static String logBuffer[MAX_LOGS];
    static size_t logIndex;
};

#endif
