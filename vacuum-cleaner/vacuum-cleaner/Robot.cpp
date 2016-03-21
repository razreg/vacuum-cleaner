#include "Robot.h"

Logger Robot::logger = Logger("Robot");

void Robot::step() {

	Direction direction = algorithm.step();
	logger.info("Algorithm chose to perform step: " + directions[static_cast<int>(direction)]);

	if (direction == Direction::East) {
		position.X++;
	}
	else if (direction == Direction::West) {
		--position.X;
	} else if (direction == Direction::South) {
		position.Y++;
	}
	else if (direction == Direction::North) {
		--position.Y;
	}
	sensor.setPosition(position);
	if (sensor.inDocking()) {
		logger.info("Robot in docking station (charging...)");
		battery.charge();
	}
	else {
		battery.consume();
	}
}