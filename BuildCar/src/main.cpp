#include <Arduino.h>

#define FRS 9
#define FRD1 22
#define FRD2 24

#define FLS 10
#define FLD1 26
#define FLD2 28

#define RRS 11
#define RRD1 5
#define RRD2 6

#define RLS 12
#define RLD1 7
#define RLD2 8

void setup() {
  pinMode(FRS, OUTPUT); pinMode(FRD1, OUTPUT); pinMode(FRD2, OUTPUT);
  pinMode(FLS, OUTPUT); pinMode(FLD1, OUTPUT); pinMode(FLD2, OUTPUT);
  pinMode(RRS, OUTPUT); pinMode(RRD1, OUTPUT); pinMode(RRD2, OUTPUT);
  pinMode(RLS, OUTPUT); pinMode(RLD1, OUTPUT); pinMode(RLD2, OUTPUT);
}

void setMotor(int speedPin, int d1Pin, int d2Pin, int speed, bool forward) {
  if (forward) {
    digitalWrite(d1Pin, HIGH);
    digitalWrite(d2Pin, LOW);
  } else {
    digitalWrite(d1Pin, LOW);
    digitalWrite(d2Pin, HIGH);
  }
  analogWrite(speedPin, speed);
}

void move(int speed, int direction) {
  // speed: analog
  // direction: 0, 1, 2, 3
  // 0: Forward
  // 1: Left
  // 2: Right
  // 3: Backward

  if (direction == 0) {
    setMotor(FRS, FRD1, FRD2, speed, true);
    setMotor(FLS, FLD1, FLD2, speed, true);
    setMotor(RRS, RRD1, RRD2, speed, true);
    setMotor(RLS, RLD1, RLD2, speed, true);
  } else if (direction == 1) {
    setMotor(FRS, FRD1, FRD2, speed, true);
    setMotor(FLS, FLD1, FLD2, speed, false);
    setMotor(RRS, RRD1, RRD2, speed, false);
    setMotor(RLS, RLD1, RLD2, speed, true);
  } else if (direction == 2) {
    setMotor(FRS, FRD1, FRD2, speed, false);
    setMotor(FLS, FLD1, FLD2, speed, true);
    setMotor(RRS, RRD1, RRD2, speed, true);
    setMotor(RLS, RLD1, RLD2, speed, false);
  } else if (direction == 3) {
    setMotor(FRS, FRD1, FRD2, speed, false);
    setMotor(FLS, FLD1, FLD2, speed, false);
    setMotor(RRS, RRD1, RRD2, speed, false);
    setMotor(RLS, RLD1, RLD2, speed, false);
  }
}

void loop() {
  move(100, 0); // Forward 3 seconds
  delay(3000);
  move(100, 3); // Backward 3 seconds
  delay(3000);
  move(100, 1); // Left 3 seconds
  delay(3000);
  move(100, 2); // Right 3 seconds
  delay(3000);
}
