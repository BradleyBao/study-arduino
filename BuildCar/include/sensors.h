#pragma once

struct ReflectorReadings {
  int left;
  int center;
  int right;
};

void initSensors();
void setServo(int angle);
ReflectorReadings readReflectors();
void printReflectorReadings();
void printUltrasonicDistance();
int getUltrasonicDistance();
// Call every loop; prints PR readings every 200 ms without blocking.
void updateSensors();
