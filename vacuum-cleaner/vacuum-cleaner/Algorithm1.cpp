#include "Algorithm1.h"

Logger Algorithm1::logger = Logger("Algorithm1");
Direction Algorithm1::previousDirection = Direction::Stay;

void Algorithm1::setConfiguration(map<string, int> config) {
	maxStepsAfterWinner = config.find(MAX_STEPS_AFTER_WINNER)->second;

}

Direction Algorithm1::step() {

	SensorInformation sensorInformation = sensor->sense();

	// construct a vector of valid directions
	vector<Direction> directions;
	directions.push_back(Direction::Stay);
	for (int i = 0; i < static_cast<int>(Direction::Stay); ++i) {
		if (!sensorInformation.isWall[i]) {
			directions.push_back(static_cast<Direction>(i));
		}
	}

	/* //Algorithm 2

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
	
	*/

	//Algorithm 1
	//the order of dorections in vector "directions" is: STAY, EAST, WEST, SOUTH and NORTH.
	//if the robot can't move east than move west, and if not west than move south, and if not south than move north
	//and if not north than stay.

	if (directions.size() == 1 || sensorInformation.dirtLevel > 1)
		return directions[0];
	else if (directions.size() == 2)
		return directions[1];
	else if (directions.size() == 3)
		return directions[2];
	else if (directions.size() == 4)
		return directions[3];
	else return directions[4];

	/*
	if (directions.size() > 1){
		cout << "***" << to_string(static_cast<int>(directions[1])) << "***" << endl;
		return directions[1];
	}
		
	return directions[0];
	*/

}

bool Algorithm1::isPossibleDirection(vector<Direction> directions, Direction direction){
	vector<Direction>::iterator it;
	it = find(directions.begin(), directions.end(), direction);
	return (it != directions.end());
}
