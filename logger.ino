#include <LittleFS.h>
File logFile;
const String logName = "/flight.log";

void setupLogger() {
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
}
String getFsInfo() {
  FSInfo fsinfo;
  LittleFS.info(fsinfo);
  return String(fsinfo.usedBytes) + "/" + String(fsinfo.totalBytes);
}
void log(String data) {
  logFile.println(data);
}

void endLog() {
  logFile.close();
}

void prepareLog() {
  if (LittleFS.exists(logName)) {
    LittleFS.remove(logName);
  }
  logFile = LittleFS.open(logName, "a");
  logFile.println("time;throttle;gyrY;Gy;Gz;G total");
}