#ifndef __ALGORITHM_2__H_
#define __ALGORITHM_2__H_

#include "AbstractAlgorithm.h"
#include "AlgorithmRegistration.h"

#include "Direction.h"
#include "Battery.h"
#include "Common.h"

using namespace std;

class Algorithm2 : public AbstractAlgorithm {

	static const int MAX_BYPASS = 4;

	const AbstractSensor* sensor;
	Battery battery;
	size_t stepsLeft;
	Direction lastDirection;
	int xPos, yPos; // coordinates relative to docking station

	// we start with 3 so the algorithm will start going east but it doesn't really matter (set it to whatever)
	size_t directionCounter = 3;
	bool configured = false;
	vector<int> directionsPermutation;

	// lets the algorithm know that it is now in bypass mode and should not try to return 
	// in manhatten distance even if in need to get to docking station
	int stepsToBypassWall;

	void restartAlgorithm() {
		if (configured) {
			battery.setCurrValue(battery.getCapacity());
		}
		directionCounter = 3;
		vector<int> perm = { 0, 2, 1, 3 };
		directionsPermutation = move(perm);
		stepsLeft = numeric_limits<size_t>::max();
		xPos = yPos = 0;
		stepsToBypassWall = 0;
	};

	Direction returnToDocking(SensorInformation& sensorInformation);

	Direction keepCleaningOutOfDocking(SensorInformation& sensorInformation);

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

	bool inDockingStation() const {
		return xPos == 0 && yPos == 0;
	};

	bool isReturnTripFeasable(size_t moves) const {
		return moves <= stepsLeft && // There are enough steps
			!battery.empty() && battery.getCurrValue() >= moves * battery.getConsumptionRate(); // The battery will suffice
	};

	// updates the vector used to return back to docking station
	void storeDataForReturnTrip(Direction direction) {
		xPos += direction == Direction::East ? 1 : direction == Direction::West ? -1 : 0;
		yPos += direction == Direction::South ? 1 : direction == Direction::North ? -1 : 0;
	}

};

#endif // __ALGORITHM_2__H_
