#ifndef __ASTAR_ALGORITHM__H_
#define __ASTAR_ALGORITHM__H_

#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "Battery.h"
#include "Position.h"
#include "Common.h"
#include "uniqueptr.h"

using namespace std;

const char BLACK = 'B';
const char DOCK = 'D';
const char WALL = 'W';

const size_t HOUSE_SIZE_UPPER_BOUND = 241; // TODO decide on bound
const size_t NUMERIC_UPPER_BOUND = numeric_limits<size_t>::max() / 2 - 1;

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

		bool empty() const {
			return poolObjects.empty();
		};

		size_t size() const {
			return poolObjects.size();
		};

		void add(Position position, shared_ptr<Node> parent, int realCost, int heuristicCost);

		void updateNode(shared_ptr<Node> node, Position& position, shared_ptr<Node> parent, 
			int realCost, int heuristicCost);

		void clear() {
			poolObjects.clear();
		};

		shared_ptr<Node> getBestNode();
	};

	const AbstractSensor* sensor;
	Battery battery;
	size_t stepsLeft;
	size_t maxHouseSize;
	vector<vector<char>> houseMatrix;
	Position currPos;
	Position docking;
	Direction expectedPrevStep = Direction::Stay;

	// mapping information for A*
	DataPool mappingDataPool; // used to map the house
	DataPool dockingDataPool; // used to return to docking station
	DataPool dustDataPool; // used to clean after mapping

	// used to understand if we need to compute the path to the docking station or dust
	size_t heuristicCostToDocking = NUMERIC_UPPER_BOUND;

	// paths and what to do next
	bool followPathToGrey = false;
	bool followPathToDocking = false;
	bool configured = false;
	bool mappingPhase = true;
	shared_ptr<Node> goingToGrey = nullptr;
	shared_ptr<Node> goingToDock = nullptr;
	shared_ptr<Node> goingToDust = nullptr;

	void initHouseMatrix();

	void updateMatrix(Position& pos, char val) {
		houseMatrix[pos.getY()][pos.getX()] = val;
	};

	char getMatrixValue(Position& pos) {
		return houseMatrix[pos.getY()][pos.getX()];
	};

	void updateWalls(SensorInformation& sensorInformation);

	void updateDirtLevel(SensorInformation& sensorInformation);

	bool greyExists() const;

	bool houseIsClean() const;

	Direction getStepFromPath(Position& dest) const;

	Direction algorithmIteration(SensorInformation& sensorInformation, bool restart);

	size_t getDistance(Position a, Position b) const {
		return abs((int)a.getX() - (int)b.getX()) + abs((int)a.getY() - (int)b.getY());
	};

	bool blackNeighborExists(Position& pos) const;

	bool allBlack(Position& pos) const;

	Position getNearestGrey(Position& pos) const;

	Position getNearestDust(Position& pos) const;

	size_t getPathToGrey();

	void getPathToDust();

	size_t getPathToDocking();

	void addNeighborToMappingDataPool(Position pos, bool pred, Position childPos, shared_ptr<Node> bestNode);

	void addNeighborToDockingDataPool(Position pos, bool pred, Position childPos, shared_ptr<Node> bestNode);

	void addNeighborToDustDataPool(Position pos, bool pred, Position childPos, shared_ptr<Node> bestNode);

	bool inDockingStation() {
		return houseMatrix[currPos.getY()][currPos.getX()] == DOCK;
	};

	void restartDataForIteration();

protected:

	Position& getCurrPos() {
		return currPos;
	};

	vector<vector<char>>& getHouseMatrix() {
		return houseMatrix;
	};

	size_t getMaxHouseSize() {
		return maxHouseSize;
	};

	virtual bool isReturnTripFeasable(size_t pathLength) {
		return pathLength < stepsLeft && // There are enough steps
			battery.getCurrValue() > pathLength * battery.getConsumptionRate(); // The battery will suffice
	};

	virtual void restartAlgorithm();

	virtual Direction chooseSimpleDirectionToBlack() = 0;

	virtual Direction chooseSimpleDirection() = 0;

public:

	virtual void setSensor(const AbstractSensor& sensor) override {
		this->sensor = &sensor;
		restartAlgorithm();
	};

	virtual void setConfiguration(map<string, int> config) override;

	virtual Direction step(Direction prevStep) override;

	virtual void aboutToFinish(int stepsTillFinishing) override {
		stepsLeft = stepsTillFinishing > 0 ? stepsTillFinishing : 0;
	};

	// should the algorithm keep mapping/moving on or stay?
	virtual bool keepMoving(SensorInformation& sensorInformation) const {
		return sensorInformation.dirtLevel == 0; // keep cleaning if there's dust
	};

};

#endif // __ASTAR_ALGORITHM__H_