#ifndef __NAIVE_ALGORITHM__H_
#define __NAIVE_ALGORITHM__H_

#include <stdlib.h>
#include <ctime>
#include <vector>

#include "AbstractAlgorithm.h"
#include "Common.h"

using namespace std;

class NaiveAlgorithm : public AbstractAlgorithm {

	static Logger logger;

	const AbstractSensor* sensor;
	int maxSteps;
	int maxStepsAfterWinner;

public:

	NaiveAlgorithm() {
		srand(time(NULL));
	};

	void setSensor(const AbstractSensor& sensor) override {
		this->sensor = &sensor;
	};

	void setConfiguration(map<string, int> config) override;

	Direction step() override;

	void aboutToFinish(int stepsTillFinishing) override {};
};

#endif // __NAIVE_ALGORITHM__H_