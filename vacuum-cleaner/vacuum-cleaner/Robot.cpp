#include "Robot.h"

void Robot::step() {

	Direction direction = algorithm.step();

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
		battery.charge();
	}
	else {
		battery.consume();
	}
}