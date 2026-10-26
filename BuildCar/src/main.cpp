#include <Arduino.h>
#include "move.h"
#include "sensors.h"
#include "functions.h"

void setup() {
  initMotors();
  initSensors();
  Serial.begin(9600);
  initializeControl();
}

void loop() {
  gameControl();
  if (!isManualControl()) {
    const bool obstacleDetected = hasObstacle(15);
    bool autoStopped = obstacleDetected;
    if (obstacleDetected) {
      move(0, 0);
    } else {
      autoStopped = !moveAlongLine();
    }
    updateAutoStopTimeout(autoStopped);
  }
  delay(100);
}
