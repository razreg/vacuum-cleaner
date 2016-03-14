#include "NaiveAlgorithm.h"


void NaiveAlgorithm::setConfiguration(map<string, int> config){

	map<string, int>::iterator mapIterator;

	mapIterator = config.find("MaxSteps");
	maxSteps = (mapIterator != config.end()) ? mapIterator->second : DEFAULT_MAX_STEPS;
	mapIterator = config.find("MaxStepsAfterWinner");
	maxStepsAfterWinner = (mapIterator != config.end()) ? mapIterator->second : DEFAULT_MAX_STEPS_AFTER_WINNER;

	mapIterator = config.find("BatteryCapacity");
	battery.setCapacity((mapIterator != config.end()) ? mapIterator->second : DEFAULT_BATTERY_CAPACITY);

	mapIterator = config.find("BatteryConsumptionRate");
	battery.setConsumptionRate((mapIterator != config.end()) ? mapIterator->second : DEFAULT_BATTERY_CONSUMPTION_RATE);
	
	mapIterator = config.find("BatteryRechargeRate");
	battery.setRechargeRate((mapIterator != config.end()) ? mapIterator->second : DEFAULT_BATTERY_RECHARGE_RATE);
}

Direction NaiveAlgorithm::step() {

	SensorInformation sensorInformation = this->sensor->sense();

	// construct a vector of valid directions
	vector<Direction> directions;
	directions.push_back(Direction::Stay);
	for (int i = 0; i < static_cast<int>(Direction::Stay); i++) {
		if (!sensorInformation.isWall[i]) {
			directions.push_back(static_cast<Direction>(i));
		}
	}

	return directions[rand() % directions.size()];
}