#pragma once
#include "AbstractAlgorithm.h"
#include <stdlib.h>
#include <ctime>
#include <vector>

using namespace std;

class NaiveAlgorithm : public AbstractAlgorithm {

	AbstractSensor *sensor;

public:

	NaiveAlgorithm() {
		srand(time(NULL));
	};

	void setSensor(AbstractSensor& sensor) override {
		this->sensor = &sensor;
	};

	void setConfiguration(map<string, int> config) override;

	Direction step() override;

	void aboutToFinish(int stepsTillFinishing) override {}
};