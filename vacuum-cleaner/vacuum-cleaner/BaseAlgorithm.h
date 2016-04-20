#ifndef __BASE_ALGORITHM__H_
#define __BASE_ALGORITHM__H_

#include "AbstractAlgorithm.h"

#include "Direction.h"
#include "Battery.h"
#include "Common.h"

using namespace std;

// This class is virtual ("abstract") - cannot be instantiated
class BaseAlgorithm : public AbstractAlgorithm {

protected:
	const AbstractSensor* sensor;
	Battery battery;
	size_t stepsLeft;
	Direction lastDirection;

	// we start with 3 so the algorithm will start going east 
	// but it doesn't really matter (set it to whatever)
	size_t directionCounter = 3;
	bool configured = false;
	vector<int> directionsPermutation;

	virtual void restartAlgorithm() {
		if (configured) {
			battery.setCurrValue(battery.getCapacity());
		}
		directionCounter = 3;
		vector<int> perm = { 0, 2, 1, 3 };
		directionsPermutation = move(perm);
		stepsLeft = numeric_limits<size_t>::max();
		lastDirection = Direction::Stay;
	};

public:

	virtual void setSensor(const AbstractSensor& sensor) override {
		this->sensor = &sensor;
		restartAlgorithm();
	};

	virtual void setConfiguration(map<string, int> config) override {
		battery.setCapacity(max(0, config.find(BATTERY_CAPACITY)->second));
		battery.setCurrValue(battery.getCapacity());
		battery.setConsumptionRate(max(0, config.find(BATTERY_CONSUMPTION_RATE)->second));
		battery.setRechargeRate(max(0, config.find(BATTERY_RECHARGE_RATE)->second));
		configured = true;
	};

	virtual Direction step() override = 0;

	virtual void aboutToFinish(int stepsTillFinishing) override {
		stepsLeft = stepsTillFinishing > 0 ? stepsTillFinishing : 0;
	};

	virtual bool inDockingStation() const = 0;

	virtual bool isReturnTripFeasable(size_t moves) const {
		return moves <= stepsLeft && // There are enough steps
			battery.getCurrValue() >= moves * battery.getConsumptionRate(); // The battery will suffice
	};

	// updates the vector used to return back to docking station
	virtual void storeDataForReturnTrip(Direction direction) = 0;

};

#endif // __BASE_ALGORITHM__H_