#include "RelayNames.h"

RelayNameManager::RelayNameManager() {}

void RelayNameManager::begin() {
  preferences.begin("relay-names", false);
  loadAll();
}

void RelayNameManager::loadAll() {
  for (int i = 0; i < 16; i++) {
    char key[8];
    snprintf(key, sizeof(key), "relay%d", i);

    char defaultName[16];
    snprintf(defaultName, sizeof(defaultName), "Relais %d", i + 1);

    names[i] = preferences.getString(key, defaultName);
  }
}

void RelayNameManager::saveAll() {
  for (int i = 0; i < 16; i++) {
    char key[8];
    snprintf(key, sizeof(key), "relay%d", i);
    preferences.putString(key, names[i]);
  }
}

String RelayNameManager::getName(int index) {
  if (index < 0 || index >= 16) {
    return "";
  }
  return names[index];
}

void RelayNameManager::setName(int index, const String& name) {
  if (index < 0 || index >= 16) {
    return;
  }

  char key[8];
  snprintf(key, sizeof(key), "relay%d", index);

  names[index] = name;
  preferences.putString(key, name);
}
