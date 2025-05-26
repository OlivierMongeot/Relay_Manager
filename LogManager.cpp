#include "LogManager.h"

String LogManager::_buffer;

void LogManager::log(const String &message) {
  Serial.println(message);
  _buffer += message + "\n";       // ✅ Stockage en RAM
  if (_buffer.length() > 5000) {   // ✅ Limite de sécurité (~5 KB)
    _buffer.remove(0, 1000);       // Supprime les anciens logs
  }
}

void LogManager::logf(const char* format, ...) {
  char buf[256];
  va_list args;
  va_start(args, format);
  vsnprintf(buf, sizeof(buf), format, args);
  va_end(args);
  log(String(buf));
}

String LogManager::getLogs() {
  return _buffer;
}

void LogManager::clearLogs() {
  _buffer = "";
}