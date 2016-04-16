#include "Algorithm1.h"

Logger Algorithm1::logger = Logger("Algorithm1");

void Algorithm1::setConfiguration(map<string, int> config) {
	maxStepsAfterWinner = config.find(MAX_STEPS_AFTER_WINNER)->second;
	maxSteps = config.find(MAX_STEPS)->second;
	capacity = config.find(BATTERY_CAPACITY)->second;
	consumptionRate = config.find(BATTERY_CONSUMPTION_RATE)->second;
	rechargeRate = config.find(BATTERY_RECHARGE_RATE)->second;
}

Direction Algorithm1::step() {

	SensorInformation sensorInformation = sensor->sense();

	// construct a vector of valid directions
	vector<Direction> directions;
	directions.push_back(Direction::Stay); //STAY is in directions no matter what! (list's size is at least 1)
	for (int i = 0; i < static_cast<int>(Direction::Stay); ++i) {
		if (!sensorInformation.isWall[i]) {
			directions.push_back(static_cast<Direction>(i));
		}
	}

	if (movesBack.size()>0 && currValue <= ((int)movesBack.size())*consumptionRate) {
		Direction direction = movesBack.back();
		movesBack.pop_back();
		setCurrValue();
		return direction;
	}

	else {

		//the order of dorections in vector "directions" is: STAY, EAST, WEST, SOUTH and NORTH.
		//if the robot can't move east than move west, and if not west than move south, and if not south than move north
		//and if not north than stay.

		Direction returnedDir;

		if ((sensorInformation.dirtLevel > 1 && sensorInformation.dirtLevel <= 9) || directions.size() == 1)
			returnedDir = directions[0];
		else if (directions.size() == 2)
			returnedDir = directions[1];
		else if (directions.size() == 3)
			returnedDir = directions[2];
		else if (directions.size() == 4)
			returnedDir = directions[3];
		else returnedDir = directions[4];
			

		updateMovesBack(returnedDir);
		setCurrValue();
		return returnedDir;
	}
}
