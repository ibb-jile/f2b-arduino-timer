
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
  setupLogger();
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
  startServer();
  blink(20, 50);
  digitalWrite(LED_BUILTIN, HIGH);

  if (autostart == 1) {
    delay(10000);
    startTimer();
  }
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

  prepareLog();
}

void stopTimer() {
  Serial.println("Timing stopped");
  stopMotor();
  state = 0;
  timerFinishAt = 0;
  startAfterDelay = 0;
  endLog();
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
  handleServerClients();
}

void setup1() {
}

void loop1() {
  checkTimer();
  calculateRpm();
  if (isGyroInitiated() && state == 2) {
    String data = String(millis()) + ";" + String(getCurrentSpeed()) + ";" + String(getGyrY()) + ";" + String(getGy()) + ";" + String(getGz()) + ";" + String(getTotalG());
    log(data);
    //shareOnUdpPort(data);
  }
  delay(10);
}
