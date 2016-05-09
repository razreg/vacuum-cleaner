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
const char WALL = 'A';
const int ADD_FOR_GREY = 30; // add this amount whenever you want to signal that this cell is "grey"

const int NORTH_IDX = static_cast<int>(Direction::North);
const int EAST_IDX = static_cast<int>(Direction::East);
const int WEST_IDX = static_cast<int>(Direction::West);
const int SOUTH_IDX = static_cast<int>(Direction::South);

class AstarAlgorithm : public AbstractAlgorithm {

	class Node {
	public:
		Position position;
		Node* parent = nullptr;
		int realCost = 0;
		int heuristicCost = 0;

		Node() {};

		Node(Position position, int realCost, int heuristicCost, Node* parentNode) :
			position(position), parent(parentNode), realCost(realCost), heuristicCost(heuristicCost) {};
	};

	class PoolObject {
	public:
		bool inFringe;
		Node node;
	};

	class DataPool {
		vector<PoolObject> poolObjects;
		size_t top = 0;
	public:
		DataPool(size_t capacity) {
			poolObjects = vector<PoolObject>(capacity);
			for (size_t i = 0; i < capacity; ++i)
				poolObjects[i].node = Node(Position(), 0, 0, nullptr);
		};

		void add(Position position, Node* parent, int realCost, int heuristicCost) {
			size_t idx = 0;
			while (idx < top) {
				if (poolObjects[idx].node.position == position) {
					if (realCost + heuristicCost < poolObjects[idx].node.realCost + poolObjects[idx].node.heuristicCost) {
						break;
					}
					else {
						return;
					}
				}
				idx++;
			}

			Node node = poolObjects[idx].node;
			poolObjects[idx].inFringe = true;
			node.position = position;
			node.realCost = realCost;
			node.heuristicCost = heuristicCost;
			node.parent = parent;
			top++;
		};

		void clear() {
			for (size_t i = 0; i < top; ++i) {
				poolObjects[i].inFringe = false;
			}
			top = 0;
		};

