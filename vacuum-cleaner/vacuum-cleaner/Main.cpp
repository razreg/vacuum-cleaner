#include "Simulator.h"

using namespace std;

// TODO add debug printing (mark as "DEBUG: ". It would be good to add simple logging in common.h with timestamps
int main(int argc, char** argv) {

	Logger logger("Simulator");
	string usage = "Usage: simulator [-config <config_file_location>] [-house_path <houses_path_location>]";

	//creating a simulator object
	Simulator simulator;

	// set paths to config file and houses
	string workingDir;
	try {
		workingDir = getCurrentWorkingDirectory(); //getting full path to working directoy
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INTERNAL_FAILURE;
	}
	string configPath = workingDir;
	string housesPath = workingDir;
	logger.debug("Parsing command line arguments");
	for (int i = 1; i < argc; ++i) {
		if (argv[i] == "-config") {
			configPath = argv[i + 1];
		}
		else if (argv[i] == "-house_path") {
			housesPath = argv[i + 1];
		}
		else if (i == 2 || i == 4) {
			// this argument is the first or third argument and not "-config" or "-house_path"
			logger.fatal("Invalid arguments. " + usage);
			return INVALID_ARGUMENTS;
		}
	}
	logger.info("Using config file directory path as [" + configPath + "]");
	logger.info("Using house files directory path as [" + housesPath + "]");
	try {
		logger.info("Loading houses from directory");
		simulator.setHouseList(housesPath);
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INVALID_ARGUMENTS;
	}
	try {
		logger.info("Loading configuration from directory");
		simulator.setConfiguration(configPath);
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INVALID_CONFIGURATION;
	}

	list<House> houseList = simulator.getHouseList();
	map<string, int> configMap = simulator.getConfigMap();

	int maxSteps = configMap.find(MAX_STEPS)->second;
	int maxStepsAfterWinner = configMap.find(MAX_STEPS_AFTER_WINNER)->second;
	logger.info("MaxSteps=" + to_string(maxSteps) + ", MaxStepsAfterWinner=" + to_string(maxStepsAfterWinner));

	NaiveAlgorithm algorithm;

	// TODO catch exceptions that might come from algorithm or something else and exit gracefully (but be more specific)
	logger.info("Starting simulation of algorithm: NaiveAlgorithm");
	for (list<House>::const_iterator it = houseList.begin(), end = houseList.end(), int i = 1; it != end; ++it, ++i) {

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
	// TODO print score

	return SUCCESS;
}

string getCurrentWorkingDirectory() {
	char currentPath[FILENAME_MAX];
	if (!getCurrentWorkingDir(currentPath, sizeof(currentPath))) {
		throw exception("Failed to find current working directory.");
	}
	currentPath[sizeof(currentPath) - 1] = '\0';
	return currentPath;
}