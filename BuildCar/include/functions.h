#pragma once

struct ObstacleDistances {
	int left;
	int center;
	int right;
};

struct AngleObstacleDistances {
    int angle;
    int distance;
};

bool isManualControl();
void initializeControl();
bool moveAlongLine();
bool hasObstacle(int thresholdDistance = 10);
ObstacleDistances detectObstacle3D(int delayTime = 1000);
AngleObstacleDistances scanWithAngle(int angle, int thresholdDistance, int delayTime);
void autoControl();
void gameControl();
void updateAutoStopTimeout(bool isStopped);