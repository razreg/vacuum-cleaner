#include "_305623571_C.h"

REGISTER_ALGORITHM(_305623571_C)

Direction _305623571_C::chooseSimpleDirection() {

	unsigned int directions = 0;
	Position& currPos = getCurrPos();
	vector<vector<char>>& houseMatrix = getHouseMatrix();
	Direction preferNext = getPreferNext();

	if (currPos.getX() > 0 && houseMatrix[currPos.getY()][currPos.getX() - 1] != WALL) {
		if (preferNext == WEST) {
			return preferNext;
		}
		directions |= 1;
	}
	if (currPos.getY() > 0 && houseMatrix[currPos.getY() - 1][currPos.getX()] != WALL) {
		if (preferNext == NORTH) {
			return preferNext;
		}
		directions |= 2;
	}
	if (currPos.getY() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY() + 1][currPos.getX()] != WALL) {
		if (preferNext == SOUTH) {
			return preferNext;
		}
		directions |= 4;
	}
	if (currPos.getX() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY()][currPos.getX() + 1] != WALL) {
		if (preferNext == EAST) {
			return preferNext;
		}
		directions |= 8;
	}

	return pickPrefered(directions, preferNext);
}

Direction _305623571_C::chooseSimpleDirectionToBlack() {

	unsigned int directions = 0;
	Position& currPos = getCurrPos();
	vector<vector<char>>& houseMatrix = getHouseMatrix();
	Direction preferNext = getPreferNext();

	if (currPos.getX() > 0 && houseMatrix[currPos.getY()][currPos.getX() - 1] == BLACK) {
		if (preferNext == WEST) {
			return preferNext;
		}
		directions |= 1;
	}
	if (currPos.getY() > 0 && houseMatrix[currPos.getY() - 1][currPos.getX()] == BLACK) {
		if (preferNext == NORTH) {
			return preferNext;
		}
		directions |= 2;
	}
	if (currPos.getY() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY() + 1][currPos.getX()] == BLACK) {
		if (preferNext == SOUTH) {
			return preferNext;
		}
		directions |= 4;
	}
	if (currPos.getX() < getMaxHouseSize() - 1 && houseMatrix[currPos.getY()][currPos.getX() + 1] == BLACK) {
		if (preferNext == EAST) {
			return preferNext;
		}
		directions |= 8;
	}

	return pickPrefered(directions, preferNext);
}

Direction _305623571_C::pickPrefered(unsigned int directions, Direction& preferNext) {
	if (directions == 0) {
		return STAY;
	}
	unsigned int opp =
		preferNext == NORTH ? 4 :
		preferNext == SOUTH ? 2 :
		preferNext == WEST ? 8 :
		preferNext == EAST ? 1 : 0;
	bool useOpp;
	if ((useOpp = directions & opp)) {
		directions -= opp;
	}
	if (directions & 1) {
		preferNext = WEST;
	}
	else if (directions & 2) {
		preferNext = NORTH;
	}
	else if (directions & 4) {
		preferNext = SOUTH;
	}
	else if (directions & 8) {
		preferNext = EAST;
	}
	else if (useOpp) {
		preferNext = getOppositeDirection(preferNext);
	}
	else {
		return STAY;
	}
	setPreferNext(preferNext);
	return preferNext;
}
