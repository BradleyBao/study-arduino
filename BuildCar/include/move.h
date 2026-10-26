#pragma once

void initMotors();
// speed: 0..255; direction: 0 forward, 1 left, 2 right, 3 backward,
// 4 left forward, 5 right forward, 6 left backward, 7 right backward.
void move(int speed, int direction);
void runMovementDemo();
void listenKeys();
void moveWithKeys();
