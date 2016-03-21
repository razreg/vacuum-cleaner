#include "Simulator.h"

using namespace std;

Logger Simulator::logger = Logger("Simulator");

// TODO add debug printing (mark as "DEBUG: ". It would be good to add simple logging in common.h with timestamps
void Simulator::start() {

	int maxSteps = configMap.find(MAX_STEPS)->second;
	int maxStepsAfterWinner = configMap.find(MAX_STEPS_AFTER_WINNER)->second;
	logger.info("MaxSteps=" + to_string(maxSteps) + ", MaxStepsAfterWinner=" + to_string(maxStepsAfterWinner));

	NaiveAlgorithm algorithm;

	// TODO catch exceptions that might come from algorithm or something else and exit gracefully (but be more specific)
	logger.info("Starting simulation of algorithm: NaiveAlgorithm");
	int i = 1;
	for (list<House>::const_iterator it = houseList.begin(), end = houseList.end(); it != end; ++it, ++i) {

		House currHouse(*it); // copy constructor called
		logger.info("Simulation started for house number [" + to_string(i) + "] - Name: " + currHouse.getShortName());
		// TODO implement a method to print the house for debug purposes (place in House)
		SensorImpl sensor;
		sensor.setHouse(currHouse);
		Score currScore;

		logger.info("Initializing robot");
		Robot robot(configMap, algorithm, sensor, currHouse.getDockingStation());
		int steps = 0;
		int stepsAfterWinner = 0;

		while (steps < maxSteps && stepsAfterWinner < maxStepsAfterWinner) {
			if (robot.getBatteryValue() == 0) {
				// dead battery - we fast forward now to the point when time is up
				logger.info("Robot has dead battery");
				steps = maxSteps;
				break;
			}
			robot.step(); // this also updates the sensor and the battery but not the house
			if (!currHouse.isInside(robot.getPosition()) || currHouse.isWall(robot.getPosition())) {
				logger.warn("The algorithm has performed an illegal step.");
				currScore.reportBadBehavior();
				break;
			}
			// perform one cleaning step and update score
			if (currHouse.clean(robot.getPosition())) {
				currScore.incrementDirtCollected();
			}
			steps++; // TODO increment stepsAfterWinner if winner were found (and update score)
			if (currHouse.getTotalDust() == 0) {
				logger.info("House is clean of dust");
				break; // clean house
			}
			// TODO notify aboutToFinish to algorithm if necessary
		}
		currScore.setWinnerNumSteps(steps); // TODO change to support more than one algorithm in ex2
		currScore.setIsBackInDocking(robot.inDocking());
		currScore.setThisNumSteps(steps);
		currScore.setSumDirtInHouse(currHouse.getTotalDust());
		if (currHouse.getTotalDust() > 0) {
			currScore.setPositionInCopmetition(DIDNT_FINISH_POSITION_IN_COMPETETION);
		}
		else {
			currScore.setPositionInCopmetition(1); // TODO change to support more than one algorithms in ex2
		}
		logger.info("Score for house " + currHouse.getShortName() + " is " + to_string(currScore.getScore()));
	}
	// TODO return score and print in main
}
