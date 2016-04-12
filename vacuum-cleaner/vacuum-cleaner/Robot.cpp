#include "Robot.h"

Logger Robot::logger = Logger("Robot");

void Robot::step(House& house) {

	if (!sensor.inDocking()) {
		battery.consume();
	}

	cout << battery.getCurrValue() << endl;
	cout << (int)house.getMovesBack().size() << endl;

	if (house.getMovesBack().size() >0 && battery.getCurrValue() <= ((int)house.getMovesBack().size())* battery.getConsumptionRate())
	{
		Direction direction = house.getMovesBack().back();
		switch (direction)  {
			case (Direction::North) : 
				position.moveNorth();
				break;
			case (Direction::South) :
				position.moveSouth();
				break;
			case (Direction::East) :
				position.moveEast();
				break;
			case (Direction::West) :
				position.moveWest();
				break;
			default:
				break;
		}	
		house.popMovesBack();
	}

	else {
		Direction direction = algorithm.step();
		//if (logger.debugEnabled()) logger.debug("Algorithm chose step: " + directions[static_cast<int>(direction)]);

		if (direction == Direction::East) {
			house.pushMovesBack(Direction::West);
			position.moveEast();
		}
		else if (direction == Direction::West) {
			house.pushMovesBack(Direction::East);
			position.moveWest();
		}
		else if (direction == Direction::South) {
			house.pushMovesBack(Direction::North);
			position.moveSouth();
		}
		else if (direction == Direction::North) {
			house.pushMovesBack(Direction::South);
			position.moveNorth();
		}
	}
	
	sensor.setPosition(position);

	if (sensor.inDocking()) {
		//if (logger.debugEnabled()) logger.debug("Robot in docking station (charging...)");
		battery.charge();
		house.eraseMovesBack(); 
	}
}