#include "NaiveAlgorithm.h"


// raz- I just copied what you did in simulator.cpp to here.. hope I am not wrong!
void NaiveAlgorithm::setConfiguration(map<string, int> config){
	Configuration con;
	map<string, int>::iterator mapIterator;
	mapIterator = config.find("MaxSteps");
	con.maxSteps =
		(mapIterator != config.end()) ? mapIterator->second : DEFAULT_MAX_STEPS;
	mapIterator = config.find("MaxStepsAfterWinner");
	con.maxStepsAfterWinner =
		(mapIterator != config.end()) ? mapIterator->second : DEFAULT_MAX_STEPS_AFTER_WINNER;
	mapIterator = config.find("BatteryCapacity");
	con.batteryCapacity =
		(mapIterator != config.end()) ? mapIterator->second : DEFAULT_BATTERY_CAPACITY;
	mapIterator = config.find("BatteryConsumptionRate");
	con.batteryConsumptionRate =
		(mapIterator != config.end()) ? mapIterator->second : DEFAULT_BATTERY_CONSUMPTION_RATE;
	mapIterator = config.find("BatteryRachargeRate");
	con.batteryRachargeRate =
		(mapIterator != config.end()) ? mapIterator->second : DEFAULT_BATTERY_RECHARGE_RATE;
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