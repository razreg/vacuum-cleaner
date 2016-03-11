#pragma once

const int EAST = 0;
const int WEST = 1;
const int SOUTH = 2;
const int NORTH = 3;

enum class Direction { East, West, South, North, Stay };

struct SensorInformation {
	int dirtLevel;
	bool isWall[4];
};

class AbstractSensor {
public:
	// returns the sensor's information of the current location of the robot
	virtual struct SensorInformation sense() const = 0;
};