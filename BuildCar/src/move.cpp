#include <Arduino.h>
#include "move.h"

namespace {
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

#define ENCODER_A_PIN 20
#define ENCODER_B_PIN 21


volatile int angle = 0;

void setMotor(int speedPin, int d1Pin, int d2Pin, int speed, bool forward) {
  speed = constrain(speed, 0, 255);
  digitalWrite(d1Pin, forward ? HIGH : LOW);
  digitalWrite(d2Pin, forward ? LOW : HIGH);
  analogWrite(speedPin, speed);
}

void rotateA() {
  int b = digitalRead(ENCODER_B_PIN);
  if (b == HIGH) {
    angle++;
  } else {
    angle--;
  }
}

// namespace
}

void initMotors() {
  pinMode(FRS, OUTPUT); pinMode(FRD1, OUTPUT); pinMode(FRD2, OUTPUT);
  pinMode(FLS, OUTPUT); pinMode(FLD1, OUTPUT); pinMode(FLD2, OUTPUT);
  pinMode(RRS, OUTPUT); pinMode(RRD1, OUTPUT); pinMode(RRD2, OUTPUT);
  pinMode(RLS, OUTPUT); pinMode(RLD1, OUTPUT); pinMode(RLD2, OUTPUT);
  pinMode(ENCODER_A_PIN, INPUT); pinMode(ENCODER_B_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_A_PIN), rotateA, RISING);
}

void listenKeys() {
  if (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n' || inChar == '\r') return;
      Serial.println("Key pressed: " + String(inChar));
    }
}

void moveWithKeys() {
  if (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n' || inChar == '\r') return;
      Serial.println("Key pressed: " + String(inChar));
      switch (inChar) {
        case 'w':
          move(100, 0); // Forward
          break;
        case 'a':
          move(100, 1); // Left
          break;
        case 'd':
          move(100, 2); // Right
          break;
        case 's':
          move(100, 3); // Backward
          break;
        case 'q':
          move(100, 4); // Left Forward
          break;
        case 'e':
          move(100, 5); // Right Forward
          break;
        default:
          move(0, 0); // Stop
          break;
      }
    }
}

void move(int speed, int direction, int speed_lf, int speed_lr,
          int speed_rf, int speed_rr) {
    // speed: analog
    // direction: 0, 1, 2, 3
    // 0: Forward
    // 1: Left
    // 2: Right
    // 3: Backward
    // 4: Left Forward
    // 5: Right Forward
    // 6: Left Backward
    // 7: Right Backward
    // 8: Left Turn
    // 9: Right Turn

  if (speed_lf < 0 || speed_lr < 0 || speed_rf < 0 || speed_rr < 0) {
    speed_lf = speed;
    speed_lr = speed;
    speed_rf = speed;
    speed_rr = speed;
  }

  if (direction == 0) {
    setMotor(FRS, FRD1, FRD2, speed_rf, true);
    setMotor(FLS, FLD1, FLD2, speed_lf, true);
    setMotor(RRS, RRD1, RRD2, speed_rr, true);
    setMotor(RLS, RLD1, RLD2, speed_lr, true);
  } else if (direction == 1) {
    setMotor(FRS, FRD1, FRD2, speed_rf, true);
    setMotor(FLS, FLD1, FLD2, speed_lf, false);
    setMotor(RRS, RRD1, RRD2, speed_rr, false);
    setMotor(RLS, RLD1, RLD2, speed_lr, true);
  } else if (direction == 2) {
    setMotor(FRS, FRD1, FRD2, speed_rf, false);
    setMotor(FLS, FLD1, FLD2, speed_lf, true);
    setMotor(RRS, RRD1, RRD2, speed_rr, true);
    setMotor(RLS, RLD1, RLD2, speed_lr, false);
  } else if (direction == 3) {
    setMotor(FRS, FRD1, FRD2, speed_rf, false);
    setMotor(FLS, FLD1, FLD2, speed_lf, false);
    setMotor(RRS, RRD1, RRD2, speed_rr, false);
    setMotor(RLS, RLD1, RLD2, speed_lr, false);
  } else if (direction == 4) {
    setMotor(FRS, FRD1, FRD2, speed_rf, true);
    setMotor(FLS, FLD1, FLD2, speed_lf / 3, true);
    setMotor(RRS, RRD1, RRD2, speed_rr, true);
    setMotor(RLS, RLD1, RLD2, speed_lr / 3, true);
  } else if (direction == 5) {
    setMotor(FRS, FRD1, FRD2, speed_rf / 3, true);
    setMotor(FLS, FLD1, FLD2, speed_lf, true);
    setMotor(RRS, RRD1, RRD2, speed_rr / 3, true);
    setMotor(RLS, RLD1, RLD2, speed_lr, true);
  } else if (direction == 6) {
    setMotor(FRS, FRD1, FRD2, speed_rf / 3, false);
    setMotor(FLS, FLD1, FLD2, speed_lf, false);
    setMotor(RRS, RRD1, RRD2, speed_rr / 3, false);
    setMotor(RLS, RLD1, RLD2, speed_lr, false);
  } else if (direction == 7) {
    setMotor(FRS, FRD1, FRD2, speed_rf, false);
    setMotor(FLS, FLD1, FLD2, speed_lf / 3, false);
    setMotor(RRS, RRD1, RRD2, speed_rr, false);
    setMotor(RLS, RLD1, RLD2, speed_lr / 3, false);
  } else if (direction == 8){
    setMotor(FRS, FRD1, FRD2, speed_rf, true);
    setMotor(FLS, FLD1, FLD2, speed_lf, false);
    setMotor(RRS, RRD1, RRD2, speed_rr, true);
    setMotor(RLS, RLD1, RLD2, speed_lr, false);
  } else if (direction == 9){
    setMotor(FRS, FRD1, FRD2, speed_rf, false);
    setMotor(FLS, FLD1, FLD2, speed_lf, true);
    setMotor(RRS, RRD1, RRD2, speed_rr, false);
    setMotor(RLS, RLD1, RLD2, speed_lr, true);
  } else {
    // Stop
    setMotor(FRS, FRD1, FRD2, 0, true);
    setMotor(FLS, FLD1, FLD2, 0, true);
    setMotor(RRS, RRD1, RRD2, 0, true);
    setMotor(RLS, RLD1, RLD2, 0, true);
  }
}

void runMovementDemo() {
  static const int directions[] = {0, 3, 1, 2};
  static unsigned long lastChange = 0;
  static unsigned int step = 0;
  static bool started = false;
  const unsigned long now = millis();

  if (!started) {
    started = true;
    lastChange = now;
    move(100, directions[step]);
  } else if (now - lastChange >= 3000UL) {
    lastChange = now;
    step = (step + 1) % 4;
    move(100, directions[step]);
  }
}
