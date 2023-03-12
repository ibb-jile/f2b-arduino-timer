#include <EEPROM.h>


void initStorage() {
  EEPROM.begin(512);
}

void storePresets() {
  EEPROM.put<int>(0, presetTimeSec);
  EEPROM.put<byte>(4, presetSpeed);
  EEPROM.put<byte>(5, presetDelaySec);
  EEPROM.put<byte>(6, autostart);

  EEPROM.put<byte>(7, allowSpeedOptimizer);
  EEPROM.put<byte>(8, maxSpeedUp);
  EEPROM.put<byte>(9, halfSpeedUp);
  EEPROM.put<byte>(10, littleSpeedUp);
  EEPROM.put<byte>(11, speedChangeStep);
  EEPROM.put<byte>(12, optimalMinG);

  EEPROM.commit();
  Serial.println("Defaults were set");
}

void readPresets() {
  EEPROM.get<int>(0, presetTimeSec);
  EEPROM.get<byte>(4, presetSpeed);
  EEPROM.get<byte>(5, presetDelaySec);
  EEPROM.get<byte>(6, autostart);

  EEPROM.get<byte>(7, allowSpeedOptimizer);
  EEPROM.get<byte>(8, maxSpeedUp);
  EEPROM.get<byte>(9, halfSpeedUp);
  EEPROM.get<byte>(10, littleSpeedUp);
  EEPROM.get<byte>(11, speedChangeStep);
  EEPROM.get<byte>(12, optimalMinG);

  Serial.println("Defaults were loaded");
}
