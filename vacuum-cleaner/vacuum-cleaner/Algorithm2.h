#ifndef __ALGORITHM_2__H_
#define __ALGORITHM_2__H_

#include "ManhattenAlgorithm.h"
#include "AlgorithmRegistration.h"

using namespace std;

class Algorithm2 : public ManhattenAlgorithm {
protected:
	virtual bool isReadyToMoveOn(SensorInformation& sensorInformation) const override {
		return sensorInformation.dirtLevel == 0;
	};
};

#endif // __ALGORITHM_2__H_
