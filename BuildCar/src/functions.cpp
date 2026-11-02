#include <Arduino.h>
#include "functions.h"
#include "sensors.h"
#include "move.h"

namespace {
  const int OBSTACLE_THRESHOLD_CM = 30;
  const int AUTO_SPEED = 160;
  const int AUTO_RIGHT_SPEED = AUTO_SPEED * 218L / 255L;
  const int SIDE_CLEAR_CM = 45;
  const int WALL_MAX_CM = 90;
  const unsigned long SIDE_SCAN_INTERVAL_MS = 260;
  const unsigned long SERVO_SETTLE_MS = 130;
  const unsigned long TURN_90_MS = 450;
  const unsigned long TURN_LIMIT_MS = 800;
  static unsigned long lastCmdTime = 0;
  static bool manualTimeoutActive = false;
  static unsigned long autoStoppedSince = 0;
  static bool autoStoppedTimeoutActive = false;
  const unsigned long CMD_TIMEOUT = 350;
  const unsigned long AUTO_STOP_TIMEOUT = 5000;
  bool isManual = false;
  int turnDirection = 0; // 8 = left, 9 = right
  int turnsCompleted = 0;
  int wallSide = 0; // -1 = left, +1 = right
  int wallTargetCm = 30;
  int wallDistanceCm = -1;
  unsigned long turnStartedAt = 0;
  unsigned long lastSideScanAt = 0;
  bool turning = false;
  bool autoHalted = false;

  void resetAutoRoute() {
    turnDirection = 0;
    turnsCompleted = 0;
    wallSide = 0;
    wallDistanceCm = -1;
    lastSideScanAt = 0;
    turning = false;
    autoHalted = false;
  }

  bool isValidDistance(int distance) {
    return distance > 0;
  }

  int readAtAngle(int angle) {
    setServo(angle);
    delay(SERVO_SETTLE_MS);
    return getUltrasonicDistance();
  }

  void centerServo() {
    setServo(90);
    delay(SERVO_SETTLE_MS);
  }

  void driveAlongWall() {
    int correction = 0;
    if (wallSide != 0 && isValidDistance(wallDistanceCm) &&
        wallDistanceCm < WALL_MAX_CM) {
      const int error = wallTargetCm - wallDistanceCm;
      if (abs(error) > 2) {
        correction = constrain(error * 4 * -wallSide, -40, 40);
      }
    }
    // Positive correction steers right. Keep the measured right-side motor trim.
    const int leftSpeed = constrain(AUTO_SPEED + correction, 0, 255);
    const int rightSpeed = constrain(AUTO_RIGHT_SPEED - correction, 0, 255);
    move(AUTO_SPEED, 0, leftSpeed, leftSpeed, rightSpeed, rightSpeed);
  }

  void spinAuto() {
    move(AUTO_SPEED, turnDirection, AUTO_SPEED, AUTO_SPEED,
         AUTO_RIGHT_SPEED, AUTO_RIGHT_SPEED);
  }

