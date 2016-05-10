#ifndef __ASTAR_ALGORITHM__H_
#define __ASTAR_ALGORITHM__H_

#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Battery.h"
#include "Position.h"
#include "Common.h"
#include "uniqueptr.h"
#include "AlgorithmRegistration.h"

using namespace std;

const char BLACK = 'B';
const char DOCK = 'D';
const char WALL = 'W';

const size_t HOUSE_SIZE_UPPER_BOUND = 201; // TODO decide on bound

const int NORTH_IDX = static_cast<int>(Direction::North);
const int EAST_IDX = static_cast<int>(Direction::East);
const int WEST_IDX = static_cast<int>(Direction::West);
const int SOUTH_IDX = static_cast<int>(Direction::South);

class AstarAlgorithm : public AbstractAlgorithm {

	class Node {
	public:
		Position position;
		shared_ptr<Node> parent = nullptr;
		int realCost = 0;
		int heuristicCost = 0;

		Node() {};

		Node(Position position, int realCost, int heuristicCost, shared_ptr<Node> parentNode) :
			position(position), parent(parentNode), realCost(realCost), heuristicCost(heuristicCost) {};
	};

	class PoolObject {
	public:
		bool inFringe = false;
		shared_ptr<Node> node;
	};

	class DataPool {
		vector<PoolObject> poolObjects;
	public:

		bool empty() {
			return poolObjects.empty();
		};

		size_t size() {
			return poolObjects.size();
		};

		void add(Position position, shared_ptr<Node> parent, int realCost, int heuristicCost) {
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
		};

		void updateNode(shared_ptr<Node> node, Position& position, shared_ptr<Node> parent, int realCost, int heuristicCost) {
			node->position = position;
			node->realCost = realCost;
			node->heuristicCost = heuristicCost;
			node->parent = parent;
		};

		void clear() {
			poolObjects.clear();
		};

		shared_ptr<Node> getBestNode() {

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
		};
	};

	const AbstractSensor* sensor;
	Battery battery;
	size_t stepsLeft;
	size_t maxHouseSize;
	vector<vector<char>> houseMatrix;
	Position currPos;
	Position docking;
	DataPool mappingDataPool; // used to map the house
	DataPool dockingDataPool; // used to return to docking station
	DataPool dustDataPool; // used to clean after mapping

	bool configured = false;
	bool mappingPhase = true;
	list<shared_ptr<Node>> goingToGrey;
	list<shared_ptr<Node>> goingToDock;
	list<shared_ptr<Node>> goingToDust;

	void restartAlgorithm() {
		if (configured) {
			battery.setCurrValue(battery.getCapacity());
		}
		stepsLeft = numeric_limits<size_t>::max();
		initHouseMatrix();
		mappingPhase = true;
		mappingDataPool.clear();
		dockingDataPool.clear();
		dustDataPool.clear();
	};

	void initHouseMatrix() {
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
	};

	void updateMatrix(Position& pos, char val) {
		houseMatrix[pos.getY()][pos.getX()] = val;
	};

	char getMatrixValue(Position& pos) {
		return houseMatrix[pos.getY()][pos.getX()];
	};

	void updateWalls(SensorInformation& sensorInformation) {
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
	};

	Direction chooseSimpleDirectionToBlack() {
		return
			houseMatrix[currPos.getY() - 1][currPos.getX()] == BLACK ? Direction::North :
			houseMatrix[currPos.getY() + 1][currPos.getX()] == BLACK ? Direction::South :
			houseMatrix[currPos.getY()][currPos.getX() + 1] == BLACK ? Direction::East :
			houseMatrix[currPos.getY()][currPos.getX() - 1] == BLACK ? Direction::West :
			Direction::Stay;
	};

	Direction chooseSimpleDirection() {
		return
			houseMatrix[currPos.getY() - 1][currPos.getX()] != WALL ? Direction::North :
			houseMatrix[currPos.getY() + 1][currPos.getX()] != WALL ? Direction::South :
			houseMatrix[currPos.getY()][currPos.getX() + 1] != WALL ? Direction::East :
			houseMatrix[currPos.getY()][currPos.getX() - 1] != WALL ? Direction::West :
			Direction::Stay;
	};

