#include <Servo.h>
#include <servo.pio.h>

int servoMin = 10;
int servoMax = 170;
int presetSpeed = 90;
int currentSpeed = 0;
Servo myservo;

void setPresetSpeed(int value) {
  presetSpeed = value;
}

int getPresetSpeed() {
  return presetSpeed;
}

int getCurrentSpeed() {
  return currentSpeed;
}

void setupMotorControl() {
  myservo.attach(2);
  stopMotor();
}

void stopMotor() {
  Serial.println("Motor stopped");
  setSpeed(0);
}

void startMotor() {
  Serial.println("Motor started");
  setSpeed(presetSpeed);
}

void speedUp() {
  Serial.println("Motor speed up");
  if (currentSpeed < 100) {
    setSpeed(currentSpeed + 10);
  }
}

void speedDown() {
  Serial.println("Motor speed down");
  if (currentSpeed > 0) {
    setSpeed(currentSpeed - 10);
  }
}

void setSpeed(int speed) {
  Serial.println("Set speed: " + String(speed));
  currentSpeed = speed;
  int pos = countPosFromSpeed(currentSpeed);
  Serial.println("Servo angle: " + String(pos));
  myservo.write(pos);
}

void setMotorMax() {
  setSpeed(100);
}

int countPosFromSpeed(int speed) {
  float step = (servoMax - servoMin) / 100;
  return int(servoMin + (step * speed));
}

int countSpeedFromPos(int pos) {
  float step = (servoMax - servoMin) / 100;
  return (pos - servoMin) / step;
}