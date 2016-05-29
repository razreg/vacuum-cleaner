#ifndef __ALGORITHM_3__H_
#define __ALGORITHM_3__H_

#include "AstarAlgorithm.h"
#include "uniqueptr.h"
#include "AlgorithmRegistration.h"

class _305623571_C : public AstarAlgorithm {

	Direction pickPrefered(unsigned int directions, Direction& preferNext);

protected:

	virtual void resetPreferNext() override {
		setPreferNext(WEST);
	};

	virtual Direction chooseSimpleDirection() override;

	virtual Direction chooseSimpleDirectionToBlack() override;

	virtual bool isReturnTripFeasable(size_t pathLength) override {
		return AstarAlgorithm::isReturnTripFeasable(pathLength*1.05); // somewhat conservative
	};
	
};

#endif // __ALGORITHM_3__H_