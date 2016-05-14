#ifndef __ALGORITHM_3__H_
#define __ALGORITHM_3__H_

#include "AstarAlgorithm.h"
#include "uniqueptr.h"
#include "AlgorithmRegistration.h"

class _305623571_C : public AstarAlgorithm {

	size_t rotatingCounter = 3;

	void incRotate() {
		rotatingCounter = (rotatingCounter + 1) % 4;
	};

protected:

	virtual void restartAlgorithm() override {
		AstarAlgorithm::restartAlgorithm();
		rotatingCounter = 0;
	};

	virtual Direction chooseSimpleDirectionToBlack() override;

	virtual Direction chooseSimpleDirection() override;

	virtual bool isReturnTripFeasable(size_t pathLength) override {
		return AstarAlgorithm::isReturnTripFeasable(pathLength + 8); // conservative - don't end up with no battery!
	};

public:

	virtual bool keepMoving(SensorInformation& sensorInformation) const override {
		return sensorInformation.dirtLevel <= 1;
	};
};

#endif // __ALGORITHM_3__H_