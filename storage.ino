#include <EEPROM.h>


void initStorage() {
  EEPROM.begin(512);
}

void storePresets() {
  EEPROM.put<byte>(0, presetSpeed);
  EEPROM.put<int>(4, presetTimeSec);
  EEPROM.put<byte>(8, presetDelaySec);
  EEPROM.put<byte>(12, autostart);
  EEPROM.put<byte>(13, allowSpeedOptimizer);
  EEPROM.put<byte>(14, speedChangeStep);
  EEPROM.put<byte>(15, minSpeed);
  EEPROM.put<byte>(16, maxSpeed);
  EEPROM.put<byte>(17, optimalG);
  EEPROM.put<byte>(18, threshold);
  EEPROM.commit();
  Serial.println("Defaults were set");
}

void readPresets() {
  //presetSpeed = 95;
  //presetTimeSec = 300;
  //presetDelaySec = 10;
  //autostart = 0;
  EEPROM.get<byte>(0, presetSpeed);
  EEPROM.get<int>(4, presetTimeSec);
  EEPROM.get<byte>(8, presetDelaySec);
  EEPROM.get<byte>(12, autostart);
  EEPROM.get<byte>(13, allowSpeedOptimizer);
  EEPROM.get<byte>(14, speedChangeStep);
  EEPROM.get<byte>(15, minSpeed);
  EEPROM.get<byte>(16, maxSpeed);
  EEPROM.get<byte>(17, optimalG);
  EEPROM.get<byte>(18, threshold);
  Serial.println("Defaults were loaded");
}
