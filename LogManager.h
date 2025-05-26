#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <Arduino.h>

class LogManager {
  public:
    static void log(const String &message);
    static void logf(const char* format, ...);
    static String getLogs();     // ✅ Pour exposer les logs via HTTP
    static void clearLogs();     // ✅ Pour les effacer si besoin

  private:
    static String _buffer;       // ✅ Stocke les logs en RAM
};

#endif