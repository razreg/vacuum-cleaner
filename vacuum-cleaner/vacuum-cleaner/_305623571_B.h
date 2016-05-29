#ifndef __ALGORITHM_2__H_
#define __ALGORITHM_2__H_

#include "AstarAlgorithm.h"
#include "uniqueptr.h"
#include "AlgorithmRegistration.h"

using namespace std;

class _305623571_B : public AstarAlgorithm {

	Direction pickPrefered(unsigned int directions, Direction& preferNext);

protected:

	virtual void resetPreferNext() override {
		setPreferNext(WEST);
	};

	virtual Direction chooseSimpleDirection() override;

	virtual Direction chooseSimpleDirectionToBlack() override;

	virtual bool isReturnTripFeasable(size_t pathLength) override {
		return AstarAlgorithm::isReturnTripFeasable(pathLength*1.1); // conservative
	};
};

#endif // __ALGORITHM_2__H_
