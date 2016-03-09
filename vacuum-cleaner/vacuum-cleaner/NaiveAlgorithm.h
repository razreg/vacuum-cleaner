#pragma once
#include "AbstractAlgorithm.h"
#include <stdlib.h>
#include <ctime>
#include <vector>

using namespace std;

class NaiveAlgorithm : public AbstractAlgorithm {

	AbstractSensor sensor; // TODO fix

public:

	NaiveAlgorithm() {
		srand(time(NULL));
	};

	void setSensor(AbstractSensor& sensor) {
		this->sensor = sensor;
	};

	Direction step();

	Direction aboutToFinish(int stepsTillFinishing) {
		// TODO
	}
};