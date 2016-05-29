#include "Robot.h"

Logger Robot::logger = Logger("Robot");

void Robot::step() {

	if (!sensor.inDocking()) {
		battery.consume();
	}

	prevStep = algorithm.step(prevStep);
	//if (logger.debugEnabled()) logger.debug("Algorithm chose step: " + directions[static_cast<int>(prevStep)]);

	if (prevStep == Direction::East) {
		position.moveEast();
	}
	else if (prevStep == Direction::West) {
		position.moveWest();
	}
	else if (prevStep == Direction::South) {
		position.moveSouth();
	}
	else if (prevStep == Direction::North) {
		position.moveNorth();
	}
	sensor.setPosition(position);

	if (sensor.inDocking()) {
		//if (logger.debugEnabled()) logger.debug("Robot in docking station (charging...)");
		battery.charge();
	}
}

void Robot::configBattery(const map<string, int>& configMap) {
	battery.setCapacity(max(0, configMap.find(BATTERY_CAPACITY)->second));
	battery.setConsumptionRate(max(0, configMap.find(BATTERY_CONSUMPTION_RATE)->second));
	battery.setRechargeRate(max(0, configMap.find(BATTERY_RECHARGE_RATE)->second));
	battery.setCurrValue(battery.getCapacity());
}

void Robot::updateSensorWithHouse() {
	sensor.setHouse(this->house);
	position = this->house.getDockingStation(); // copy constructor
	sensor.setPosition(position);
	algorithm.setSensor(sensor);
}

void Robot::configure(const map<string, int>& configMap) {
	configBattery(configMap);
	this->algorithm.setConfiguration(configMap);
	this->algorithm.setSensor(this->sensor);
}

void Robot::restart() {
	battery.setCurrValue(battery.getCapacity());
	prevStep = Direction::Stay;
	illegalStepPerformed = false;
	batteryDead = false;
}

void Robot::setHouse(House&& house) {
	this->house = forward<House>(house);
	updateSensorWithHouse();
	if (captureVideo) video.init(house.getNumRows(), house.getNumCols(), house.getName(), algorithmName);
}