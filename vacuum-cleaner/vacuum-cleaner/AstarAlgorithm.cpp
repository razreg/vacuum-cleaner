#include "AstarAlgorithm.h"

Direction AstarAlgorithm::step(Direction prevStep) {

	// update currPos with prevStep
	currPos.moveDirection(prevStep);

	// get sensor information
	Direction direction = Direction::Stay; // default
	SensorInformation sensorInformation = sensor->sense();

	// consume battery
	if (!inDockingStation()) {
		battery.consume();
	}

	direction = algorithmIteration(sensorInformation, expectedPrevStep != prevStep);

	// charge battery
	if (inDockingStation()) {
		battery.charge();
	}

	--stepsLeft;

	/* // for debugging the house as seen by the algorithm
	if (battery.getCurrValue() <= 1 || stepsLeft == 0 || (!mappingPhase && houseIsClean())) {
		cout << endl << "House mapped by algorithm:" << endl << endl;
		for (size_t i = 0; i < houseMatrix.size(); ++i) {
			for (size_t j = 0; j < houseMatrix[i].size(); ++j) {
				cout << (houseMatrix[i][j] == '0' ? ' ' : houseMatrix[i][j]);
			}
			cout << endl;
		}
		cout << endl;
	} */

	expectedPrevStep = direction;
	return direction;
}

Direction AstarAlgorithm::algorithmIteration(SensorInformation& sensorInformation, bool restart) {
	
	Direction direction = Direction::Stay;
	if (restart) {
		restartDataForIteration();
		heuristicCostToDocking = NUMERIC_UPPER_BOUND;
	}

	// update houseMatrix according to sensorInformation
	updateWalls(sensorInformation);
	updateDirtLevel(sensorInformation);

	// if charging, stay unless unnecessary
	if (inDockingStation() && battery.getCurrValue() < battery.getCapacity() 
		&& battery.getCurrValue() <= stepsLeft * battery.getConsumptionRate()) {
		restartDataForIteration();
		return direction; // stay
	}

	if (!mappingPhase && houseIsClean()) {
		if (inDockingStation()) {
			return direction; // stay
		}
		followPathToDocking = true; // we can finish now
	}
	// check if we have enough steps to go back to docking
	if (!followPathToDocking || goingToDock == nullptr) {
		if (isReturnTripFeasable(heuristicCostToDocking + 2)) {
			followPathToDocking = false;
		}
		else {
			heuristicCostToDocking = getPathToDocking();
			followPathToDocking = isReturnTripFeasable(heuristicCostToDocking)
				&& !isReturnTripFeasable(heuristicCostToDocking + 2);
		}
		if (!followPathToDocking && keepMoving(sensorInformation)) {
			goingToDock = nullptr; // we're going to make a move so next time need to calculate way to dock again
			if (mappingPhase) {
				// we can keep mapping
				if (goingToGrey == nullptr) {
					if (blackNeighborExists(currPos)) {
						direction = chooseSimpleDirectionToBlack();
					}
					else if (greyExists()) {
						// A* to search for nearest grey
						size_t costToGrey = getPathToGrey();
						if (inDockingStation() && !isReturnTripFeasable((costToGrey + 1) * 2)) {
							// the next black is too far from the docking station
							mappingPhase = false;
							followPathToGrey = false;
							goingToGrey = nullptr;
						}
						else {
							followPathToGrey = true;
						}
					}
					else {
						mappingPhase = false; // finished mapping
						followPathToGrey = false;
						goingToGrey = nullptr;
					}
				}
				else {
					followPathToGrey = true;
				}
				if (followPathToGrey && goingToGrey != nullptr) {
					Position dest = goingToGrey->position;
					goingToGrey = goingToGrey->parent;
					direction = getStepFromPath(dest);
				}
			}
			else {
				// clean the house
				if (goingToDust == nullptr) {
					getPathToDust();
					if (goingToDust == nullptr) {
						direction = chooseSimpleDirection();
					}
				}
				if (goingToDust != nullptr) {
					Position dest = goingToDust->position;
					goingToDust = goingToDust->parent;
					direction = getStepFromPath(dest);
				}
			}
			heuristicCostToDocking++;
		}
	}
	if (followPathToDocking && goingToDock != nullptr) {
		goingToDust = nullptr;
		goingToGrey = nullptr;
		Position dest = goingToDock->position;
		goingToDock = goingToDock->parent;
		direction = getStepFromPath(dest);
		heuristicCostToDocking--;
	}
	return direction;
}

