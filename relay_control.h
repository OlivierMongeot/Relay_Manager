#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <Arduino.h>

// Tableau des broches GPIO de l'ESP branchés sue les relais
extern const int RELAY_PINS[16];
// Nombre total de relais
#define RELAY_COUNT 16
// Fonction d'initialisation des relais
void setupRelays();

#endif