#include <Servo.h>
#include <servo.pio.h>

byte servoMin = 10;
byte servoMax = 170;
byte presetSpeed = 90;
byte currentSpeed = 0;
Servo myservo;

void setPresetSpeed(byte value) {
  presetSpeed = value;
}

byte getPresetSpeed() {
  return presetSpeed;
}

byte getCurrentSpeed() {
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
    setSpeed(currentSpeed + 1);
  }
}

void speedDown() {
  Serial.println("Motor speed down");
  if (currentSpeed > 0) {
    setSpeed(currentSpeed - 1);
  }
}

void setSpeed(byte speed) {
  Serial.println("Set speed: " + String(speed));
  currentSpeed = speed;
  byte pos = countPosFromSpeed(currentSpeed);
  Serial.println("Servo angle: " + String(pos));
  myservo.write(pos);
}

void setMotorMax() {
  setSpeed(100);
}

byte countPosFromSpeed(byte speed) {
  float step = (servoMax - servoMin) / 10;
  return byte(servoMin + ((step * speed) / 10));
}

byte countSpeedFromPos(byte pos) {
  float step = (servoMax - servoMin) / 100;
  return (pos - servoMin) / step;
}