void AstarAlgorithm::restartDataForIteration() {
	followPathToDocking = false;
	followPathToGrey = false;
	goingToDust = nullptr;
	goingToGrey = nullptr;
	goingToDock = nullptr;
}

void AstarAlgorithm::DataPool::updateNode(shared_ptr<Node> node, Position& position, 
	shared_ptr<Node> parent, int realCost, int heuristicCost) {
	node->position = position;
	node->realCost = realCost;
	node->heuristicCost = heuristicCost;
	node->parent = parent;
}

void AstarAlgorithm::DataPool::add(Position position, shared_ptr<Node> parent, int realCost, int heuristicCost) {
	size_t idx = 0;
	while (idx < poolObjects.size()) {
		if (poolObjects[idx].node->position == position) {
			if (realCost + heuristicCost < poolObjects[idx].node->realCost + poolObjects[idx].node->heuristicCost) {
				updateNode(poolObjects[idx].node, position, parent, realCost, heuristicCost);
			}
			return;
		}
		idx++;
	}
	poolObjects.push_back(PoolObject());
	poolObjects[idx].inFringe = true;
	poolObjects[idx].node = make_shared<Node>();
	updateNode(poolObjects[idx].node, position, parent, realCost, heuristicCost);
}

shared_ptr<AstarAlgorithm::Node> AstarAlgorithm::DataPool::getBestNode() {

	int bestIdx = -1;
	int cost = numeric_limits<int>::max();
	shared_ptr<Node> node;

	for (size_t i = 0; i < poolObjects.size(); ++i) {
		node = poolObjects[i].node;
		if (poolObjects[i].inFringe && ((node->heuristicCost + node->realCost) < cost)) {
			bestIdx = i;
			cost = node->realCost + node->heuristicCost;
		}
	}
	if (bestIdx == -1) {
		return nullptr;
	}
	poolObjects[bestIdx].inFringe = false;
	return poolObjects[bestIdx].node;
}

void AstarAlgorithm::initHouseMatrix() {
	houseMatrix.clear();
	vector<char> row;
	for (size_t i = 0; i < maxHouseSize; ++i) {
		row.push_back(BLACK);
	}
	for (size_t i = 0; i < maxHouseSize; ++i) {
		houseMatrix.push_back(row);
	}
	size_t center = (maxHouseSize + 1) / 2;
	currPos = Position(center, center);
	updateMatrix(currPos, DOCK);
	docking = currPos;
}

void AstarAlgorithm::updateWalls(SensorInformation& sensorInformation) {
	if (sensorInformation.isWall[NORTH_IDX] && currPos.getY() > 0) {
		houseMatrix[currPos.getY() - 1][currPos.getX()] = WALL;
	}
	if (sensorInformation.isWall[SOUTH_IDX] && currPos.getY() < maxHouseSize - 1) {
		houseMatrix[currPos.getY() + 1][currPos.getX()] = WALL;
	}
	if (sensorInformation.isWall[EAST_IDX] && currPos.getX() < maxHouseSize - 1) {
		houseMatrix[currPos.getY()][currPos.getX() + 1] = WALL;
	}
	if (sensorInformation.isWall[WEST_IDX] && currPos.getX() > 0) {
		houseMatrix[currPos.getY()][currPos.getX() - 1] = WALL;
	}
}

bool AstarAlgorithm::greyExists() const {
	for (size_t row = 0; row < maxHouseSize; ++row) {
		for (size_t col = 0; col < maxHouseSize; ++col) {
			Position temp = Position(col, row);
			if (houseMatrix[row][col] != BLACK && houseMatrix[row][col] != WALL 
				&& blackNeighborExists(temp)) {
				return true;
			}
		}
	}
	return false;
}

bool AstarAlgorithm::houseIsClean() const {
	for (size_t row = 0; row < maxHouseSize; ++row) {
		for (size_t col = 0; col < maxHouseSize; ++col) {
			if ('0' < houseMatrix[row][col] && houseMatrix[row][col] <= '9') {
				return false;
			}
		}
	}
	return true;
}

