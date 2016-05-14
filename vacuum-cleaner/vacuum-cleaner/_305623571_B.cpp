#include "_305623571_B.h"

REGISTER_ALGORITHM(_305623571_B)

Direction _305623571_B::chooseSimpleDirection() {
	vector<vector<char>>& houseMatrix = getHouseMatrix();
	Position& currPos = getCurrPos();
	return
		(currPos.getX() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY()][currPos.getX() + 1] != WALL) ? Direction::East :
		(currPos.getY() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY() + 1][currPos.getX()] != WALL) ? Direction::South :
		(currPos.getX() > 0 && houseMatrix[currPos.getY()][currPos.getX() - 1] != WALL) ? Direction::West :
		(currPos.getY() > 0 && houseMatrix[currPos.getY() - 1][currPos.getX()] != WALL) ? Direction::North :
		Direction::Stay;
}

Direction _305623571_B::chooseSimpleDirectionToBlack() {
	vector<vector<char>>& houseMatrix = getHouseMatrix();
	Position& currPos = getCurrPos();
	return
		(currPos.getX() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY()][currPos.getX() + 1] == BLACK) ? Direction::East :
		(currPos.getY() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY() + 1][currPos.getX()] == BLACK) ? Direction::South :
		(currPos.getX() > 0 && houseMatrix[currPos.getY()][currPos.getX() - 1] == BLACK) ? Direction::West :
		(currPos.getY() > 0 && houseMatrix[currPos.getY() - 1][currPos.getX()] == BLACK) ? Direction::North :
		Direction::Stay;
}