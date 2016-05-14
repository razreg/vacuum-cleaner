#ifndef __ALGORITHM_2__H_
#define __ALGORITHM_2__H_

#include "AstarAlgorithm.h"
#include "uniqueptr.h"
#include "AlgorithmRegistration.h"

using namespace std;

class _305623571_B : public AstarAlgorithm {

protected:

	virtual Direction chooseSimpleDirectionToBlack() override;

	virtual Direction chooseSimpleDirection() override;

public:

	virtual bool keepMoving(SensorInformation& sensorInformation) const override {
		return true; // hyperactive
	};
};

#endif // __ALGORITHM_2__H_
