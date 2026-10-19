#include "sensors.h"
#include "move.h"

void moveAlongLine() {
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
  }
}

bool hasObstacle(int thresholdDistance = 10) {
    int distance = getUltrasonicDistance();
    if (distance < thresholdDistance) { // If an obstacle is detected within the threshold distance
        return true;
    }
    return false;
}