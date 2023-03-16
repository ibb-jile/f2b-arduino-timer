#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <StreamString.h>
#include <ArduinoJson.h>
#include "http-body.h"
WebServer server(80);

int modePin = 14;
int mode = 0;
int state = 0;
unsigned long timerFinishAt = 0;
unsigned long startAfterDelay = 0;

int presetTimeSec = 60;
byte presetDelaySec = 15;
byte autostart = 0;
byte allowSpeedOptimizer = 1;
byte maxSpeedUp = 70;
byte halfSpeedUp = 65;
byte littleSpeedUp = 60;
byte speedChangeStep = 2;
byte optimalMinG = 23;

void setup() {
  initSerial();
  initStorage();
  Serial.println("Welcome Stunt Timer");
  readPresets();
  initHW();
  setupMotorControl();
  rpmInit();

  Serial.println("HW is ready");

  /*if (digitalRead(modePin) == LOW) {
    Serial.println("Setup range");
    setupRange();
  }*/
  blink(10, 100);
  stopMotor();
  blink(20, 80);

  setupWifi();
  setupGyro();
  server.on("/", handleRoot);
  server.on("/set", HTTP_PUT, setData);
  server.on("/get", getData);
  server.on("/timer/start", startTimerRequest);
  server.on("/timer/stop", stopTimerRequest);
  server.on("/motor/stop", stopMotorRequest);
  server.on("/motor/start", startMotorRequest);
  server.on("/speed/up", speedUpRequest);
  server.on("/speed/down", speedDownRequest);
  server.onNotFound(handleNotFound);
  startServer();
  blink(20, 50);
  digitalWrite(LED_BUILTIN, HIGH);

  if (autostart == 1) {
    delay(10000);
    startTimer();
  }
}
void handleRoot() {
  Serial.println("client connected 5");
  server.send(200, "text/html", pageBody);
}
void startTimerRequest() {
  startTimer();
  server.send(200, "application/json", "");
}
void stopTimerRequest() {
  stopTimer();
  server.send(200, "application/json", "");
}
void stopMotorRequest() {
  stopMotor();
  server.send(200, "application/json", "");
}
void startMotorRequest() {
  startMotor();
  server.send(200, "application/json", "");
}
void speedUpRequest() {
  speedUp();
  server.send(200, "application/json", "");
}
void speedDownRequest() {
  speedDown();
  server.send(200, "application/json", "");
}
void initHW() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(modePin, INPUT);
  digitalWrite(modePin, HIGH);
}
void initSerial() {
  Serial.begin(115200);
}

void setupRange() {
  blink(3, 500);
  mode = 1;
  setMotorMax();
  digitalWrite(LED_BUILTIN, HIGH);
  while (digitalRead(modePin) == LOW) {
    delay(15);
  }
  stopMotor();
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

void getData() {
  StaticJsonDocument<500> doc;

  doc["preset"]["speed"] = getPresetSpeed();
  doc["preset"]["timeSec"] = presetTimeSec;
  doc["preset"]["delaySec"] = presetDelaySec;
  doc["preset"]["autostart"] = autostart;

  doc["preset"]["optimizer"]["allowSpeedOptimizer"] = allowSpeedOptimizer;
  doc["preset"]["optimizer"]["maxSpeedUp"] = maxSpeedUp;
  doc["preset"]["optimizer"]["halfSpeedUp"] = halfSpeedUp;
  doc["preset"]["optimizer"]["littleSpeedUp"] = littleSpeedUp;
  doc["preset"]["optimizer"]["speedChangeStep"] = speedChangeStep;
  doc["preset"]["optimizer"]["optimalMinG"] = optimalMinG;

  doc["current"]["speed"] = getCurrentSpeed();
  doc["current"]["state"] = state;
  doc["current"]["finishAt"] = timerFinishAt;
  doc["current"]["rpm"] = getRpm();
  doc["current"]["time"] = millis();
  String output;
  serializeJson(doc, output);

  server.send(200, "application/json", output);
}

void setData() {
  String json = server.arg("plain");
  Serial.println("JSON:" + json);
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);
  setPresetSpeed(byte(doc["preset"]["speed"]));
  presetTimeSec = int(doc["preset"]["timeSec"]);
  presetDelaySec = byte(doc["preset"]["delaySec"]);
  autostart = byte(doc["preset"]["autostart"]);

  allowSpeedOptimizer = byte(doc["preset"]["optimizer"]["allowSpeedOptimizer"]);
  maxSpeedUp = byte(doc["preset"]["optimizer"]["maxSpeedUp"]);
  halfSpeedUp = byte(doc["preset"]["optimizer"]["halfSpeedUp"]);
  littleSpeedUp = byte(doc["preset"]["optimizer"]["littleSpeedUp"]);
  speedChangeStep = byte(doc["preset"]["optimizer"]["speedChangeStep"]);
  optimalMinG = byte(doc["preset"]["optimizer"]["optimalMinG"]);

  storePresets();

  server.send(200, "application/json", "");
}

void handleNotFound() {
  server.send(404, "text/plain", "unknown URL");
}

void blink(int xTimes, int interval) {
  for (int i = 0; i < xTimes; i++) {
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
    delay(interval);                  // wait for a second
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
    delay(interval);
  }
}

void startTimer() {
  Serial.println("Timing start");
  unsigned long currentTime = millis();
  startAfterDelay = currentTime + (presetDelaySec * 1000);
  timerFinishAt = startAfterDelay + (presetTimeSec * 1000);
  state = 1;
}

void stopTimer() {
  Serial.println("Timing stopped");
  stopMotor();
  state = 0;
  timerFinishAt = 0;
  startAfterDelay = 0;
}

void checkTimer() {
  unsigned long currentTime = millis();
  if (state == 1) {
    if (currentTime >= startAfterDelay) {
      startMotor();
      state = 2;
    }
  }
  if (state == 2) {
    optimizeSpeed();
    if (currentTime >= timerFinishAt) {
      stopTimer();
    }
  }
}

void loop() {
  server.handleClient();
}

void setup1() {
}

void loop1() {
  checkTimer();
  calculateRpm();
  if (isGyroInitiated()) {
    String data = String(millis()) + "," + String(getGyrX()) + "," + String(getGyrY()) + "," + String(getGyrZ()) + "," + String(getGx()) + "," + String(getGy()) + "," + String(getGz()) + "," + String(getTotalG()) + "," + String(getRpm()) + "\n";
    shareOnUdpPort(data);
  }
  delay(10);
}
