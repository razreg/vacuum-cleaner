#include "ManhattenAlgorithm.h"

/**
* Return trip scheme is according to manhatten distance
* In order to prevent the algorithm from getting back to the docking station to charge
* and then take the same path as before, we keep a cyclic counter so it will have some
* freedom to choose where to go.
* When in docking station charge till full.
*/
Direction ManhattenAlgorithm::step(Direction prevStep) {

	Direction direction = Direction::Stay; // default
	SensorInformation sensorInformation = sensor->sense();

	// consume battery
	if (!inDockingStation()) {
		battery.consume();
	}

	// if the robot can return after next step or can't return now - keep cleaning
	// 4 is a precaution to allow getting back to docking even if there are walls in the way
	if (stepsToBypassWall > 0 || isReturnTripFeasable(abs(xPos) + abs(yPos) + 2 + MAX_BYPASS)
		|| !isReturnTripFeasable(abs(xPos) + abs(yPos) + stepsToBypassWall)) {
		int stayIndex = static_cast<int>(Direction::Stay);
		if (inDockingStation()) {
			// stay in docking station until battery is full and don't travel if can't come back in time
			if (battery.full() && isReturnTripFeasable(abs(xPos) + abs(yPos) + 2)) {
				int i = 0;
				do {
					directionCounter = (directionCounter + 1) % stayIndex;
				} while (sensorInformation.isWall[directionCounter] && i++ < stayIndex);
				if (!sensorInformation.isWall[directionCounter]) {
					direction = static_cast<Direction>(directionCounter);
					lastDirection = direction;
				}
			}
		}
		else if (isReadyToMoveOn(sensorInformation)) {
			direction = keepCleaningOutOfDocking(sensorInformation);
		}
	}
	else if (!inDockingStation()) { // robot must return now to get to docking
		direction = returnToDocking(sensorInformation);
	}

	// charge battery
	if (inDockingStation()) {
		battery.charge();
	}

	--stepsLeft;
	if (stepsToBypassWall > 0) {
		--stepsToBypassWall;
	}
	storeDataForReturnTrip(direction);
	return direction;
}

Direction ManhattenAlgorithm::keepCleaningOutOfDocking(SensorInformation& sensorInformation) {

	Direction direction = Direction::Stay;
	// prefer to stay on your track
	if (!sensorInformation.isWall[static_cast<int>(lastDirection)]) {
		direction = lastDirection;
	}
	else {
		int stayIndex = static_cast<int>(Direction::Stay);
		for (int i : directionsPermutation) {
			int index = (directionCounter + i) % stayIndex;
			if (!sensorInformation.isWall[index]) {
				direction = static_cast<Direction>(index);
				lastDirection = direction;
				break;
			}
		}
	}
	return direction;
}

Direction ManhattenAlgorithm::returnToDocking(SensorInformation& sensorInformation) {
	Direction direction = Direction::Stay;
	if (!inDockingStation()) {

		if (xPos < 0 && !sensorInformation.isWall[static_cast<int>(Direction::East)]) {
			direction = Direction::East;
		}
		else if (xPos > 0 && !sensorInformation.isWall[static_cast<int>(Direction::West)]) {
			direction = Direction::West;
		}
		else if (yPos < 0 && !sensorInformation.isWall[static_cast<int>(Direction::South)]) {
			direction = Direction::South;
		}
		else if (yPos > 0 && !sensorInformation.isWall[static_cast<int>(Direction::North)]) {
			direction = Direction::North;
		}
		else {
			stepsToBypassWall = MAX_BYPASS;
			direction = keepCleaningOutOfDocking(sensorInformation);
		}
	}
	return direction;
}