  void setManualMove(int direction) {
    isManual = true;
    resetAutoRoute();
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

bool hasObstacle(int thresholdDistance) {
  // unit is centimeters
  int distance = getUltrasonicDistance();
  Serial.print("Ultrasonic Distance: "); Serial.print(distance); Serial.println(" cm");
  if (distance < 0 || distance < thresholdDistance) {
      return true;
  }
  return false;
}

ObstacleDistances detectObstacle3D(int delayTime) {
  // unit is centimeters
  // Detect front obstacle
  setServo(0);
  delay(delayTime);
  int leftDistance = getUltrasonicDistance();
  setServo(90); // Center
  delay(delayTime);
  int centerDistance = getUltrasonicDistance();
  setServo(180); // Right
  delay(delayTime);
  int rightDistance = getUltrasonicDistance();
  // Print the distances for debugging
  Serial.print("Left Distance: "); Serial.print(leftDistance); Serial.println(" cm");
  Serial.print("Center Distance: "); Serial.print(centerDistance); Serial.println(" cm");
  Serial.print("Right Distance: "); Serial.print(rightDistance); Serial.println(" cm");
  setServo(90);
  delay(delayTime);
  return {leftDistance, centerDistance, rightDistance};
}

AngleObstacleDistances scanWithAngle(int angle, int thresholdDistance, int delayTime) {
  setServo(angle);
  delay(delayTime);
  int distance = getUltrasonicDistance();
  return {angle, distance};
}

void autoControl() {
  if (autoHalted) {
    move(0, 0);
    updateAutoStopTimeout(true);
    return;
  }
  if (turning) {
    const unsigned long elapsed = millis() - turnStartedAt;
    if (elapsed < TURN_90_MS) {
      spinAuto();
      updateAutoStopTimeout(false);
      return;
    }
    move(0, 0);
    const int front = getUltrasonicDistance();
    if (isValidDistance(front) && front > OBSTACLE_THRESHOLD_CM) {
      turning = false;
      ++turnsCompleted;
      wallSide = 0;
      lastSideScanAt = 0;
      Serial.print("EVENT:TURN_COMPLETE:"); Serial.println(turnsCompleted);
      updateAutoStopTimeout(false);
      return;
    }
    if (elapsed >= TURN_LIMIT_MS) {
      turning = false;
      autoHalted = true;
      Serial.println("EVENT:TURN_FAILED");
      updateAutoStopTimeout(true);
      return;
    }
    spinAuto();
    updateAutoStopTimeout(false);
    return;
  }

  const int front = getUltrasonicDistance();
  if (!isValidDistance(front)) {
    move(0, 0);
    updateAutoStopTimeout(true);
    return;
  }
  if (front <= OBSTACLE_THRESHOLD_CM) {
    move(0, 0);
    if (turnsCompleted >= 2) {
      autoHalted = true;
      Serial.println("EVENT:ROUTE_COMPLETE");
      updateAutoStopTimeout(true);
      return;
    }
    const ObstacleDistances sides = detectObstacle3D(SERVO_SETTLE_MS);
    // The first opening chooses the course direction. Both top corners turn
    // the same way; never choose a different side at the second corner.
    if (turnDirection == 0) {
      if (sides.left > SIDE_CLEAR_CM || sides.right > SIDE_CLEAR_CM) {
        turnDirection = sides.left >= sides.right ? 8 : 9;
      }
    }
    const int chosenSide = turnDirection == 8 ? sides.left : sides.right;
    const int confirmedFront = getUltrasonicDistance();
    if (isValidDistance(confirmedFront) && confirmedFront > OBSTACLE_THRESHOLD_CM) {
      updateAutoStopTimeout(false);
      return;
    }
    if (!isValidDistance(chosenSide) || chosenSide <= SIDE_CLEAR_CM ||
        !isValidDistance(confirmedFront)) {
      autoHalted = true;
      Serial.println("EVENT:NO_CLEAR_TURN");
      updateAutoStopTimeout(true);
      return;
    }
    turning = true;
    turnStartedAt = millis();
    Serial.println(turnDirection == 8 ? "EVENT:TURN_LEFT" : "EVENT:TURN_RIGHT");
    spinAuto();
    updateAutoStopTimeout(false);
    return;
  }

  if (lastSideScanAt == 0 || millis() - lastSideScanAt >= SIDE_SCAN_INTERVAL_MS) {
    move(0, 0); // Never drive without a forward reading while the servo points sideways.
    if (wallSide == 0) {
      const int left = readAtAngle(0);
      const int right = readAtAngle(180);
      if (isValidDistance(left) && left < WALL_MAX_CM &&
          (!isValidDistance(right) || left <= right)) {
        wallSide = -1;
        wallDistanceCm = left;
      } else if (isValidDistance(right) && right < WALL_MAX_CM) {
        wallSide = 1;
        wallDistanceCm = right;
      }
      if (wallSide != 0) {
        wallTargetCm = constrain(wallDistanceCm, 20, 50);
      }
    } else {
      wallDistanceCm = readAtAngle(wallSide == -1 ? 0 : 180);
      if (!isValidDistance(wallDistanceCm) || wallDistanceCm >= WALL_MAX_CM) {
        wallSide = 0;
      }
    }
    centerServo();
    lastSideScanAt = millis();
    // A side scan takes time. Check ahead again before applying motor power.
    const int checkedFront = getUltrasonicDistance();
    if (!isValidDistance(checkedFront) || checkedFront <= OBSTACLE_THRESHOLD_CM) {
      updateAutoStopTimeout(!isValidDistance(checkedFront));
      return;
    }
  }
  driveAlongWall();
  updateAutoStopTimeout(false);
}

void gameControl(){
  while (Serial.available() > 0) {
    char key = Serial.read();
    switch (key) {
      case 't': case 'T':
        isManual = !isManual;
        resetAutoRoute();
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
        if (!isManual) {
          isManual = true;
          resetAutoRoute();
          reportControlMode();
        }
        lastCmdTime = millis();
        manualTimeoutActive = true;
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
