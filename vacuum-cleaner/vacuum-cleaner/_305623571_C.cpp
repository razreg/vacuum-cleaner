#include "_305623571_C.h"

REGISTER_ALGORITHM(_305623571_C)

Direction _305623571_C::chooseSimpleDirection() {

	vector<Direction> directions;
	Position& currPos = getCurrPos();
	vector<vector<char>>& houseMatrix = getHouseMatrix();
	Direction preferNext = getPreferNext();

	if (currPos.getX() > 0 && houseMatrix[currPos.getY()][currPos.getX() - 1] != WALL) {
		if (preferNext == Direction::West) {
			return preferNext;
		}
		directions.push_back(Direction::West);
	}
	if (currPos.getY() > 0 && houseMatrix[currPos.getY() - 1][currPos.getX()] != WALL) {
		if (preferNext == Direction::North) {
			return preferNext;
		}
		directions.push_back(Direction::North);
	}
	if (currPos.getY() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY() + 1][currPos.getX()] != WALL) {
		if (preferNext == Direction::South) {
			return preferNext;
		}
		directions.push_back(Direction::South);
	}
	if (currPos.getX() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY()][currPos.getX() + 1] != WALL) {
		if (preferNext == Direction::East) {
			return preferNext;
		}
		directions.push_back(Direction::East);
	}

	if (directions.empty()) {
		return Direction::Stay;
	}
	Direction opp = getOppositeDirection(preferNext);
	for (Direction& dir : directions) {
		setPreferNext(dir);
		preferNext = dir;
		if (dir != opp) {
			break;
		}
	}
	return preferNext;
}

Direction _305623571_C::chooseSimpleDirectionToBlack() {

	vector<Direction> directions;
	Position& currPos = getCurrPos();
	vector<vector<char>>& houseMatrix = getHouseMatrix();
	Direction preferNext = getPreferNext();

	if (currPos.getX() > 0 && houseMatrix[currPos.getY()][currPos.getX() - 1] == BLACK) {
		if (preferNext == Direction::West) {
			return preferNext;
		}
		directions.push_back(Direction::West);
	}
	if (currPos.getY() > 0 && houseMatrix[currPos.getY() - 1][currPos.getX()] == BLACK) {
		if (preferNext == Direction::North) {
			return preferNext;
		}
		directions.push_back(Direction::North);
	}
	if (currPos.getY() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY() + 1][currPos.getX()] == BLACK) {
		if (preferNext == Direction::South) {
			return preferNext;
		}
		directions.push_back(Direction::South);
	}
	if (currPos.getX() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY()][currPos.getX() + 1] == BLACK) {
		if (preferNext == Direction::East) {
			return preferNext;
		}
		directions.push_back(Direction::East);
	}

	if (directions.empty()) {
		return Direction::Stay;
	}
	Direction opp = getOppositeDirection(preferNext);
	for (Direction& dir : directions) {
		setPreferNext(dir);
		preferNext = dir;
		if (dir != opp) {
			break;
		}
	}
	return preferNext;
}
