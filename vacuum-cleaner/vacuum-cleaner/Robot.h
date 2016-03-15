#ifndef __ROBOT__H_
#define __ROBOT__H_

#include "AbstractSensor.h"
#include "House.h"
#include "AbstractAlgorithm.h"
#include "Battery.h"

Logger logger("Robot");
static const string directions[] = { "East", "West", "South", "North", "Stay" };

// please note that this robot is part of the simulator - it only encapsulates some simulator logic. 
// Therefore, this robot is aware of the sensor implementation just like the simulator is
class Robot {

	AbstractAlgorithm& algorithm;
	SensorImpl& sensor;
	Battery battery;
	Position position;

public:

	Robot(const map<string, int>& configMap, AbstractAlgorithm& algorithm, SensorImpl& sensor, const Position position) : 
		algorithm(algorithm), sensor(sensor), position(position) {
		battery.setCapacity(configMap.find(BATTERY_CAPACITY)->second);
		battery.setConsumptionRate(configMap.find(BATTERY_CONSUMPTION_RATE)->second);
		battery.setRechargeRate(configMap.find(BATTERY_RECHARGE_RATE)->second);
		algorithm.setConfiguration(configMap);
		algorithm.setSensor(sensor);
	};

	Position getPosition() const {
		return position;
	};

	int getBatteryValue() {
		return battery.getCurrValue();
	};

	int chargeBattery() {
		return battery.charge();
	};

	int comsumeBattery() {
		return battery.consume();
	};

	void step();

	bool inDocking() const {
		return sensor.inDocking();
	};

};

#endif // __ROBOT__H_