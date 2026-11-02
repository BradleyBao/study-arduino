#include <Arduino.h>
#include "move.h"
#include "sensors.h"
#include "functions.h"

namespace {
int AUTO_SPEED = 255;
int AUTO_RIGHT_SPEED = AUTO_SPEED * 218L / 255L;
const int TARGET_DISTANCE_CM = 40;
const int DISTANCE_DEADBAND_CM = 5;
const int LEFT_STEERING_GAIN = 4;
const int RIGHT_STEERING_GAIN = 2;
const int MAX_LEFT_STEERING_CORRECTION = 60;
const int MAX_RIGHT_STEERING_CORRECTION = 30;
}

void setup() {
  initMotors();
  initSensors();
  Serial.begin(9600);
  initializeControl();
  setServo(135);
  delay(300);
}

void loop() {
  const int distance = getUltrasonicDistance();
  int correction = 0;
  if (distance > 0) {
    const int distanceError = TARGET_DISTANCE_CM - distance;
    AUTO_SPEED = 255;
    AUTO_RIGHT_SPEED = AUTO_SPEED * 218L / 255L;
    if (abs(distanceError) > DISTANCE_DEADBAND_CM) {
      AUTO_SPEED = 100;
      AUTO_RIGHT_SPEED = AUTO_SPEED * 218L / 255L;
      if (distanceError < 0) {
        correction = constrain(distanceError * LEFT_STEERING_GAIN,
                               -MAX_LEFT_STEERING_CORRECTION, 0);
      } else {
        correction = constrain(distanceError * RIGHT_STEERING_GAIN,
                               0, MAX_RIGHT_STEERING_CORRECTION);
      }
    }
  }

  const int leftSpeed = constrain(AUTO_SPEED + correction, 0, 255);
  const int rightSpeed = constrain(AUTO_RIGHT_SPEED - correction, 0, 255);
  move(AUTO_SPEED, 0, leftSpeed, leftSpeed, rightSpeed, rightSpeed);
  delay(20);
}
