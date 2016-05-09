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
			poolObjects.emplace_back();
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
				cout << __LINE__ << endl;
				return nullptr;
			}
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
	DataPool fringe;

	bool configured = false;
	bool phase1Running = true;
	Node* goingToGrey;

	void restartAlgorithm() {
		if (configured) {
			battery.setCurrValue(battery.getCapacity());
		}
		stepsLeft = numeric_limits<size_t>::max();
		initHouseMatrix();
		phase1Running = true;
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

	Direction chooseSimpleDirection() {
		return
			houseMatrix[currPos.getY() - 1][currPos.getX()] == BLACK ? Direction::North :
			houseMatrix[currPos.getY() + 1][currPos.getX()] == BLACK ? Direction::South :
			houseMatrix[currPos.getY()][currPos.getX() + 1] == BLACK ? Direction::East :
			houseMatrix[currPos.getY()][currPos.getX() - 1] == BLACK ? Direction::West :
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

	Direction phase1Iteration(SensorInformation& sensorInformation) {

		cout << "phase1Iteration" << endl;

		bool followPath = false;

		// update houseMatrix according to sensorInformation
		updateWalls(sensorInformation);
		updateDirtLevel(sensorInformation);

		if (goingToGrey == nullptr) {
			if (blackNeighborExists(currPos)) {
				Direction direction = chooseSimpleDirection();
				return direction;
			}
			else if (greyExists()) {
				// A* to search for nearest grey
				getPathToGrey();
				followPath = true;
			}
			else {
				// finished phase1
				// TOOD copy contents of the houseMatrix to a smaller matrix
				phase1Running = false;
			}
		}
		else {
			followPath = true;
		}

		if (followPath) {
			// Following path generated by A* on some previous step
			Position dest = goingToGrey->position;
			goingToGrey = goingToGrey->parent;
			return 
				dest.getY() < currPos.getY() ? Direction::North : 
				dest.getX() < currPos.getX() ? Direction::West : 
				dest.getY() > currPos.getY() ? Direction::South : 
				dest.getX() > currPos.getX() ? Direction::East : 
				Direction::Stay;
		}
		return Direction::Stay;
	};

	// TODO
	Direction phase2Iteration(SensorInformation& sensorInformation) {
		cout << "phase2Iteration: stepsLeft = "<< numeric_limits<size_t>::max() - stepsLeft << endl;
		return Direction::Stay;
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

	void getPathToGrey() {

		fringe.clear();
		fringe.add(currPos, nullptr, 0, getDistance(currPos, getNearestGrey(currPos)));
		Node* bestNode = fringe.getBestNode(); // TODO what if bestNode == nullptr ?
		Position pos = bestNode->position;
		Position childPos;

		while (!blackNeighborExists(pos)) {
			addBlackToFringe(pos, pos.getX() > 0, pos.getX() - 1, pos.getY(), bestNode);
			addBlackToFringe(pos, pos.getX() < maxHouseSize - 1, pos.getX() + 1, pos.getY(), bestNode);
			addBlackToFringe(pos, pos.getY() < maxHouseSize - 1, pos.getX(), pos.getY() + 1, bestNode);
			addBlackToFringe(pos, pos.getY() > 0, pos.getX(), pos.getY() - 1, bestNode);
			bestNode = fringe.getBestNode();
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
				temp2 = temp3;
			}
			temp2->parent = temp1;
			bestNode->parent = nullptr;
		}

		goingToGrey = temp1;
	};

	void addBlackToFringe(Position pos, bool pred, size_t x, size_t y, Node* bestNode) {
		if (pred) {
			if (houseMatrix[y][x] != WALL) {
				Position childPos(x, y);
				fringe.add(childPos, bestNode, bestNode->realCost + 1, getDistance(childPos, getNearestGrey(childPos)));
			}
		}
	};

	bool inDockingStation() {
		return houseMatrix[currPos.getY()][currPos.getX()] == DOCK;
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
		if (battery.getConsumptionRate() < 1) {
			maxHouseSize = 400; // TODO move to constant
		}
		else {
			maxHouseSize = battery.getCapacity() / battery.getConsumptionRate();
			maxHouseSize += 1 - maxHouseSize % 2; // make odd so docking station would be in the center
			// TODO make sure it's not too big
		}
		maxHouseSize = 40; // TODO remove
		initHouseMatrix();
		configured = true;
	};

	// TODO get back to docking
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
		cout << "(" << currPos.getX() << "," << currPos.getY() << ")" << endl;

		// get sensor information
		Direction direction = Direction::Stay; // default
		SensorInformation sensorInformation = sensor->sense();

		// consume battery
		if (!inDockingStation()) {
			battery.consume();
		}

		// clean currPos
		if (sensorInformation.dirtLevel > 0) {
			// TODO staty until clean?
		}

		direction = phase1Running ? phase1Iteration(sensorInformation) : 
			phase2Iteration(sensorInformation);

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