#include "relay_control.h"


// GPIO à éviter
// 0	Mode boot → ne pas utiliser
// 2	Utilisé pour le boot + LED parfois
// 12	Si HIGH au boot → mode flash 1.8V → brick possible
// 15	Nécessite un niveau bas au boot
// 6–11	Utilisés pour le flash SPI → ⚠️ ne pas toucher
// 34–39	En lecture seule (input only) → pas utilisables en output

// Rangée de gauche : 13, 14, 27
// Rangée de droite : 4, 16, 17, 23, 22

const int RELAY_PINS[RELAY_COUNT] = {
  13, 22, 14, 27,
  26, 25, 33, 32,
  21, 5, 18, 19,
  4, 16, 17, 23
};

void setupRelays() {
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(RELAY_PINS[i], OUTPUT_OPEN_DRAIN);
    digitalWrite(RELAY_PINS[i], HIGH);
     
  }
}