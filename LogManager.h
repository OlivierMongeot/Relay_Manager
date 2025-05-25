#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <Arduino.h>

class LogManager {
public:
    static void begin();                         // Initialise LittleFS
    static void log(const String& message);      // Ajoute un log
    static String readLogs();                    // Lit tout le fichier de log
    static void logf(const char* fmt, ...);   
};

#endif
