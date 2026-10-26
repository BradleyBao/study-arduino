#include <Arduino.h>
#include "move.h"
#include "sensors.h"
#include "functions.h"

void setup() {
  initMotors();
  initSensors();
  Serial.begin(9600);
}

void loop() {
  // runMovementDemo();
  // updateSensors();
  // moveAlongLine();
  // if (!hasObstacle(15)) {
  //   moveAlongLine();
  // } else {
  //   move(0, 0); // Stop
  // }
  // listenKeys();
  gameControl();
  delay(100);
}