	void updateDirtLevel(SensorInformation& sensorInformation) {
		if (!inDockingStation()) {
			int dirtLevel = sensorInformation.dirtLevel;
			houseMatrix[currPos.getY()][currPos.getX()] = '0' + dirtLevel;
		}
	};

	bool greyExists() {
		for (size_t row = 0; row < maxHouseSize; ++row) {
			for (size_t col = 0; col < maxHouseSize; ++col) {
				Position temp = Position(col, row);
				if (houseMatrix[row][col] != WALL && blackNeighborExists(temp) && !allBlack(temp)) {
					return true;
				}
			}
		}
		return false;
	};

	bool houseIsClean() {
		for (size_t row = 0; row < maxHouseSize; ++row) {
			for (size_t col = 0; col < maxHouseSize; ++col) {
				if ('0' < houseMatrix[row][col] && houseMatrix[row][col] <= '9') {
					return false;
				}
			}
		}
		return true;
	};

	Direction getStepFromPath(Position& dest) {
		// Following path generated by A* on some previous step
		return
			dest.getY() < currPos.getY() ? Direction::North :
			dest.getX() < currPos.getX() ? Direction::West :
			dest.getY() > currPos.getY() ? Direction::South :
			dest.getX() > currPos.getX() ? Direction::East :
			Direction::Stay;
	};

	Direction algorithmIteration(SensorInformation& sensorInformation) {

		Direction direction = Direction::Stay;

		bool followPathToGrey = false;
		bool followPathToDocking = false;
		goingToDock.clear();
		goingToDust.clear();
		goingToGrey.clear();
		// update houseMatrix according to sensorInformation
		updateWalls(sensorInformation);
		updateDirtLevel(sensorInformation);

		if (!mappingPhase && houseIsClean()) {
			followPathToDocking = true; // we can finish now
		}
		// check if we have enough steps to go back to docking
		size_t pathLength = getPathToDocking();
		if (isReturnTripFeasable(pathLength) && !isReturnTripFeasable(pathLength + 2)) {
			followPathToDocking = true;
		}
		if (followPathToDocking && !goingToDock.empty()) {
			Position dest = goingToDock.back()->position;
			goingToDock.pop_back();
			direction = getStepFromPath(dest);
		}
		else if (sensorInformation.dirtLevel == 0)  { // keep cleaning if there's dust
			if (mappingPhase) {
				// we can keep mapping
				if (goingToGrey.empty()) {
					if (blackNeighborExists(currPos)) {
						direction = chooseSimpleDirectionToBlack();
					}
					else if (greyExists()) {
						// A* to search for nearest grey
						getPathToGrey();
						followPathToGrey = true;
					}
					else {
						mappingPhase = false; // finished mapping
					}
				}
				else {
					followPathToGrey = true;
				}
				if (followPathToGrey) {
					Position dest = goingToGrey.back()->position;
					goingToGrey.pop_back();
					direction = getStepFromPath(dest);
				}
			}
			else {
				// we can clean now that mapping is done
				getPathToDust();
				if (goingToDust.empty()) {
					direction = chooseSimpleDirection();
				}
				else {
					Position dest = goingToDust.back()->position;
					goingToDust.pop_back();
					direction = getStepFromPath(dest);
				}
			}
		}
		return direction;
	};

	size_t getDistance(Position a, Position b) {
		return abs((int)a.getX() - (int)b.getX()) + abs((int)a.getY() - (int)b.getY());
	};

	bool blackNeighborExists(Position& pos) {
		return 
			(pos.getX() > 0 && houseMatrix[pos.getY()][pos.getX() - 1] == BLACK) ||
			(pos.getX() < maxHouseSize - 1 && houseMatrix[pos.getY()][pos.getX() + 1] == BLACK) ||
			(pos.getY() > 0 && houseMatrix[pos.getY() - 1][pos.getX()] == BLACK) ||
			(pos.getY() < maxHouseSize - 1 && houseMatrix[pos.getY() + 1][pos.getX()] == BLACK);
	};

	bool allBlack(Position& pos) {
		char c;
		return 
			((c = houseMatrix[pos.getY()][pos.getX()]) == BLACK || c == WALL) &&
			(pos.getX() == 0 || (c = houseMatrix[pos.getY()][pos.getX() - 1]) == BLACK || c == WALL) &&
			(pos.getX() == maxHouseSize - 1 || (c = houseMatrix[pos.getY()][pos.getX() + 1]) == BLACK || c == WALL) &&
			(pos.getY() == 0 || (c = houseMatrix[pos.getY() - 1][pos.getX()]) == BLACK || c == WALL) &&
			(pos.getY() == maxHouseSize - 1 || (c = houseMatrix[pos.getY() + 1][pos.getX()]) == BLACK || c == WALL);
	};

