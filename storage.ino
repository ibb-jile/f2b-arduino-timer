#include <EEPROM.h>

int address = 0;
byte value;

void initStorage() {
  EEPROM.begin(512);
}

void storePresets() {
  EEPROM.write(0, presetSpeed);
  EEPROM.write(4, presetTimeSec);
  EEPROM.write(8, presetDelaySec);
  EEPROM.write(12, autostart);
  EEPROM.commit();
  Serial.println("Defaults were set");
}

void readPresets() {
  presetSpeed = EEPROM.read(0);
  presetTimeSec = EEPROM.read(4);
  presetDelaySec = EEPROM.read(8);
  autostart = EEPROM.read(12);
  Serial.println("Defaults were loaded");
}
