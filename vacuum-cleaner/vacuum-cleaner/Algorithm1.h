#ifndef __ALGORITHM_1__H_
#define __ALGORITHM_1__H_

#include <cstdlib>

#include "AbstractAlgorithm.h"
#include "Common.h"

using namespace std;

class Algorithm1 : public AbstractAlgorithm {

	static Logger logger;

	vector<Direction> movesBack;
	const AbstractSensor* sensor;
	int maxStepsAfterWinner;
	int maxSteps;
	int capacity;
	int consumptionRate;
	int rechargeRate;
	int currValue;

public:

	Algorithm1() {
		srand(time(NULL));
	};

	void setSensor(const AbstractSensor& sensor) override {
		this->sensor = &sensor;
		currValue = capacity;
	};

	void setConfiguration(map<string, int> config) override;

	Direction step() override;

	void aboutToFinish(int stepsTillFinishing) override {};

	bool inDockingStation() {
		return movesBack.size() == 0;
	}

	void setCurrValue() { //back to docking station
		if (inDockingStation()) {
			currValue = min(currValue -1 + rechargeRate, capacity); // charge battery
			movesBack.erase(movesBack.begin(), movesBack.end()); // empty movesback
		}
		else currValue = max(0, currValue - consumptionRate); // consume battery
	}

	//updates the vector which is responsible for returning back to docking station (opposite of the current returned direction)
	void updateMovesBack(Direction direction) {
		switch (direction)  {
		case (Direction::North) :
			movesBack.push_back(Direction::South);
			break;
		case (Direction::South) :
			movesBack.push_back(Direction::North);
			break;
		case (Direction::East) :
			movesBack.push_back(Direction::West);
			break;
		case (Direction::West) :
			movesBack.push_back(Direction::East);
			break;
		default:
			break;
		}
	}

};

#endif // __ALGORITHM_1__H_