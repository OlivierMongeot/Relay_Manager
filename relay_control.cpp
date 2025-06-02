#include "relay_control.h"


const int RELAY_PINS[RELAY_COUNT] = {
  13, 12, 14, 27,
  26, 25, 33, 32,
  21, 5, 18, 19,
  4, 16, 17, 15
};

void setupRelays() {
  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(RELAY_PINS[i], OUTPUT_OPEN_DRAIN);
    digitalWrite(RELAY_PINS[i], HIGH);
     
  }
}