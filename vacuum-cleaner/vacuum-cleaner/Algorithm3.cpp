#include "Algorithm3.h"

Logger Algorithm3::logger = Logger("Algorithm3");

void Algorithm3::setConfiguration(map<string, int> config) {
	maxStepsAfterWinner = config.find(MAX_STEPS_AFTER_WINNER)->second;
	maxSteps = config.find(MAX_STEPS)->second;
	capacity = config.find(BATTERY_CAPACITY)->second;
	consumptionRate = config.find(BATTERY_CONSUMPTION_RATE)->second;
	rechargeRate = config.find(BATTERY_RECHARGE_RATE)->second;
	currValue = capacity;
}

Direction Algorithm3::step() {

	SensorInformation sensorInformation = sensor->sense();

	//DELETE 
	cout << "currValue: " << currValue << endl;
	cout << movesBack.size() << endl;
	cout << "dirt: " << sensorInformation.dirtLevel << endl;

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
		else { // two possible directions at least
			if (isDirectionInList(directions, Direction::East))
				returnedDir = Direction::East;
			else if (isDirectionInList(directions, Direction::South))
				returnedDir = Direction::South;
			else if (isDirectionInList(directions, Direction::North))
				returnedDir = Direction::North;
			else returnedDir = Direction::West;
		}

		updateMovesBack(returnedDir);
		setCurrValue();
		return returnedDir;
	}
}