bool AstarAlgorithm::blackNeighborExists(Position& pos) const {
	return
		(pos.getX() > 0 && houseMatrix[pos.getY()][pos.getX() - 1] == BLACK) ||
		(pos.getX() < maxHouseSize - 1 && houseMatrix[pos.getY()][pos.getX() + 1] == BLACK) ||
		(pos.getY() > 0 && houseMatrix[pos.getY() - 1][pos.getX()] == BLACK) ||
		(pos.getY() < maxHouseSize - 1 && houseMatrix[pos.getY() + 1][pos.getX()] == BLACK);
}

bool AstarAlgorithm::allBlack(Position& pos) const {
	char c;
	return
		((c = houseMatrix[pos.getY()][pos.getX()]) == BLACK || c == WALL) &&
		(pos.getX() == 0 || (c = houseMatrix[pos.getY()][pos.getX() - 1]) == BLACK || c == WALL) &&
		(pos.getX() == maxHouseSize - 1 || (c = houseMatrix[pos.getY()][pos.getX() + 1]) == BLACK || c == WALL) &&
		(pos.getY() == 0 || (c = houseMatrix[pos.getY() - 1][pos.getX()]) == BLACK || c == WALL) &&
		(pos.getY() == maxHouseSize - 1 || (c = houseMatrix[pos.getY() + 1][pos.getX()]) == BLACK || c == WALL);
}

Direction AstarAlgorithm::getStepFromPath(Position& dest) const {
	return
		dest.getY() < currPos.getY() ? Direction::North :
		dest.getX() < currPos.getX() ? Direction::West :
		dest.getY() > currPos.getY() ? Direction::South :
		dest.getX() > currPos.getX() ? Direction::East :
		Direction::Stay;
}

Position AstarAlgorithm::getNearestGrey(Position& pos) const {
	size_t distance = NUMERIC_UPPER_BOUND;
	Position nearest = pos;
	for (size_t row = 0; row < maxHouseSize; ++row) {
		for (size_t col = 0; col < maxHouseSize; ++col) {
			Position temp = Position(col, row);
			if (houseMatrix[row][col] != BLACK && blackNeighborExists(temp)) {
				size_t newDistance = getDistance(pos, temp);
				if (distance > newDistance) {
					distance = newDistance;
					nearest = temp;
				}
			}
		}
	}
	return nearest;
}

Position AstarAlgorithm::getNearestDust(Position& pos) const {
	size_t distance = NUMERIC_UPPER_BOUND;
	Position nearest = pos;
	for (size_t row = 0; row < maxHouseSize; ++row) {
		for (size_t col = 0; col < maxHouseSize; ++col) {
			Position temp = Position(col, row);
			if (houseMatrix[pos.getY()][pos.getX()] <= '9' && houseMatrix[pos.getY()][pos.getX()] > '0') {
				size_t newDistance = getDistance(pos, temp);
				if (distance > newDistance) {
					distance = newDistance;
					nearest = temp;
				}
			}
		}
	}
	return nearest;
}

size_t AstarAlgorithm::getPathToGrey() {
	goingToGrey = nullptr;
	mappingDataPool.clear();
	mappingDataPool.add(currPos, nullptr, 0, getDistance(currPos, getNearestGrey(currPos)));
	shared_ptr<Node> bestNode = mappingDataPool.getBestNode();
	if (bestNode == nullptr) {
		return NUMERIC_UPPER_BOUND;
	}
	Position pos = bestNode->position;
	Position childPos;
	while (!blackNeighborExists(pos)) {
		addNeighborToMappingDataPool(pos, pos.getX() > 0, Position(pos.getX() - 1, pos.getY()), bestNode);
		addNeighborToMappingDataPool(pos, pos.getX() < maxHouseSize - 1, Position(pos.getX() + 1, pos.getY()), bestNode);
		addNeighborToMappingDataPool(pos, pos.getY() < maxHouseSize - 1, Position(pos.getX(), pos.getY() + 1), bestNode);
		addNeighborToMappingDataPool(pos, pos.getY() > 0, Position(pos.getX(), pos.getY() - 1), bestNode);
		bestNode = mappingDataPool.getBestNode();
		if (bestNode == nullptr) {
			return NUMERIC_UPPER_BOUND;
		}
		pos = bestNode->position;
	}
	// reverse path
	size_t pathLength = 1;
	shared_ptr<Node> temp1 = bestNode;
	if (bestNode->parent != nullptr) {
		shared_ptr<Node> temp2 = bestNode->parent;
		shared_ptr<Node> temp3;
		while (temp2->parent != nullptr) {
			temp3 = temp2->parent;
			temp2->parent = temp1;
			temp1 = temp2;
			temp2 = temp3;
			pathLength++;
		}
		temp2->parent = temp1;
		bestNode->parent = nullptr;
	}
	goingToGrey = temp1;
	return pathLength;
}

