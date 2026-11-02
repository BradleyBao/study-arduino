#include <Arduino.h>
#include <Servo.h>
#include "sensors.h"

namespace {
constexpr int SERVO_PIN = 13;
constexpr int LOWER_LIMIT = 650;
constexpr int UPPER_LIMIT = 2800;
constexpr int US_Echo_PIN = 38;
constexpr int US_Trig_PIN = 30;
constexpr unsigned long US_ECHO_TIMEOUT_US = 25000UL;
constexpr int Left_PR_sensor_Pin = 2;
constexpr int Center_PR_sensor_Pin = 3;
constexpr int Right_PR_sensor_Pin = 4;

Servo servo;
} // namespace

void initSensors() {
    servo.attach(SERVO_PIN, LOWER_LIMIT, UPPER_LIMIT);
    servo.write(90);
    pinMode(US_Echo_PIN, INPUT);
    pinMode(US_Trig_PIN, OUTPUT);
    pinMode(Left_PR_sensor_Pin, INPUT);
    pinMode(Center_PR_sensor_Pin, INPUT);
    pinMode(Right_PR_sensor_Pin, INPUT);
}

void setServo(int angle) {
  servo.write(angle);
}

ReflectorReadings readReflectors() {
  return {
    digitalRead(Left_PR_sensor_Pin),
    digitalRead(Center_PR_sensor_Pin),
    digitalRead(Right_PR_sensor_Pin)
  };
}

void printReflectorReadings() {
  const ReflectorReadings readings = readReflectors();
  Serial.print("Left: "); Serial.print(readings.left);
  Serial.print(" Center: "); Serial.print(readings.center);
  Serial.print(" Right: "); Serial.println(readings.right);
}

void printUltrasonicDistance() {
  const int distance = getUltrasonicDistance();
  Serial.print("Distance: "); Serial.print(distance); Serial.println(" cm");
}

int getUltrasonicDistance() {
  digitalWrite(US_Trig_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(US_Trig_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_Trig_PIN, LOW);

  const unsigned long duration = pulseIn(US_Echo_PIN, HIGH, US_ECHO_TIMEOUT_US);
  if (duration == 0) {
    return -1; // Missing echo is not proof that the path is clear.
  }
  const int distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}

void updateSensors() {
  static unsigned long lastPrint = 0;
  static bool printed = false;
  const unsigned long now = millis();
  if (!printed || now - lastPrint >= 200UL) {
    printed = true;
    lastPrint = now;
    // printReflectorReadings();
  }
}
