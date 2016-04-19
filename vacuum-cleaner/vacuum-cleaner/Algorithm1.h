#ifndef __ALGORITHM_1__H_
#define __ALGORITHM_1__H_

#include <memory>

#include "AbstractAlgorithm.h"
#include "AlgorithmRegistration.h"

#include "Direction.h"
#include "Battery.h"
#include "Common.h"

using namespace std;

class Algorithm1 : public AbstractAlgorithm {

	vector<Direction> movesBack;
	const AbstractSensor* sensor;
	Battery battery;
	size_t stepsLeft;
	Direction lastDirection;

	// we start with 3 so the algorithm will start going east but it doesn't really matter (set it to whatever)
	size_t directionCounter = 3;
	bool configured = false;
	vector<int> directionsPermutation;

	void restartAlgorithm() {
		if (configured) {
			battery.setCurrValue(battery.getCapacity());
		}
		movesBack.clear();
		directionCounter = 3;
		vector<int> perm = { 0, 2, 1, 3 };
		directionsPermutation = move(perm);
		stepsLeft = numeric_limits<size_t>::max();
	};

public:

	void setSensor(const AbstractSensor& sensor) override {
		this->sensor = &sensor;
		restartAlgorithm();
	};

	void setConfiguration(map<string, int> config) override {
		battery.setCapacity(max(0, config.find(BATTERY_CAPACITY)->second));
		battery.setCurrValue(battery.getCapacity());
		battery.setConsumptionRate(max(0, config.find(BATTERY_CONSUMPTION_RATE)->second));
		battery.setRechargeRate(max(0, config.find(BATTERY_RECHARGE_RATE)->second));
		configured = true;
	};

	Direction step() override;

	void aboutToFinish(int stepsTillFinishing) override {
		stepsLeft = stepsTillFinishing > 0 ? stepsTillFinishing : 0;
	};

	bool inDockingStation() {
		return movesBack.empty();
	};

	bool isReturnTripFeasable(size_t moves) {
		return moves <= stepsLeft && // There are enough steps
			!battery.empty() && battery.getCurrValue() >= moves * battery.getConsumptionRate(); // The battery will suffice
	};

	// updates the vector used to return back to docking station
	void updateMovesBack(Direction direction) {
		movesBack.push_back(
			direction == Direction::North ? Direction::South :
			direction == Direction::South ? Direction::North :
			direction == Direction::East ? Direction::West :
			direction == Direction::West ? Direction::East :
			Direction::Stay
		);
	}

};

#endif // __ALGORITHM_1__H_