	Position getNearestGrey(Position& pos) {
		size_t distance = numeric_limits<size_t>::max();
		Position nearest = pos;
		for (size_t row = 0; row < maxHouseSize; ++row) {
			for (size_t col = 0; col < maxHouseSize; ++col) {
				Position temp = Position(col, row);
				if (blackNeighborExists(temp)) {
					size_t newDistance = getDistance(pos, temp);
					if (distance > newDistance) {
						distance = newDistance;
						nearest = temp;
					}
				}
			}
		}
		return nearest;
	};

	Position getNearestDust(Position& pos) {
		size_t distance = numeric_limits<size_t>::max();
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
	};

	void getPathToGrey() {

		mappingDataPool.clear();
		mappingDataPool.add(currPos, nullptr, 0, getDistance(currPos, getNearestGrey(currPos)));
		shared_ptr<Node> bestNode = mappingDataPool.getBestNode();
		if (bestNode == nullptr) {
			return;
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
		goingToGrey.push_back(temp1);
	};

	void getPathToDust() {

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

		goingToDust.push_back(temp1);
	};

	size_t getPathToDocking() {
		dockingDataPool.clear();
		dockingDataPool.add(currPos, nullptr, 0, getDistance(currPos, docking));
		shared_ptr<Node> bestNode = dockingDataPool.getBestNode();
		if (bestNode == nullptr) {
			return numeric_limits<size_t>::max();
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
				return numeric_limits<size_t>::max();
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

		goingToDock.push_back(temp1);
		return pathLength;
	};

	void addNeighborToMappingDataPool(Position pos, bool pred, Position childPos, shared_ptr<Node> bestNode) {
		if (pred) {
			if (houseMatrix[childPos.getY()][childPos.getX()] != WALL) {
				mappingDataPool.add(childPos, bestNode, bestNode->realCost + 1, getDistance(childPos, getNearestGrey(childPos)));
			}
		}
	};

	void addNeighborToDockingDataPool(Position pos, bool pred, Position childPos, shared_ptr<Node> bestNode) {
		if (pred) {
			if (houseMatrix[childPos.getY()][childPos.getX()] != WALL) {
				dockingDataPool.add(childPos, bestNode, bestNode->realCost + 1, getDistance(childPos, docking));
			}
		}
	};

	void addNeighborToDustDataPool(Position pos, bool pred, Position childPos, shared_ptr<Node> bestNode) {
		if (pred) {
			if (houseMatrix[childPos.getY()][childPos.getX()] != WALL) {
				dustDataPool.add(childPos, bestNode, bestNode->realCost + 1, getDistance(childPos, getNearestDust(childPos)));
			}
		}
	};

	bool inDockingStation() {
		return houseMatrix[currPos.getY()][currPos.getX()] == DOCK;
	};

	bool isReturnTripFeasable(size_t pathLength) {
		return pathLength <= stepsLeft && // There are enough steps
			battery.getCurrValue() >= pathLength * battery.getConsumptionRate(); // The battery will suffice
	};

public:

	virtual void setSensor(const AbstractSensor& sensor) override {
		this->sensor = &sensor;
		restartAlgorithm();
	};

	virtual void setConfiguration(map<string, int> config) override {
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
	};

	virtual Direction step(Direction prevStep) override {

		// update currPos with prevStep
		currPos.moveDirection(prevStep);

		// get sensor information
		Direction direction = Direction::Stay; // default
		SensorInformation sensorInformation = sensor->sense();

		// consume battery
		if (!inDockingStation()) {
			battery.consume();
		}

		// TODO another algorithm should avoid staying in one place when in phase1
		direction = algorithmIteration(sensorInformation);

		// charge battery
		if (inDockingStation()) {
			battery.charge();
		}

		--stepsLeft;
		return direction;
	};

	virtual void aboutToFinish(int stepsTillFinishing) override {
		stepsLeft = stepsTillFinishing > 0 ? stepsTillFinishing : 0;
	};

};

#endif // __ASTAR_ALGORITHM__H_