void AstarAlgorithm::getPathToDust() {
	goingToDust = nullptr;
	dustDataPool.clear();
	dustDataPool.add(currPos, nullptr, 0, getDistance(currPos, getNearestDust(currPos)));
	shared_ptr<Node> bestNode = dustDataPool.getBestNode();
	if (bestNode == nullptr) {
		return;
	}
	Position pos = bestNode->position;
	Position childPos;

	// while there is no dust in pos
	while (houseMatrix[pos.getY()][pos.getX()] > '9' || houseMatrix[pos.getY()][pos.getX()] <= '0') {
		addNeighborToDustDataPool(pos, pos.getX() > 0, Position(pos.getX() - 1, pos.getY()), bestNode);
		addNeighborToDustDataPool(pos, pos.getX() < maxHouseSize - 1, Position(pos.getX() + 1, pos.getY()), bestNode);
		addNeighborToDustDataPool(pos, pos.getY() < maxHouseSize - 1, Position(pos.getX(), pos.getY() + 1), bestNode);
		addNeighborToDustDataPool(pos, pos.getY() > 0, Position(pos.getX(), pos.getY() - 1), bestNode);
		bestNode = dustDataPool.getBestNode();
		if (bestNode == nullptr) {
			return;
		}
		pos = bestNode->position;
	}

	// reverse path
	shared_ptr<Node> temp1 = bestNode;
	if (bestNode->parent != nullptr) {
		shared_ptr<Node> temp2 = bestNode->parent;
		shared_ptr<Node> temp3;
		while (temp2->parent != nullptr) {
			temp3 = temp2->parent;
			temp2->parent = temp1;
			temp1 = temp2;
			temp2 = temp3;
		}
		temp2->parent = temp1;
		bestNode->parent = nullptr;
	}
	goingToDust = temp1;
}

size_t AstarAlgorithm::getPathToDocking() {
	goingToDock = nullptr;
	dockingDataPool.clear();
	dockingDataPool.add(currPos, nullptr, 0, getDistance(currPos, docking));
	shared_ptr<Node> bestNode = dockingDataPool.getBestNode();
	if (bestNode == nullptr) {
		return NUMERIC_UPPER_BOUND;
	}
	Position pos = bestNode->position;
	Position childPos;

	while (pos != docking) {
		addNeighborToDockingDataPool(pos, pos.getX() > 0, Position(pos.getX() - 1, pos.getY()), bestNode);
		addNeighborToDockingDataPool(pos, pos.getX() < maxHouseSize - 1, Position(pos.getX() + 1, pos.getY()), bestNode);
		addNeighborToDockingDataPool(pos, pos.getY() < maxHouseSize - 1, Position(pos.getX(), pos.getY() + 1), bestNode);
		addNeighborToDockingDataPool(pos, pos.getY() > 0, Position(pos.getX(), pos.getY() - 1), bestNode);
		bestNode = dockingDataPool.getBestNode();
		if (bestNode == nullptr) {
			return NUMERIC_UPPER_BOUND;
		}
		pos = bestNode->position;
	}
	// reverse path
	size_t pathLength = 1;
	shared_ptr<Node> temp1 = bestNode;
	if (bestNode->parent != nullptr) {
		shared_ptr<Node> temp2 = bestNode->parent;
		shared_ptr<Node> temp3;
		while (temp2->parent != nullptr) {
			temp3 = temp2->parent;
			temp2->parent = temp1;
			temp1 = temp2;
			temp2 = temp3;
			pathLength++;
		}
		temp2->parent = temp1;
		bestNode->parent = nullptr;
	}
	goingToDock = temp1;
	return pathLength;
}

void AstarAlgorithm::addNeighborToMappingDataPool(Position pos, bool pred, Position childPos, shared_ptr<Node> bestNode) {
	if (pred) {
		if (houseMatrix[childPos.getY()][childPos.getX()] != WALL) {
			mappingDataPool.add(childPos, bestNode, bestNode->realCost + 1, getDistance(childPos, getNearestGrey(childPos)));
		}
	}
}

