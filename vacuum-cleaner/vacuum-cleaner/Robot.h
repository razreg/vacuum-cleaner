#ifndef __ROBOT__H_
#define __ROBOT__H_

#include <utility>

#include "SensorImpl.h"
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "House.h"
#include "Battery.h"
#include "Position.h"
#include "Common.h"

static const string directions[] = { "East", "West", "South", "North", "Stay" };

// please note that this robot is part of the simulator - it only encapsulates some simulator logic. 
// Therefore, this robot is aware of the sensor implementation just like the simulator should be.
class Robot {

	static Logger logger;

	AbstractAlgorithm& algorithm;
	string algorithmName;
	House house;
	SensorImpl sensor;
	Battery battery;
	Position position;
	bool illegalStepPerformed = false;
	bool batteryDead = false;
	bool finished = false;

	void configBattery(const map<string, int>& configMap) {
		battery.setCapacity(max(0, configMap.find(BATTERY_CAPACITY)->second));
		battery.setConsumptionRate(max(0, configMap.find(BATTERY_CONSUMPTION_RATE)->second));
		battery.setRechargeRate(max(0, configMap.find(BATTERY_RECHARGE_RATE)->second));
		battery.setCurrValue(battery.getCapacity());
	};

	void updateSensorWithHouse() {
		sensor.setHouse(this->house);
		position = this->house.getDockingStation(); // copy constructor
		sensor.setPosition(position);
		algorithm.setSensor(sensor);
	};

	void configure(const map<string, int>& configMap) {
		configBattery(configMap);
		this->algorithm.setConfiguration(configMap);
		this->algorithm.setSensor(this->sensor);
	}

public:

	Robot(const map<string, int>& configMap, AbstractAlgorithm& algorithm, string algorithmName, House&& house) :
		algorithm(algorithm), algorithmName(algorithmName), house(house) {
		updateSensorWithHouse();
		configure(configMap);
	};

	Robot(const map<string, int>& configMap, AbstractAlgorithm& algorithm, string algorithmName) : 
		algorithm(algorithm), algorithmName(algorithmName) {
		configure(configMap);
	};

	void setHouse(House&& house) {
		this->house = forward<House>(house);
		updateSensorWithHouse();
	};

	void restart() {
		battery.setCurrValue(battery.getCapacity());
		illegalStepPerformed = false;
		batteryDead = false;
		finished = false;
	};

	House& getHouse() {
		return house;
	};

	Position getPosition() const {
		return position;
	};

	size_t getBatteryValue() const {
		return battery.getCurrValue();
	};

	void step();

	bool inDocking() const {
		return sensor.inDocking();
	};

	void aboutToFinish(int stepsTillFinishing) {
		algorithm.aboutToFinish(stepsTillFinishing);
	};

	void reportBadBehavior() {
		illegalStepPerformed = true;
	};

	bool performedIllegalStep() const {
		return illegalStepPerformed;
	};

	string getAlgorithmName() const {
		return algorithmName;
	};

	void setBatteryDeadNotified() {
		batteryDead = true;
	};

	bool isBatteryDeadNotified() const {
		return batteryDead;
	};

	bool isFinished() const {
		return finished;
	};

	void setFinished() {
		finished = true;
	};

};

#endif // __ROBOT__H_