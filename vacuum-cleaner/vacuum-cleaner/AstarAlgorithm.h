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

const size_t HOUSE_SIZE_UPPER_BOUND = 501;

const int NORTH_IDX = static_cast<int>(Direction::North);
const int EAST_IDX = static_cast<int>(Direction::East);
const int WEST_IDX = static_cast<int>(Direction::West);
const int SOUTH_IDX = static_cast<int>(Direction::South);

class AstarAlgorithm : public AbstractAlgorithm {

	class Node {
	public:
		Position position;
		Node* parent = nullptr; // TODO shared_ptr?
		int realCost = 0;
		int heuristicCost = 0;

		Node() {};

		Node(Position position, int realCost, int heuristicCost, Node* parentNode) :
			position(position), parent(parentNode), realCost(realCost), heuristicCost(heuristicCost) {};
	};

	class PoolObject {
	public:
		bool inFringe = false;
		Node node;
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

		void add(Position position, Node* parent, int realCost, int heuristicCost) {
			size_t idx = 0;
			while (idx < poolObjects.size()) {
				if (poolObjects[idx].node.position == position) {
					if (realCost + heuristicCost < poolObjects[idx].node.realCost + poolObjects[idx].node.heuristicCost) {
						updateNode(poolObjects[idx].node, position, parent, realCost, heuristicCost);
					}
					return;
				}
				idx++;
			}
			poolObjects.push_back(PoolObject());
			poolObjects[idx].inFringe = true;
			updateNode(poolObjects[idx].node, position, parent, realCost, heuristicCost);
		};

		void updateNode(Node& node, Position& position, Node* parent, int realCost, int heuristicCost) {
			node.position = position;
			node.realCost = realCost;
			node.heuristicCost = heuristicCost;
			node.parent = parent;
		};

		void clear() {
			poolObjects.clear();
		};

