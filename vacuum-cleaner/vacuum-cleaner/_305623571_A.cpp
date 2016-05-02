#include "_305623571_A.h"

REGISTER_ALGORITHM(_305623571_A)

/**
 * Return trip scheme is to follow the reverse trip.
 * In order to prevent the algorithm from getting back to the docking station to charge 
 * and then take the same path as before, we keep a cyclic counter so it will have some 
 * freedom to choose where to go.
 * When in docking station charge till full.
 */
Direction _305623571_A::step(Direction prevStep) {

	Direction direction = Direction::Stay; // default
	SensorInformation sensorInformation = sensor->sense();

	// consume battery
	if (!inDockingStation()) {
		battery.consume();
	}

	// if the robot can return after next step or can't return now - keep cleaning
	if (isReturnTripFeasable(movesBack.size() + 2) || !isReturnTripFeasable(movesBack.size())) {
		int stayIndex = static_cast<int>(Direction::Stay);
		if (inDockingStation()) {
			// stay in docking station until battery is full and don't travel if can't come back in time
			if (battery.full() && isReturnTripFeasable(movesBack.size() + 2)) {
				int i = 0;
				do {
					directionCounter = (directionCounter + 1) % stayIndex;
				} while (sensorInformation.isWall[directionCounter] && i++ < stayIndex);
				if (!sensorInformation.isWall[directionCounter]) {
					direction = static_cast<Direction>(directionCounter);
					lastDirection = direction;
					storeDataForReturnTrip(direction);
				}
			}
		}
		else if (sensorInformation.dirtLevel <= 1) {
			// prefer to stay on your track
			if (!sensorInformation.isWall[static_cast<int>(lastDirection)]) {
				direction = lastDirection;
				storeDataForReturnTrip(direction);
			}
			else {
				for (int i : directionsPermutation) {
					int index = (directionCounter + i) % stayIndex;
					if (!sensorInformation.isWall[index]) {
						direction = static_cast<Direction>(index);
						lastDirection = direction;
						storeDataForReturnTrip(direction);
						break;
					}
				}
			}
		}
	}
	else if (!movesBack.empty()) { // robot must return now to get to docking
		direction = movesBack.back();
		movesBack.pop_back();
	}

	// charge battery
	if (inDockingStation()) {
		battery.charge();
	}

	--stepsLeft;
	return direction;
}
