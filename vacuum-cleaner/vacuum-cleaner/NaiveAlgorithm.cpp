#include "NaiveAlgorithm.h"

void NaiveAlgorithm::setConfiguration(map<string, int> config){

	maxSteps = config.find(MAX_STEPS)->second;
	maxStepsAfterWinner = config.find(MAX_STEPS_AFTER_WINNER)->second;

	battery = new Battery(config.find(BATTERY_CAPACITY)->second, 
		config.find(BATTERY_CONSUMPTION_RATE)->second, config.find(BATTERY_RECHARGE_RATE)->second);
}

Direction NaiveAlgorithm::step() {

	SensorInformation sensorInformation = sensor->sense();

	// construct a vector of valid directions
	vector<Direction> directions;
	directions.push_back(Direction::Stay);
	for (int i = 0; i < static_cast<int>(Direction::Stay); i++) {
		if (!sensorInformation.isWall[i]) {
			directions.push_back(static_cast<Direction>(i));
		}
	}
	directions[rand() % directions.size()];

	return directions[rand() % directions.size()];
}