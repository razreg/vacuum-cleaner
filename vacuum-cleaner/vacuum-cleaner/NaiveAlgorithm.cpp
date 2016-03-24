#include "NaiveAlgorithm.h"

Logger NaiveAlgorithm::logger = Logger("NaiveAlogorithm");

void NaiveAlgorithm::setConfiguration(map<string, int> config){
	maxSteps = config.find(MAX_STEPS)->second;
	maxStepsAfterWinner = config.find(MAX_STEPS_AFTER_WINNER)->second;
}

Direction NaiveAlgorithm::step() {

	SensorInformation sensorInformation = sensor->sense();

	// construct a vector of valid directions
	vector<Direction> directions;
	directions.push_back(Direction::Stay);
	for (int i = 0; i < static_cast<int>(Direction::Stay); ++i) {
		if (!sensorInformation.isWall[i]) {
			directions.push_back(static_cast<Direction>(i));
		}
	}
	return directions[rand() % directions.size()];
}