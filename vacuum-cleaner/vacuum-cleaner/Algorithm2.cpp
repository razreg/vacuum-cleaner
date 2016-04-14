#include "Algorithm2.h"

Logger Algorithm2::logger = Logger("Algorithm2");


void Algorithm2::setConfiguration(map<string, int> config) {
	maxStepsAfterWinner = config.find(MAX_STEPS_AFTER_WINNER)->second;
	maxSteps = config.find(MAX_STEPS)->second;
	capacity = config.find(BATTERY_CAPACITY)->second;
	consumptionRate = config.find(BATTERY_CONSUMPTION_RATE)->second;
	rechargeRate = config.find(BATTERY_RECHARGE_RATE)->second;
	currValue = capacity;
}

Direction Algorithm2::step() {

	SensorInformation sensorInformation = sensor->sense();

	//DELETE 
	cout << "currValue: " << currValue << endl;
	cout << movesBack.size() << endl;
	cout << "(" << Xpos << ", " << Ypos<< ")" << endl;
	cout << "dirt: " << sensorInformation.dirtLevel << endl;
	cout << "size of outOFDock: " << outOFDock.size() << endl;

	// construct a vector of valid directions
	vector<Direction> directions;
	directions.push_back(Direction::Stay);
	for (int i = 0; i < static_cast<int>(Direction::Stay); ++i) {
		if (!sensorInformation.isWall[i]) {
			directions.push_back(static_cast<Direction>(i));
		}
	}

	//in case battery is about to end && robot is not in docking station
	if (movesBack.size()>0 && currValue <= ((int)movesBack.size())*consumptionRate) {
		Direction direction = movesBack.back();
		movesBack.pop_back();
		updatePosition(direction);
		setCurrValue();
		return direction;
	}

	else {

		bool foundDirection = false;

		if (sensorInformation.dirtLevel > 1 && sensorInformation.dirtLevel <= 9){
			updatePrevAndReturnDirection(Direction::Stay);
			foundDirection = true;
		}

		//robot is in docking station and tries to make a different move than before(if there was any)
		else if (inDockingStation() && (!outOFDock.empty())){
			for (Direction &direction : directions){
				if (!isDirectionInList(outOFDock, direction) && (static_cast<int>(direction) != 4)){ //exclude STAY
					updatePrevAndReturnDirection(direction);
					foundDirection = true;
					break;
				}
			}
		}

		if (!foundDirection)
		{
			if (previousDirection == Direction::South) { //robot was 1 square above
				if (isDirectionInList(directions, Direction::South))
					updatePrevAndReturnDirection(Direction::South);
				else if (isDirectionInList(directions, Direction::East))
					updatePrevAndReturnDirection(Direction::East);
				//got to corner
				else if (isDirectionInList(directions, Direction::West))
					updatePrevAndReturnDirection(Direction::West);
				//got to dead end
				else if (isDirectionInList(directions, Direction::North))
					updatePrevAndReturnDirection(Direction::North);
				//stack!
				else updatePrevAndReturnDirection(Direction::Stay);
			}

			else if (previousDirection == Direction::North){ //robot was 1 square below
				if (isDirectionInList(directions, Direction::North))
					updatePrevAndReturnDirection(Direction::North);
				else if (isDirectionInList(directions, Direction::East))
					updatePrevAndReturnDirection(Direction::East);
				//got to corner
				else if (isDirectionInList(directions, Direction::West))
					updatePrevAndReturnDirection(Direction::West);
				//got to dead end
				else if (isDirectionInList(directions, Direction::South))
					updatePrevAndReturnDirection(Direction::South);
				//stack!
				else updatePrevAndReturnDirection(Direction::Stay);
			}

			else if (previousDirection == Direction::East){ //robot was 1 square to the left
				if (isDirectionInList(directions, Direction::East))
					updatePrevAndReturnDirection(Direction::East);
				else if (isDirectionInList(directions, Direction::North))
					updatePrevAndReturnDirection(Direction::North);
				else if (isDirectionInList(directions, Direction::South))
					updatePrevAndReturnDirection(Direction::South);
				//got to dead end
				else if (isDirectionInList(directions, Direction::West))
					updatePrevAndReturnDirection(Direction::West);
				//stack!
				else updatePrevAndReturnDirection(Direction::Stay);
			}

			else if (previousDirection == Direction::West){ //robot was 1 square to the right
				if (isDirectionInList(directions, Direction::West))
					updatePrevAndReturnDirection(Direction::West);
				else if (isDirectionInList(directions, Direction::South))
					updatePrevAndReturnDirection(Direction::South);
				else if (isDirectionInList(directions, Direction::North))
					updatePrevAndReturnDirection(Direction::North);
				//got to dead end
				else if (isDirectionInList(directions, Direction::East))
					updatePrevAndReturnDirection(Direction::East);
				//stack!
				else updatePrevAndReturnDirection(Direction::Stay);
			}

			else { //previousDirrection was Stay
				if (isDirectionInList(directions, Direction::North))
					updatePrevAndReturnDirection(Direction::North);
				else if (isDirectionInList(directions, Direction::South))
					updatePrevAndReturnDirection(Direction::South);
				else if (isDirectionInList(directions, Direction::West))
					updatePrevAndReturnDirection(Direction::West);
				//got to dead end
				else if (isDirectionInList(directions, Direction::East))
					updatePrevAndReturnDirection(Direction::East);
				//stack!
				else updatePrevAndReturnDirection(Direction::Stay);
			}
		}

		updateOutOfDock();
		updateMovesBack();
		updatePosition(returnedDir);
		setCurrValue();
		return returnedDir;
	}
}
