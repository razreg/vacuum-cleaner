#ifndef __ALGORITHM_1__H_
#define __ALGORITHM_1__H_

#include "BaseAlgorithm.h"
#include "AlgorithmRegistration.h"

using namespace std;

class Algorithm1 : public BaseAlgorithm {

	vector<Direction> movesBack;

protected:

	virtual void restartAlgorithm() override {
		BaseAlgorithm::restartAlgorithm();
		movesBack.clear();
	};

public:

	virtual Direction step() override;

	virtual bool inDockingStation() const override {
		return movesBack.empty();
	};

	// updates the vector used to return back to docking station
	virtual void storeDataForReturnTrip(Direction direction) override {
		if (direction == Direction::Stay) {
			return;
		}
		movesBack.push_back(
			direction == Direction::North ? Direction::South :
			direction == Direction::South ? Direction::North :
			direction == Direction::East ? Direction::West :
			direction == Direction::West ? Direction::East :
			Direction::Stay
		);
	};

};

#endif // __ALGORITHM_1__H_