		Node* getBestNode() {
			if (poolObjects.empty()) {
				cout << __LINE__ << endl;
				return nullptr;
			}
			int bestIdx = -1;
			int cost = 999999999; // TODO replace with MAX_INT
			Node* node;

			for (size_t i = 0; i < poolObjects.size(); ++i) {
				node = &poolObjects[i].node;
				if (poolObjects[i].inFringe && ((node->heuristicCost + node->realCost) < cost)) {
					bestIdx = i;
					cost = node->realCost + node->heuristicCost;
				}
			}
			if (bestIdx == -1) {
				return nullptr;
			}
			cout << "best cost: " << cost << endl;
			poolObjects[bestIdx].inFringe = false;
			return &poolObjects[bestIdx].node;
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
	Node* goingToGrey = nullptr;
	Node* goingToDock = nullptr;
	Node* goingToDust = nullptr;

	void restartAlgorithm() {
		if (configured) {
			battery.setCurrValue(battery.getCapacity());
		}
		stepsLeft = numeric_limits<size_t>::max();
		initHouseMatrix();
		mappingPhase = true;
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
		if (sensorInformation.isWall[NORTH_IDX]) {
			houseMatrix[currPos.getY() - 1][currPos.getX()] = WALL;
		}
		if (sensorInformation.isWall[SOUTH_IDX]) {
			houseMatrix[currPos.getY() + 1][currPos.getX()] = WALL;
		}
		if (sensorInformation.isWall[EAST_IDX]) {
			houseMatrix[currPos.getY()][currPos.getX() + 1] = WALL;
		}
		if (sensorInformation.isWall[WEST_IDX]) {
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

		// update houseMatrix according to sensorInformation
		updateWalls(sensorInformation);
		updateDirtLevel(sensorInformation);

		if (!mappingPhase && houseIsClean()) {
			followPathToDocking = true; // we can finish now
		}
		cout << __LINE__ << endl; // TODO remove
		// check if we have enough steps to go back to docking
		size_t pathLength = getPathToDocking();
		cout << __LINE__ << endl; // TODO remove
		if (isReturnTripFeasable(pathLength) && !isReturnTripFeasable(pathLength + 2)) {
			cout << __LINE__ << endl; // TODO remove
			followPathToDocking = true;
		}
		cout << __LINE__ << endl; // TODO remove
		if (followPathToDocking && goingToDock != nullptr) {
			cout << __LINE__ << endl; // TODO remove
			Position dest = goingToDock->position;
			cout << __LINE__ << endl; // TODO remove
			goingToDock = goingToDock->parent;
			cout << __LINE__ << endl; // TODO remove
			direction = getStepFromPath(dest);
			cout << __LINE__ << endl; // TODO remove
		}
		else if (sensorInformation.dirtLevel == 0)  { // keep cleaning if there's dust
			cout << __LINE__ << endl; // TODO remove
			if (mappingPhase) {
				// we can keep mapping
				if (goingToGrey == nullptr) {
					cout << __LINE__ << endl; // TODO remove
					if (blackNeighborExists(currPos)) {
						cout << __LINE__ << endl; // TODO remove
						direction = chooseSimpleDirectionToBlack();
					}
					else if (greyExists()) {
						cout << __LINE__ << endl; // TODO remove
						// A* to search for nearest grey
						getPathToGrey();
						cout << __LINE__ << endl; // TODO remove
						followPathToGrey = true;
					}
					else {
						mappingPhase = false; // finished mapping
					}
				}
				else {
					followPathToGrey = true; // TODO what happens if prevStep is not what we selected
				}
				cout << __LINE__ << endl; // TODO remove
				if (followPathToGrey) {
					cout << __LINE__ << endl; // TODO remove
					Position dest = goingToGrey->position;
					cout << __LINE__ << endl; // TODO remove
					goingToGrey = goingToGrey->parent;
					cout << __LINE__ << endl; // TODO remove
					direction = getStepFromPath(dest);
					cout << __LINE__ << endl; // TODO remove
				}
			}
			else {
				cout << __LINE__ << endl; // TODO remove
				// we can clean now that mapping is done
				getPathToDust();
				if (goingToDust == nullptr) {
					cout << __LINE__ << endl; // TODO remove
					direction = chooseSimpleDirection();
				}
				else {
					Position dest = goingToDust->position;
					goingToDust = goingToDust->parent;
					direction = getStepFromPath(dest);
				}
			}
		}
		cout << __LINE__ << endl; // TODO remove
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
		if (mappingDataPool.empty()) {
			return;
		}
		Node* bestNode = mappingDataPool.getBestNode(); // TODO what if bestNode == nullptr ?
		Position pos = bestNode->position;
		Position childPos;

		while (!blackNeighborExists(pos)) {
			addNeighborToMappingDataPool(pos, pos.getX() > 0, Position(pos.getX() - 1, pos.getY()), bestNode);
			addNeighborToMappingDataPool(pos, pos.getX() < maxHouseSize - 1, Position(pos.getX() + 1, pos.getY()), bestNode);
			addNeighborToMappingDataPool(pos, pos.getY() < maxHouseSize - 1, Position(pos.getX(), pos.getY() + 1), bestNode);
			addNeighborToMappingDataPool(pos, pos.getY() > 0, Position(pos.getX(), pos.getY() - 1), bestNode);
			bestNode = mappingDataPool.getBestNode();
			pos = bestNode->position;
		}

		// reverse path
		Node* temp1 = bestNode;
		if (bestNode->parent != nullptr) {
			Node* temp2 = bestNode->parent;
			Node* temp3;

			while (temp2->parent != nullptr) {
				temp3 = temp2->parent;
				temp2->parent = temp1;
				temp1 = temp2;
				temp2 = temp3; // TODO isn't this line redundant?
			}
			temp2->parent = temp1;
			bestNode->parent = nullptr;
		}

		goingToGrey = temp1;
	};

	void getPathToDust() {

		dustDataPool.clear();
		dustDataPool.add(currPos, nullptr, 0, getDistance(currPos, getNearestDust(currPos)));
		if (dustDataPool.empty()) {
			return;
		}
		Node* bestNode = dustDataPool.getBestNode(); // TODO what if bestNode == nullptr ?
		Position pos = bestNode->position;
		Position childPos;

		// while there is no dust in pos
		while (houseMatrix[pos.getY()][pos.getX()] > '9' || houseMatrix[pos.getY()][pos.getX()] <= '0') {
			addNeighborToDustDataPool(pos, pos.getX() > 0, Position(pos.getX() - 1, pos.getY()), bestNode);
			addNeighborToDustDataPool(pos, pos.getX() < maxHouseSize - 1, Position(pos.getX() + 1, pos.getY()), bestNode);
			addNeighborToDustDataPool(pos, pos.getY() < maxHouseSize - 1, Position(pos.getX(), pos.getY() + 1), bestNode);
			addNeighborToDustDataPool(pos, pos.getY() > 0, Position(pos.getX(), pos.getY() - 1), bestNode);
			bestNode = dustDataPool.getBestNode();
			pos = bestNode->position;
		}

		// reverse path
		Node* temp1 = bestNode;
		if (bestNode->parent != nullptr) {
			Node* temp2 = bestNode->parent;
			Node* temp3;

			while (temp2->parent != nullptr) {
				temp3 = temp2->parent;
				temp2->parent = temp1;
				temp1 = temp2;
				temp2 = temp3; // TODO isn't this line redundant?
			}
			temp2->parent = temp1;
			bestNode->parent = nullptr;
		}

		goingToDust = temp1;
	};

	size_t getPathToDocking() {

		dockingDataPool.clear();
		dockingDataPool.add(currPos, nullptr, 0, getDistance(currPos, docking));
		if (dockingDataPool.empty()) {
			return 0;
		}
		cout << __LINE__ << endl; // TODO remove
		Node* bestNode = dockingDataPool.getBestNode(); // TODO what if bestNode == nullptr ?
		cout << __LINE__ << endl; // TODO remove
		Position pos = bestNode->position;
		cout << __LINE__ << endl; // TODO remove
		Position childPos;
		
		while (pos != docking) {
			addNeighborToDockingDataPool(pos, pos.getX() > 0, Position(pos.getX() - 1, pos.getY()), bestNode);
			addNeighborToDockingDataPool(pos, pos.getX() < maxHouseSize - 1, Position(pos.getX() + 1, pos.getY()), bestNode);
			addNeighborToDockingDataPool(pos, pos.getY() < maxHouseSize - 1, Position(pos.getX(), pos.getY() + 1), bestNode);
			addNeighborToDockingDataPool(pos, pos.getY() > 0, Position(pos.getX(), pos.getY() - 1), bestNode);
			bestNode = dockingDataPool.getBestNode();
			pos = bestNode->position;
			cout << "dockingDataPool.size() == " << dockingDataPool.size() << endl;
		}
		cout << __LINE__ << endl; // TODO remove
		// reverse path
		size_t pathLength = 1;
		Node* temp1 = bestNode;
		cout << __LINE__ << endl; // TODO remove
		if (bestNode->parent != nullptr) {
			cout << __LINE__ << endl; // TODO remove
			Node* temp2 = bestNode->parent;
			Node* temp3;
			cout << __LINE__ << endl; // TODO remove
			while (temp2->parent != nullptr) {
				temp3 = temp2->parent;
				temp2->parent = temp1;
				temp1 = temp2;
				temp2 = temp3; // TODO isn't this line redundant?
				pathLength++;
			}
			temp2->parent = temp1;
			bestNode->parent = nullptr;
		}

		goingToDock = temp1;
		return pathLength;
	};

	void addNeighborToMappingDataPool(Position pos, bool pred, Position childPos, Node* bestNode) {
		if (pred) {
			if (houseMatrix[childPos.getY()][childPos.getX()] != WALL) {
				mappingDataPool.add(childPos, bestNode, bestNode->realCost + 1, getDistance(childPos, getNearestGrey(childPos)));
			}
		}
	};

	void addNeighborToDockingDataPool(Position pos, bool pred, Position childPos, Node* bestNode) {
		if (pred) {
			if (houseMatrix[childPos.getY()][childPos.getX()] != WALL) {
				dockingDataPool.add(childPos, bestNode, bestNode->realCost + 1, getDistance(childPos, docking));
			}
		}
	};

	void addNeighborToDustDataPool(Position pos, bool pred, Position childPos, Node* bestNode) {
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
		maxHouseSize = 41; // TODO remove
		maxHouseSize = min(maxHouseSize, HOUSE_SIZE_UPPER_BOUND);
		initHouseMatrix();
		configured = true;
	};

	virtual Direction step(Direction prevStep) override {

		// TODO remove
		if (stepsLeft < 2) {
			for (size_t row = 0; row < maxHouseSize; ++row) {
				for (size_t col = 0; col < maxHouseSize; ++col) {
					cout << houseMatrix[row][col];
				}
				cout << endl;
			}
			cout << endl;
			size_t count = 0;
			for (size_t row = 0; row < maxHouseSize; ++row) {
				for (size_t col = 0; col < maxHouseSize; ++col) {
					Position temp = Position(col, row);
					if (houseMatrix[row][col] != WALL && blackNeighborExists(temp) && !allBlack(temp)) {
						count++;
					}
				}
			}
			cout << "Greys: " << count << endl;
		}

		// update currPos with prevStep
		currPos.moveDirection(prevStep);
		cout << "(" << currPos.getX() << "," << currPos.getY() << ")" << endl; // TODO remove

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