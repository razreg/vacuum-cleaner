#include "Robot.h"

Logger Robot::logger = Logger("Robot");

void Robot::step() {

	Direction direction = algorithm.step();
	//logger.debug("Algorithm chose step: " + directions[static_cast<int>(direction)]);

	if (direction == Direction::East) {
		position.moveEast();
	}
	else if (direction == Direction::West) {
		position.moveWest();
	} else if (direction == Direction::South) {
		position.moveSouth();
	}
	else if (direction == Direction::North) {
		position.moveNorth();
	}
	sensor.setPosition(position);
	if (sensor.inDocking()) {
		//logger.debug("Robot in docking station (charging...)");
		battery.charge();
	}
	else {
		battery.consume();
	}
}