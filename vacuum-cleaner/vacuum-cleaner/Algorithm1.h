#ifndef __ALGORITHM_1__H_
#define __ALGORITHM_1__H_

#include <cstdlib>

#include "AbstractAlgorithm.h"
#include "Common.h"

using namespace std;

class Algorithm1 : public AbstractAlgorithm {

	static Logger logger;

	static Direction previousDirection;

	const AbstractSensor* sensor;
	int maxStepsAfterWinner;

public:

	Algorithm1() {
		srand(time(NULL));
	};

	void setSensor(const AbstractSensor& sensor) override {
		this->sensor = &sensor;
	};

	void setConfiguration(map<string, int> config) override;

	Direction step() override;

	void aboutToFinish(int stepsTillFinishing) override {};

	bool isPossibleDirection(vector<Direction> directions, Direction direction);

};

#endif // __ALGORITHM_1__H_