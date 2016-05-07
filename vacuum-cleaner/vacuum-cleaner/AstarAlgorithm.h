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

	const AbstractSensor* sensor;
	Battery battery;
	size_t stepsLeft;
	size_t maxHouseSize;
	vector<vector<char>> houseMatrix;
	Position currPos;

	bool configured = false;
	bool phase1Running = false;

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

	void phase1Iteration(SensorInformation& sensorInformation) {

		// update houseMatrix according to sensorInformation
		updateWalls(sensorInformation);

		// TODO if(GoingToGrey == null)
		if (isGrey(getMatrixValue(currPos))) {
			
		}
	};

	bool inDockingStation() {
		return houseMatrix[currPos.getY()][currPos.getX()] == DOCK ||
			houseMatrix[currPos.getY()][currPos.getX()] == DOCK + ADD_FOR_GREY;
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
			maxHouseSize = 1000;
		}
		else {
			maxHouseSize = battery.getCapacity() / battery.getConsumptionRate();
			maxHouseSize += 1 - maxHouseSize % 2; // make odd so docking station would be in the center
		}
		initHouseMatrix();
		configured = true;
	};

	virtual Direction step(Direction prevStep) override {
		Direction direction = Direction::Stay; // default
		SensorInformation sensorInformation = sensor->sense();

		// consume battery
		if (!inDockingStation()) {
			battery.consume();
		}

		// TODO do your thing

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