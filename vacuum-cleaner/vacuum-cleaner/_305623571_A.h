#ifndef __ALGORITHM_1__H_
#define __ALGORITHM_1__H_

#include "AstarAlgorithm.h"
#include "uniqueptr.h"
#include "AlgorithmRegistration.h"

using namespace std;

class _305623571_A : public AstarAlgorithm {

protected:

	virtual Direction chooseSimpleDirectionToBlack() override;

	virtual Direction chooseSimpleDirection() override;

public:

	virtual bool keepMoving(SensorInformation& sensorInformation) const override {
		return sensorInformation.dirtLevel == 0; // keep cleaning if there's dust
	};
};

#endif // __ALGORITHM_1__H_
