#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <StreamString.h>
#include <ArduinoJson.h>
WebServer server(80);

int modePin = 15;
int mode = 0;
int state = 0;
unsigned long timerFinishAt = 0;
unsigned long startAfterDelay = 0;

int presetTimeSec = 60;
int presetDelaySec = 30;
int autostart = 1;

void setup() {
  initSerial();
  Serial.println("Welcome Stunt Timer");
  initHW();
  setupMotorControl();
  Serial.println("HW is ready");

  if (digitalRead(modePin) == LOW) {
    Serial.println("Setup range");
    setupRange();
  }
  blink(10, 100);
  stopMotor();
  blink(20, 80);

  setupWifi();
  server.on("/set", setData);
  server.on("/get", getData);
  server.on("/timer/start", startTimerRequest);
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
void startTimerRequest() {
  startTimer();
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
  doc["current"]["speed"] = getCurrentSpeed();
  doc["current"]["state"] = state;
  doc["current"]["finishAt"] = timerFinishAt;
  doc["current"]["time"] = millis();
  String output;
  serializeJson(doc, output);

  server.send(200, "application/json", output);
}

void setData() {
  Serial.println("try to set data");
  WiFiClient client = server.client();
  if (!client) {
    Serial.println("no client");
  }
  while (!client.available()) {
    delay(10);
  }
  String body = client.readStringUntil('\n');
  Serial.println(body);
  /* StaticJsonDocument<500> doc;

  doc["preset"]["speed"] = presetSpeed;
  doc["preset"]["timeSec"] = presetTimeSec;
  doc["preset"]["delaySec"] = presetDelaySec;
  doc["preset"]["autostart"] = autostart;
  doc["current"]["speed"] = currentSpeed;
  doc["current"]["startAt"] = startAt;
  doc["current"]["state"] = state;
  String output;
  serializeJson(doc, output);*/

  server.send(200, "application/json", body);
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
  startAfterDelay = currentTime + presetDelaySec * 1000;
  timerFinishAt = startAfterDelay + currentTime + presetTimeSec * 1000;
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
    if (currentTime >= timerFinishAt) {
      stopTimer();
    }
  }
}

void loop() {
  server.handleClient();
  delay(10);
}

void setup1() {
}

void loop1() {
  checkTimer();
  delay(10);
}
