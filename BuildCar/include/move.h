#pragma once

void initMotors();
// speed: 0..255; direction: 0 forward, 1 left, 2 right, 3 backward,
// 4 left forward, 5 right forward.
void move(int speed, int direction);
void runMovementDemo();
