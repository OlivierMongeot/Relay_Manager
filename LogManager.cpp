#include "LogManager.h"
#include <FS.h>
#include <SPIFFS.h>
#include <time.h>  // Si tu veux ajouter un horodatage (optionnel)
#include <stdarg.h>
// #include "mqtt_manager.h"

#define LOG_FILE_PATH "/logs.txt"
#define MAX_LOG_SIZE 8192  // Limite pour éviter de saturer

void LogManager::begin() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Erreur lors du montage SPIFFS");
    } else {
        Serial.println("SPIFFS monté avec succès");
    }
}

void LogManager::log(const String& message) {
    File file = SPIFFS.open(LOG_FILE_PATH, FILE_APPEND);
    if (!file) {
        Serial.println("Erreur d'ouverture du fichier log");
        return;
    }

    // Horodatage simple (si tu as configuré NTP sinon commentaire)
    time_t now = time(nullptr);
    String timestamp = now > 0 ? String(ctime(&now)) : "[NO TIME]";
    timestamp.trim();

    file.println("[" + timestamp + "] " + message);
    file.close();
    Serial.println("[LogManager] " + message);

   
}


// void LogManager::clear() {
//     SPIFFS.remove(LOG_FILE_PATH);
// }

void LogManager::logf(const char* fmt, ...) {
  char buffer[128]; // Ajuste si nécessaire
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  log(String(buffer)); // Appelle ta méthode log(String)
}