void AstarAlgorithm::addNeighborToDustDataPool(Position pos, bool pred, Position childPos, shared_ptr<Node> bestNode) {
	if (pred) {
		if (houseMatrix[childPos.getY()][childPos.getX()] != WALL) {
			dustDataPool.add(childPos, bestNode, bestNode->realCost + 1, getDistance(childPos, getNearestDust(childPos)));
		}
	}
}

void AstarAlgorithm::addNeighborToDockingDataPool(Position pos, bool pred, Position childPos, shared_ptr<Node> bestNode) {
	if (pred) {
		if (houseMatrix[childPos.getY()][childPos.getX()] != WALL && houseMatrix[childPos.getY()][childPos.getX()] != BLACK) {
			dockingDataPool.add(childPos, bestNode, bestNode->realCost + 1, getDistance(childPos, docking));
		}
	}
}

void AstarAlgorithm::setConfiguration(map<string, int> config) {
	battery.setCapacity(max(0, config.find(BATTERY_CAPACITY)->second));
	battery.setCurrValue(battery.getCapacity());
	battery.setConsumptionRate(max(0, config.find(BATTERY_CONSUMPTION_RATE)->second));
	battery.setRechargeRate(max(0, config.find(BATTERY_RECHARGE_RATE)->second));
	maxHouseSize = HOUSE_SIZE_UPPER_BOUND;
	if (battery.getConsumptionRate() >= 1) {
		maxHouseSize = battery.getCapacity() / battery.getConsumptionRate();
		maxHouseSize += 1 - maxHouseSize % 2; // make odd so docking station would be in the center
	}
	maxHouseSize = min(maxHouseSize, HOUSE_SIZE_UPPER_BOUND);
	initHouseMatrix();
	configured = true;
}

void AstarAlgorithm::updateDirtLevel(SensorInformation& sensorInformation) {
	if (!inDockingStation()) {
		int dirtLevel = sensorInformation.dirtLevel;
		houseMatrix[currPos.getY()][currPos.getX()] = '0' + dirtLevel;
	}
}

void AstarAlgorithm::restartAlgorithm() {
	if (configured) {
		battery.setCurrValue(battery.getCapacity());
	}
	stepsLeft = NUMERIC_UPPER_BOUND;
	heuristicCostToDocking = NUMERIC_UPPER_BOUND;
	initHouseMatrix();
	mappingPhase = true;
	mappingDataPool.clear();
	dockingDataPool.clear();
	dustDataPool.clear();
	expectedPrevStep = Direction::Stay;
	followPathToDocking = false;
	followPathToGrey = false;
	goingToDock = nullptr;
	goingToDust = nullptr;
	goingToGrey = nullptr;
	resetPreferNext();
}

Direction AstarAlgorithm::getOppositeDirection(Direction direction) {
	return 
		direction == Direction::North ? Direction::South :
		direction == Direction::South ? Direction::North :
		direction == Direction::West ? Direction::East :
		direction == Direction::East ? Direction::West :
		Direction::Stay;
}

Direction AstarAlgorithm::chooseSimpleDirection() {

	vector<Direction> directions;

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
	if (currPos.getX() > 0 && houseMatrix[currPos.getY()][currPos.getX() - 1] != WALL) {
		if (preferNext == Direction::West) {
			return preferNext;
		}
		directions.push_back(Direction::West);
	}

	if (directions.empty()) {
		return Direction::Stay;
	}
	Direction opp = getOppositeDirection(preferNext);
	for (Direction& dir : directions) {
		preferNext = dir;
		if (dir != opp) {
			break;
		}
	}
	return preferNext;
}

Direction AstarAlgorithm::chooseSimpleDirectionToBlack() {

	vector<Direction> directions;

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
	if (currPos.getX() > 0 && houseMatrix[currPos.getY()][currPos.getX() - 1] == BLACK) {
		if (preferNext == Direction::West) {
			return preferNext;
		}
		directions.push_back(Direction::West);
	}

	if (directions.empty()) {
		return Direction::Stay;
	}
	Direction opp = getOppositeDirection(preferNext);
	for (Direction& dir : directions) {
		preferNext = dir;
		if (dir != opp) {
			break;
		}
	}
	return preferNext;
}