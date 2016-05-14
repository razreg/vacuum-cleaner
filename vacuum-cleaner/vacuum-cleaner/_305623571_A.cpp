#include "_305623571_A.h"

REGISTER_ALGORITHM(_305623571_A)

Direction _305623571_A::chooseSimpleDirection() {
	vector<vector<char>>& houseMatrix = getHouseMatrix();
	Position& currPos = getCurrPos();
	return
		(currPos.getY() > 0 && houseMatrix[currPos.getY() - 1][currPos.getX()] != WALL) ? Direction::North :
		(currPos.getY() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY() + 1][currPos.getX()] != WALL) ? Direction::South :
		(currPos.getX() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY()][currPos.getX() + 1] != WALL) ? Direction::East :
		(currPos.getX() > 0 && houseMatrix[currPos.getY()][currPos.getX() - 1] != WALL) ? Direction::West :
		Direction::Stay;
}

Direction _305623571_A::chooseSimpleDirectionToBlack() {
	vector<vector<char>>& houseMatrix = getHouseMatrix();
	Position& currPos = getCurrPos();
	return
		(currPos.getY() > 0 && houseMatrix[currPos.getY() - 1][currPos.getX()] == BLACK) ? Direction::North :
		(currPos.getY() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY() + 1][currPos.getX()] == BLACK) ? Direction::South :
		(currPos.getX() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY()][currPos.getX() + 1] == BLACK) ? Direction::East :
		(currPos.getX() > 0 && houseMatrix[currPos.getY()][currPos.getX() - 1] == BLACK) ? Direction::West :
		Direction::Stay;
}