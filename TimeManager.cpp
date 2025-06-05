#include "TimeManager.h"

void TimeManager::setupTime() {
    // NTP config
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1); // Europe/Paris
    tzset();

    // Attente que l'heure soit synchronisée
    Serial.println("Synchronisation avec le serveur NTP");
    int retry = 0;
    while (time(nullptr) < 100000 && retry < 10) {
        delay(500);
        Serial.print(".");
        retry++;
    }
    delay(50);
    Serial.println("\nHeure synchronisée !");

    // Affiche heure locale
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    delay(50);

    char buffer[30];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    delay(50);
    Serial.println(buffer);
}
