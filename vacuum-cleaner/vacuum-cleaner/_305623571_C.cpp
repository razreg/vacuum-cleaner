#include "_305623571_C.h"

REGISTER_ALGORITHM(_305623571_C)

Direction _305623571_C::chooseSimpleDirection() {
	vector<vector<char>>& houseMatrix = getHouseMatrix();
	Position& currPos = getCurrPos();
	incRotate();
	switch (rotatingCounter) {
	case 0:
		if (currPos.getX() < getMaxHouseSize() - 1 
			&& houseMatrix[currPos.getY()][currPos.getX() + 1] != WALL)
			return Direction::East;
	case 1:
		if (currPos.getX() > 0 
			&& houseMatrix[currPos.getY()][currPos.getX() - 1] != WALL) 
			return Direction::West;
	case 2:
		if (currPos.getY() > 0 
			&& houseMatrix[currPos.getY() - 1][currPos.getX()] != WALL)
			return Direction::North;
	case 3:
		if (currPos.getY() < getMaxHouseSize() - 1 
			&& houseMatrix[currPos.getY() + 1][currPos.getX()] != WALL)
			return Direction::South;
	default:
		return Direction::Stay;
	}
}

Direction _305623571_C::chooseSimpleDirectionToBlack() {
	vector<vector<char>>& houseMatrix = getHouseMatrix();
	Position& currPos = getCurrPos();
	incRotate();
	switch (rotatingCounter) {
	case 0:
		if (currPos.getX() < getMaxHouseSize() - 1
			&& houseMatrix[currPos.getY()][currPos.getX() + 1] == BLACK)
			return Direction::East;
	case 1:
		if (currPos.getX() > 0
			&& houseMatrix[currPos.getY()][currPos.getX() - 1] == BLACK)
			return Direction::West;
	case 2:
		if (currPos.getY() > 0
			&& houseMatrix[currPos.getY() - 1][currPos.getX()] == BLACK)
			return Direction::North;
	case 3:
		if (currPos.getY() < getMaxHouseSize() - 1
			&& houseMatrix[currPos.getY() + 1][currPos.getX()] == BLACK)
			return Direction::South;
	default:
		return Direction::Stay;
	}
}