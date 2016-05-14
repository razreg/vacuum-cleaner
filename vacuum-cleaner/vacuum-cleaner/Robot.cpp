#include "Robot.h"

Logger Robot::logger = Logger("Robot");

void Robot::step() {

	if (!sensor.inDocking()) {
		battery.consume();
	}

	prevStep = algorithm.step(prevStep);
	//if (logger.debugEnabled()) logger.debug("Algorithm chose step: " + directions[static_cast<int>(prevStep)]);

	if (prevStep == Direction::East) {
		position.moveEast();
	}
	else if (prevStep == Direction::West) {
		position.moveWest();
	}
	else if (prevStep == Direction::South) {
		position.moveSouth();
	}
	else if (prevStep == Direction::North) {
		position.moveNorth();
	}
	sensor.setPosition(position);

	if (sensor.inDocking()) {
		//if (logger.debugEnabled()) logger.debug("Robot in docking station (charging...)");
		battery.charge();
	}
}