#include <EEPROM.h>


void initStorage() {
  EEPROM.begin(512);
}

void storePresets() {
  EEPROM.put<byte>(0, presetSpeed);
  EEPROM.put<int>(4, presetTimeSec);
  EEPROM.put<byte>(8, presetDelaySec);
  EEPROM.put<byte>(12, autostart);

  EEPROM.put<byte>(16, allowSpeedOptimizer);
  EEPROM.put<byte>(20, maxSpeedUp);
  EEPROM.put<byte>(24, halfSpeedUp);
  EEPROM.put<byte>(28, littleSpeedUp);
  EEPROM.put<byte>(32, speedChangeStep);
  EEPROM.put<byte>(36, optimalMinG);

  EEPROM.commit();
  Serial.println("Defaults were set");
}

void readPresets() {
  EEPROM.get<byte>(0, presetSpeed);
  EEPROM.get<int>(4, presetTimeSec);
  EEPROM.get<byte>(8, presetDelaySec);
  EEPROM.get<byte>(12, autostart);

  EEPROM.get<byte>(16, allowSpeedOptimizer);
  EEPROM.get<byte>(20, maxSpeedUp);
  EEPROM.get<byte>(24, halfSpeedUp);
  EEPROM.get<byte>(28, littleSpeedUp);
  EEPROM.get<byte>(32, speedChangeStep);
  EEPROM.get<byte>(36, optimalMinG);

  Serial.println("Defaults were loaded");
}
