#include <EEPROM.h>


void initStorage() {
  EEPROM.begin(512);
}

void storePresets() {
  EEPROM.put<byte>(0, presetSpeed);
  
  EEPROM.write(4, highByte(presetTimeSec ));
  EEPROM.write(5, lowByte(presetTimeSec ));
  EEPROM.put<int>(4, presetTimeSec);
  EEPROM.put<byte>(8, presetDelaySec);
  EEPROM.put<byte>(12, autostart);
  EEPROM.commit();
  Serial.println("Defaults were set");
}

void readPresets() {
  EEPROM.get<byte>(0, presetSpeed);
  EEPROM.get<int>(4, presetTimeSec);
  EEPROM.get<byte>(8, presetDelaySec);
  EEPROM.get<byte>(12, autostart);
  Serial.println("Defaults were loaded");
}
