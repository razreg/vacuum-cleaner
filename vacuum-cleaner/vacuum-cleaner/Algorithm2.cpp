#include "Algorithm2.h"

Logger Algorithm2::logger = Logger("Algorithm1");
Direction Algorithm2::previousDirection = Direction::Stay;

void Algorithm2::setConfiguration(map<string, int> config) {
	maxStepsAfterWinner = config.find(MAX_STEPS_AFTER_WINNER)->second;

}

Direction Algorithm2::step() {

	SensorInformation sensorInformation = sensor->sense();

	// construct a vector of valid directions
	vector<Direction> directions;
	directions.push_back(Direction::Stay);
	for (int i = 0; i < static_cast<int>(Direction::Stay); ++i) {
		if (!sensorInformation.isWall[i]) {
			directions.push_back(static_cast<Direction>(i));
		}
	}

	if (sensorInformation.dirtLevel > 1){
		previousDirection = Direction::Stay;
		return Direction::Stay;
	}


	if (previousDirection == Direction::South) { //robot was 1 square above
		if (isPossibleDirection(directions, Direction::South)){
			return Direction::South; //no need to update previousDirection
		}
		else if (isPossibleDirection(directions, Direction::East)){
			previousDirection = Direction::East;
			return Direction::East;
		}
		//got to corner
		else if (isPossibleDirection(directions, Direction::West)){
			previousDirection = Direction::West;
			return Direction::West;
		}
		//got to dead end
		else if (isPossibleDirection(directions, Direction::North)){
			previousDirection = Direction::North;
			return Direction::North;
		}
		//stack!
		else{
			previousDirection = Direction::Stay;
			return Direction::Stay;
		}
	}

	else if (previousDirection == Direction::North){//robot was 1 square below
		if (isPossibleDirection(directions, Direction::North)){
			return Direction::North; //no need to update previousDirection
		}
		else if (isPossibleDirection(directions, Direction::East)){
			previousDirection = Direction::East;
			return Direction::East;
		}
		//got to corner
		else if (isPossibleDirection(directions, Direction::West)){
			previousDirection = Direction::West;
			return Direction::West;
		}
		//got to dead end
		else if (isPossibleDirection(directions, Direction::South)){
			previousDirection = Direction::South;
			return Direction::South;
		}
		//stack!
		else{
			previousDirection = Direction::Stay;
			return Direction::Stay;
		}
	}

	else if (previousDirection == Direction::East){//robot was 1 square to the left
		if (isPossibleDirection(directions, Direction::East)){
			return Direction::East; //no need to update previousDirection
		}
		else if (isPossibleDirection(directions, Direction::North)){
			previousDirection = Direction::North;
			return Direction::North;
		}
		else if (isPossibleDirection(directions, Direction::South)){
			previousDirection = Direction::South;
			return Direction::South;
		}
		//got to dead end
		else if (isPossibleDirection(directions, Direction::West)){
			previousDirection = Direction::West;
			return Direction::West;
		}
		//stack!
		else{
			previousDirection = Direction::Stay;
			return Direction::Stay;
		}
	}

	else if (previousDirection == Direction::West){//robot was 1 square to the right
		if (isPossibleDirection(directions, Direction::West)){
			return Direction::West; //no need to update previousDirection
		}
		else if (isPossibleDirection(directions, Direction::South)){
			previousDirection = Direction::South;
			return Direction::South;
		}
		else if (isPossibleDirection(directions, Direction::North)){
			previousDirection = Direction::North;
			return Direction::North;
		}
		//got to dead end
		else if (isPossibleDirection(directions, Direction::East)){
			previousDirection = Direction::East;
			return Direction::East;
		}
		//stack!
		else{
			previousDirection = Direction::Stay;
			return Direction::Stay;
		}
	}

	else{ //previousDirrection was Stay
		if (isPossibleDirection(directions, Direction::North)){
			previousDirection = Direction::North;
			return Direction::North;
		}
		else if (isPossibleDirection(directions, Direction::South)){
			previousDirection = Direction::South;
			return Direction::South;
		}
		else if (isPossibleDirection(directions, Direction::West)){
			previousDirection = Direction::West;
			return Direction::West;
		}
		//got to dead end
		else if (isPossibleDirection(directions, Direction::East)){
			previousDirection = Direction::East;
			return Direction::East;
		}
		//stack!
		else{
			return Direction::Stay; //no need to update previousDirection
		}
	}

}

bool Algorithm2::isPossibleDirection(vector<Direction> directions, Direction direction){
	vector<Direction>::iterator it;
	it = find(directions.begin(), directions.end(), direction);
	return (it != directions.end());
}
