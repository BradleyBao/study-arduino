#include <Arduino.h>
#include "sensors.h"
#include "move.h"

namespace {
  static unsigned long lastCmdTime = 0;
  static bool manualTimeoutActive = false;
  static unsigned long autoStoppedSince = 0;
  static bool autoStoppedTimeoutActive = false;
  const unsigned long CMD_TIMEOUT = 350;
  const unsigned long AUTO_STOP_TIMEOUT = 5000;
  volatile bool isManual = true;

  void setManualMove(int direction) {
    isManual = true;
    move(120, direction);
    lastCmdTime = millis();
    manualTimeoutActive = true;
  }

  void reportControlMode() {
    Serial.print("MODE:");
    Serial.println(isManual ? "MANUAL" : "AUTO");
  }
}

bool isManualControl() {
  return isManual;
}

void initializeControl() {
  reportControlMode();
}

bool moveAlongLine() {
  const ReflectorReadings readings = readReflectors();
  if (readings.center == 1 && readings.left == 1 && readings.right == 1) {
    move(100, 0); // Forward
  } else if (readings.center == 1 && readings.left == 1 && readings.right == 0) {
    move(100, 5); // Forward Right
  } else if (readings.center == 1 && readings.left == 0 && readings.right == 1) {
    move(100, 4); // Forward Left
  } else if (readings.center == 0 && readings.left == 1 && readings.right == 1) {
    move(100, 0); // Forward
  } else if (readings.center == 0 && readings.left == 1 && readings.right == 0) {
    move(100, 4); // Forward Left
  } else if (readings.center == 0 && readings.left == 0 && readings.right == 1) {
    move(100, 5); // Forward Right
  } else if (readings.center == 1 && readings.left == 0 && readings.right == 0) {
    move(100, 0); // Forward
  }
  else {
    move(0, 0); // Stop
    return false;
  }
  return true;
}

bool hasObstacle(int thresholdDistance = 10) {
    int distance = getUltrasonicDistance();
    if (distance < thresholdDistance) { // If an obstacle is detected within the threshold distance
        return true;
    }
    return false;
}

void gameControl(){
  while (Serial.available() > 0) {
    char key = Serial.read();
    switch (key) {
      case 't': case 'T':
        isManual = !isManual;
        lastCmdTime = 0;
        manualTimeoutActive = false;
        autoStoppedSince = 0;
        autoStoppedTimeoutActive = false;
        move(0, 0);
        reportControlMode();
        break;
      case 'w': case 'W': setManualMove(0); break; // Forward
      case 'a': case 'A': setManualMove(2); break; // Left
      case 'd': case 'D': setManualMove(1); break; // Right
      case 's': case 'S': setManualMove(3); break; // Backward
      case 'q': case 'Q': setManualMove(4); break; // Left Forward
      case 'e': case 'E': setManualMove(5); break; // Right Forward
      case 'c': case 'C': setManualMove(7); break; // Right Backward
      case 'z': case 'Z': setManualMove(6); break; // Left Backward
      case ' ': case 'x':
        move(0, 0);
        if (isManual) {
          lastCmdTime = millis();
          manualTimeoutActive = true;
        }
        break; // Stop
      default: break;
    }
  }

  // Stop if the controller heartbeat is lost, but stay in manual mode.
  if (isManual && manualTimeoutActive && (millis() - lastCmdTime > CMD_TIMEOUT)) {
    move(0, 0);
    lastCmdTime = 0;
    manualTimeoutActive = false;
  }
}

void updateAutoStopTimeout(bool isStopped) {
  if (isManual) {
    autoStoppedTimeoutActive = false;
    return;
  }

  if (!isStopped) {
    autoStoppedTimeoutActive = false;
    return;
  }

  const unsigned long now = millis();
  if (!autoStoppedTimeoutActive) {
    autoStoppedSince = now;
    autoStoppedTimeoutActive = true;
  } else if (now - autoStoppedSince >= AUTO_STOP_TIMEOUT) {
    move(0, 0);
    isManual = true;
    autoStoppedTimeoutActive = false;
    manualTimeoutActive = false;
    lastCmdTime = 0;
    Serial.println("EVENT:AUTO_STOP_TIMEOUT");
    reportControlMode();
  }
}