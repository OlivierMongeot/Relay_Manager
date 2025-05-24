#ifndef RELAY_NAMES_H
#define RELAY_NAMES_H

#include <Arduino.h>
#include <Preferences.h>

class RelayNameManager {
  public:
    RelayNameManager();
    void begin();
    String getName(int index);
    void setName(int index, const String& name);
    void loadAll();
    void saveAll();
    String names[16];

  private:
    Preferences preferences;
};

#endif