#ifndef __ALGORITHM_3__H_
#define __ALGORITHM_3__H_

#include "ManhattenAlgorithm.h"
#include "AlgorithmRegistration.h"

class _305623571_C : public ManhattenAlgorithm {
protected:
	virtual bool isReadyToMoveOn(SensorInformation& sensorInformation) const override {
		return true; // hyperactive algorithm - never stays in one place
	};
};

#endif // __ALGORITHM_3__H_