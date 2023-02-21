#include <MPU6500_WE.h>
#include <Wire.h>
#define MPU6500_ADDR 0x68

MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);

bool initDone = false;

void setupGyro() {
  Serial.println("Setup gyro");
  Wire.setSCL(21);
  Wire.setSDA(8);
  Wire.begin();
  if (!myMPU6500.init()) {
    Serial.println("MPU6500 does not respond");
  } else {
    Serial.println("MPU6500 is connected");
    Serial.println("Position you MPU6500 flat and don't move it - calibrating...");
    delay(1000);
    myMPU6500.autoOffsets();
    myMPU6500.enableGyrDLPF();
    myMPU6500.setGyrDLPF(MPU6500_DLPF_6);
    myMPU6500.setSampleRateDivider(5);
    myMPU6500.setGyrRange(MPU6500_GYRO_RANGE_500);
    myMPU6500.setAccRange(MPU6500_ACC_RANGE_8G);
    myMPU6500.enableAccDLPF(true);
    myMPU6500.setAccDLPF(MPU6500_DLPF_6);
    Serial.println("Done!");
    initDone = true;
  }
}
bool isGyroInitiated() {
  return initDone;
}

float getGx() {
  return myMPU6500.getGValues().x;
}
float getGy() {
  return myMPU6500.getGValues().y;
}
float getGz() {
  return myMPU6500.getGValues().z;
}
float getTotalG() {
  return myMPU6500.getResultantG(myMPU6500.getGValues());
}

float getGyrX() {
  return myMPU6500.getGyrValues().x;
}
float getGyrY() {
  return myMPU6500.getGyrValues().y;
}
float getGyrZ() {
  return myMPU6500.getGyrValues().z;
}