		Node* getBestNode() {
			if (top == 0) {
				return nullptr;
			}
			int bestIdx = -1;
			int cost = 999999; // TODO replace with MAX_INT
			Node* node;

			for (int i = 0; i < (int)top; ++i) {
				node = &poolObjects[i].node;
				if (poolObjects[i].inFringe && ((node->heuristicCost + node->realCost) < cost)) {
					bestIdx = i;
					cost = node->realCost + node->heuristicCost;
				}
			}

			if (bestIdx == -1) {
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
	Position possiblePos;
	size_t greyCount = 0; // TODO make sure it is updated
	DataPool fringe;

	bool configured = false;
	bool phase1Running = false;
	Node* goingToGrey;

	void restartAlgorithm() {
		if (configured) {
			battery.setCurrValue(battery.getCapacity());
		}
		stepsLeft = numeric_limits<size_t>::max();
		initHouseMatrix();
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
		updateMatrix(currPos, DOCK + ADD_FOR_GREY);
	};

	void updateMatrix(Position& pos, char val) {
		houseMatrix[pos.getY()][pos.getX()] = val;
	};

	char getMatrixValue(Position& pos) {
		return houseMatrix[pos.getY()][pos.getX()];
	};

	bool isGrey(char c) {
		return c >= '0' + ADD_FOR_GREY;
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
			houseMatrix[currPos.getY() - 1][currPos.getX()] != WALL ? Direction::North :
			houseMatrix[currPos.getY() + 1][currPos.getX()] != WALL ? Direction::South :
			houseMatrix[currPos.getY()][currPos.getX() + 1] != WALL ? Direction::East :
			houseMatrix[currPos.getY()][currPos.getX() - 1] != WALL ? Direction::West :
			Direction::Stay;
	};

	void updateDirtLevel(SensorInformation& sensorInformation) {
		bool isCurrPosGrey = isGrey(getMatrixValue(currPos));
		if (!inDockingStation()) {
			int dirtLevel = sensorInformation.dirtLevel;
			houseMatrix[currPos.getY()][currPos.getX()] = '0' + dirtLevel + isCurrPosGrey ? ADD_FOR_GREY : 0;
		}
	};

	bool greyExists() {
		return greyCount > 0;
	};

	Direction phase1Iteration(SensorInformation& sensorInformation) {

		bool followPath = false;

		// update houseMatrix according to sensorInformation
		updateWalls(sensorInformation);
		updateDirtLevel(sensorInformation);

		// if all neighbors are white, set currPos to white
		if (isGrey(houseMatrix[currPos.getY()][currPos.getX()]) &&
			!(currPos.getY() > 0 && (houseMatrix[currPos.getY() - 1][currPos.getX()] == BLACK || isGrey(houseMatrix[currPos.getY() - 1][currPos.getX()])))
			&& !(currPos.getY() + 1 < maxHouseSize && (houseMatrix[currPos.getY() + 1][currPos.getX()] == BLACK || isGrey(houseMatrix[currPos.getY() + 1][currPos.getX()])))
			&& !(currPos.getX() > 0 && (houseMatrix[currPos.getY()][currPos.getX() - 1] == BLACK || isGrey(houseMatrix[currPos.getY()][currPos.getX() - 1])))
			&& !(currPos.getX() + 1 < maxHouseSize && (houseMatrix[currPos.getY()][currPos.getX() + 1] == BLACK || isGrey(houseMatrix[currPos.getY()][currPos.getX() + 1])))){
			houseMatrix[currPos.getY()][currPos.getX()] -= ADD_FOR_GREY;
		}

		if (goingToGrey == nullptr) {
			if (isGrey(getMatrixValue(currPos))) {
				Direction direction = chooseSimpleDirection();
				possiblePos = currPos;
				return direction;
			}
			else {
				if (greyExists()) {
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
		}
		else {
			followPath = true;
		}

		if (followPath) {
			// Following path generated by A* on some previous step
			Position dest = goingToGrey->position;
			goingToGrey = goingToGrey->parent;
			possiblePos = dest;

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
		cout << "phase2Iteration" << endl;
		return Direction::Stay;
	};

	size_t getDistance(Position a, Position b) {
		return abs((int)a.getX() - (int)b.getX()) + abs((int)a.getY() - (int)b.getY());
	};

	Position getNearestGrey(Position& pos) {
		size_t distance = numeric_limits<size_t>::max();
		Position nearest = pos;
		for (size_t row = 0; row < maxHouseSize; ++row) {
			for (size_t col = 0; col < maxHouseSize; ++col) {
				Position temp = Position(col, row);
				if (isGrey(getMatrixValue(temp))) {
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

	void getPathToGrey() {

		fringe.clear();
		fringe.add(currPos, nullptr, 0, getDistance(currPos, getNearestGrey(currPos)));

		Node* bestNode = fringe.getBestNode(); // TODO what if bestNode == nullptr ?
		Position pos = bestNode->position;
		Position childPos;

		// TODO change to == BLACK ?
		while (!(pos.getY() > 0 && isGrey(houseMatrix[pos.getY() - 1][pos.getX()])) 
			&& !(pos.getY()+1 < maxHouseSize && isGrey(houseMatrix[pos.getY() + 1][pos.getX()]))
			&& !(pos.getX() > 0 && isGrey(houseMatrix[pos.getY()][pos.getX() - 1])) 
			&& !(pos.getX()+1 < maxHouseSize && isGrey(houseMatrix[pos.getY()][pos.getX() + 1]))) {

			//adding neighbour tiles to fringe if they are not walls
			addBlackToFringe(pos, pos.getX() > 0, pos.getX() - 1, pos.getY(), bestNode);
			addBlackToFringe(pos, pos.getX() < maxHouseSize - 1, pos.getX() + 1, pos.getY(), bestNode);
			addBlackToFringe(pos, pos.getY() < maxHouseSize - 1, pos.getX(), pos.getY() + 1, bestNode);
			addBlackToFringe(pos, pos.getY() > 0, pos.getX(), pos.getY() - 1, bestNode);

			bestNode = fringe.getBestNode();
			pos = bestNode->position;
		}

		// reverse path
		Node* temp1 = bestNode;
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

		goingToGrey = temp1;
	};

	void addBlackToFringe(Position pos, bool pred, size_t x, size_t y, Node* bestNode) {
		if (pred) {
			if (houseMatrix[y][x] == BLACK) {
				Position childPos(x, y);
				fringe.add(childPos, bestNode, bestNode->realCost + 1, getDistance(childPos, getNearestGrey(childPos)));
			}
		}
	};

	bool inDockingStation() {
		return houseMatrix[currPos.getY()][currPos.getX()] == DOCK ||
			houseMatrix[currPos.getY()][currPos.getX()] == DOCK + ADD_FOR_GREY;
	};

public:

	AstarAlgorithm() : fringe(maxHouseSize * maxHouseSize) {};

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
			maxHouseSize = 200; // TODO move to constant
		}
		else {
			maxHouseSize = battery.getCapacity() / battery.getConsumptionRate();
			maxHouseSize += 1 - maxHouseSize % 2; // make odd so docking station would be in the center
		}
		initHouseMatrix();
		configured = true;
	};

	// TODO get back to docking
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