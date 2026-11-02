#pragma once

void initMotors();
// speed: 0..255; direction: 0 forward, 1 left, 2 right, 3 backward,
// 4 left forward, 5 right forward, 6 left backward, 7 right backward,
// 8 spin left (left wheels backward, right wheels forward),
// 9 spin right (left wheels forward, right wheels backward).
// Optional wheel speeds must all be supplied to override the base speed.
void move(int speed, int direction, int speed_lf = -1, int speed_lr = -1,
		  int speed_rf = -1, int speed_rr = -1);
void runMovementDemo();
void listenKeys();
void moveWithKeys();
