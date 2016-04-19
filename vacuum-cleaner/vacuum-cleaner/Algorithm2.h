#ifndef __ALGORITHM_2__H_
#define __ALGORITHM_2__H_

#include "AbstractAlgorithm.h"
#include "AlgorithmRegistration.h"

using namespace std;

class Algorithm2 : public AbstractAlgorithm {
public:

	Algorithm2() {
		cout << "in Algorithm2 ctor" << endl;
	};

	void setSensor(const AbstractSensor& sensor) override {
		cout << "Algorithm2 - setSensor" << endl;
	};

	void setConfiguration(map<string, int> config) override {
		cout << "Algorithm2 - setConfiguration" << endl;
	};

	Direction step() override {
		cout << "Algorithm2 - in step" << endl;
		return Direction::Stay;
	};

	void aboutToFinish(int stepsTillFinishing) override {
		cout << "Algorithm2 - in aboutToFinish" << endl;
	};
};

#endif // __ALGORITHM_2__H_
