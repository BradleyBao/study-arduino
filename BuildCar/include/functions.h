#pragma once

bool isManualControl();
void initializeControl();
bool moveAlongLine();
bool hasObstacle(int thresholdDistance = 10);
void gameControl();
void updateAutoStopTimeout(bool isStopped);