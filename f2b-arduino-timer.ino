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
byte speedChangeStep = 1;
byte minSpeed = 50;
byte maxSpeed = 100;
byte optimalG = 210;
byte threshold = 5;

void setup() {
  initSerial();
  //initStorage();
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

  doc["preset"]["allowSpeedOptimizer"] = allowSpeedOptimizer;
  doc["preset"]["speedChangeStep"] = speedChangeStep;
  doc["preset"]["minSpeed"] = minSpeed;
  doc["preset"]["maxSpeed"] = maxSpeed;
  doc["preset"]["optimalG"] = optimalG;
  doc["preset"]["threshold"] = threshold;

  doc["current"]["speed"] = getCurrentSpeed();
  doc["current"]["state"] = state;
  doc["current"]["finishAt"] = timerFinishAt;
  doc["current"]["rpm"] = getRpm();
  doc["current"]["time"] = millis();
  doc["current"]["g"] = getGy();

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

  allowSpeedOptimizer = byte(doc["preset"]["allowSpeedOptimizer"]);
  speedChangeStep = byte(doc["preset"]["speedChangeStep"]);
  minSpeed = byte(doc["preset"]["minSpeed"]);
  maxSpeed = byte(doc["preset"]["maxSpeed"]);
  optimalG = byte(doc["preset"]["optimalG"]);
  threshold = byte(doc["preset"]["threshold"]);

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

void optimizeSpeed() {
  if (allowSpeedOptimizer == 1) {
    float gy = getGy() * 100;
    float diffToOptimal = ((float)optimalG - gy);
    byte speed = getCurrentSpeed();
    if (diffToOptimal > threshold) {
      speed += speedChangeStep;
      if (speed <= maxSpeed) {
        setSpeed(speed);
      }
    } else if (diffToOptimal < -threshold) {
      speed -= speedChangeStep;
      if (speed >= minSpeed) {
        setSpeed(speed);
      }
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
  //calculateRpm();
  if (isGyroInitiated() && state == 2) {
    String data = String(millis()) + ";" + String(getCurrentSpeed()) + ";" + String(getGyrY()) + ";" + String(getGy()) + ";" + String(getGz()) + ";" + String(getTotalG());
    //shareOnUdpPort(data);
  }
  delay(10);
}
