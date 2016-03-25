#ifndef __ROBOT__H_
#define __ROBOT__H_

#include "SensorImpl.h"
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
	House* house;
	SensorImpl sensor;
	Battery battery;
	Position position;
	bool illegalStepPerformed = false;
	bool batteryDead = false;
	bool finished = false;

	void configBattery(const map<string, int>& configMap) {
		battery.setCapacity(configMap.find(BATTERY_CAPACITY)->second);
		battery.setConsumptionRate(configMap.find(BATTERY_CONSUMPTION_RATE)->second);
		battery.setRechargeRate(configMap.find(BATTERY_RECHARGE_RATE)->second);
		battery.setCurrValue(battery.getCapacity());
	};

public:

	Robot(const map<string, int>& configMap, AbstractAlgorithm& algorithm, House* house) :
		algorithm(algorithm), house(house) {
		configBattery(configMap);
		setHouse(house);
		this->algorithm.setConfiguration(configMap);
		this->algorithm.setSensor(this->sensor);
	};

	Robot(const map<string, int>& configMap, AbstractAlgorithm& algorithm) : algorithm(algorithm) {
		configBattery(configMap);
		this->algorithm.setConfiguration(configMap);
		this->algorithm.setSensor(this->sensor);
	};

	void setHouse(House* house) {
		this->house = house;
		sensor.setHouse(*this->house); 
		position = this->house->getDockingStation(); // copy constructor
		sensor.setPosition(position);
	}

	void restart() {
		battery.setCurrValue(battery.getCapacity());
		illegalStepPerformed = false;
		batteryDead = false;
	};

	House& getHouse() {
		return *house;
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

	void aboutToFinish(int stepsTillFinishing) {
		algorithm.aboutToFinish(stepsTillFinishing);
	};

	void reportBadBehavior() {
		illegalStepPerformed = true;
	};

	bool performedIllegalStep() {
		return illegalStepPerformed;
	};

	string getAlgorithmName() {
		string algoName = typeid(algorithm).name();
		return algoName.substr(algoName.find_last_of(' ') + 1);
	};

	void setBatteryDeadNotified() {
		batteryDead = true;
	};

	bool isBatteryDeadNotified() {
		return batteryDead;
	};

	bool isFinished() {
		return finished;
	};

	void setFinished() {
		finished = true;
	};

};

#endif // __ROBOT__H_