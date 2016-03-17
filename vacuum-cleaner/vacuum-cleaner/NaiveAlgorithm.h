#ifndef __NAIVE_ALGORITHM__H_
#define __NAIVE_ALGORITHM__H_

#include "AbstractAlgorithm.h"
#include "Battery.h" 
#include "House.h"

#include <stdlib.h>
#include <ctime>
#include <vector>

using namespace std;

class NaiveAlgorithm : public AbstractAlgorithm {

	AbstractSensor& sensor;
	Battery *battery;
	int maxSteps;
	int maxStepsAfterWinner;

public:

	NaiveAlgorithm() {
		srand(time(NULL));
	};

	~NaiveAlgorithm() {
		delete battery;
	};

	void setSensor(const AbstractSensor& sensor) override {
		this->sensor = sensor;
	};

	void setConfiguration(map<string, int> config) override;

	Direction step() override;

	void aboutToFinish(int stepsTillFinishing) override {};
};

#endif // __NAIVE_ALGORITHM__H_