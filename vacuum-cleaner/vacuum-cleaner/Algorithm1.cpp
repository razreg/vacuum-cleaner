#include "Algorithm1.h"

Logger Algorithm1::logger = Logger("Algorithm1");

void Algorithm1::setConfiguration(map<string, int> config) {
	maxStepsAfterWinner = config.find(MAX_STEPS_AFTER_WINNER)->second;

}

Direction Algorithm1::step() {

	SensorInformation sensorInformation = sensor->sense();

	// construct a vector of valid directions
	vector<Direction> directions;
	directions.push_back(Direction::Stay);
	for (int i = 0; i < static_cast<int>(Direction::Stay); ++i) {
		if (!sensorInformation.isWall[i]) {
			directions.push_back(static_cast<Direction>(i));
		}
	}

	//the order of dorections in vector "directions" is: STAY, EAST, WEST, SOUTH and NORTH.
	//if the robot can't move east than move west, and if not west than move south, and if not south than move north
	//and if not north than stay.

	if (directions.size() == 1 || sensorInformation.dirtLevel > 1)
		return directions[0];
	else if (directions.size() == 2)
		return directions[1];
	else if (directions.size() == 3)
		return directions[2];
	else if (directions.size() == 4)
		return directions[3];
	else return directions[4];

}
