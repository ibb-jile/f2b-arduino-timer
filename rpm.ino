void rpmInit() {
  pinMode(10, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(10), countRpm, FALLING);
}

int counter = 0;
int rpm = 0;
int numberOfBlades = 3;
unsigned long lastCalculationTime = 0;

int getRpm() {
  return rpm;
}

void calculateRpm() {
  unsigned long current = millis();
  if (current - lastCalculationTime <= 1000) {
    rpm = (counter / numberOfBlades) * 60;
    lastCalculationTime = current;
    counter = 0;
  }
}

void countRpm() {
  counter++;
}