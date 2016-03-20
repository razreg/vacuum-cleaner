#include "Simulator.h"

using namespace std;
namespace fs = boost::filesystem;

/*
// TODO add debug printing (mark as "DEBUG: ". It would be good to add simple logging in common.h with timestamps
int main(int argc, char** argv) {

	Logger logger("Simulator");
	string usage = "Usage: simulator [-config <config_file_location>] [-house_path <houses_path_location>]";

	list<House> houseList;
	map<string, int> configMap;
	
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
		getHouseList(housesPath, houseList);
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INVALID_ARGUMENTS;
	}
	try {
		logger.info("Loading configuration from directory");
		getConfiguration(configPath, configMap);
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INVALID_CONFIGURATION;
	}

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

*/

void Simulator::setConfiguration(const string& configFileDir) {
	
	string path = configFileDir;
	if (configFileDir.back() != '/') {
		path += '/';
	}
	path += "config.ini";

	// create a map of key-value pairs from config file (expected format of each line: key=value)
	ifstream configFileStream(path);
	string currLine;
	bool failedToParseConfig = true; // assume we are going to fail
	if (configFileStream) {
		failedToParseConfig = false; // seems like we're lucky
		try {
			while (getline(configFileStream, currLine)) {
				logger.debug("Read line from config file: " + currLine);
				int positionOfEquals = currLine.find("=");
				string key = currLine.substr(0, positionOfEquals);
				if (positionOfEquals != string::npos) {
					int value = stoi(currLine.substr(positionOfEquals + 1)); // possibly: invalid_argument or out_of_range
					configMap.insert(pair<string, int>(key, value));
				}
			}
			configFileStream.close();
		}
		catch (exception& e) {
			failedToParseConfig = true; // not so lucky after all
		}
	}
	if (failedToParseConfig) {
		string configError = "configuration file directoy [" + configFileDir + "] is invalid";
		throw invalid_argument(configError.c_str());
	}

	// fix map with defaults if missing configuration item
	map<string, int>::iterator mapIterator;
	mapIterator = configMap.find(MAX_STEPS);
	if (mapIterator == configMap.end()) {
		configMap.insert(pair<string, int>(MAX_STEPS, DEFAULT_MAX_STEPS));
	}
	logger.info("Configuration parameter: " + MAX_STEPS + "=" + to_string(configMap.find(MAX_STEPS)->second));
	mapIterator = configMap.find(MAX_STEPS_AFTER_WINNER);
	if (mapIterator == configMap.end()) {
		configMap.insert(pair<string, int>(MAX_STEPS_AFTER_WINNER, DEFAULT_MAX_STEPS_AFTER_WINNER));
	}
	logger.info("Configuration parameter: " + MAX_STEPS_AFTER_WINNER + "=" + to_string(configMap.find(MAX_STEPS_AFTER_WINNER)->second));
	mapIterator = configMap.find(BATTERY_CAPACITY);
	if (mapIterator == configMap.end()) {
		configMap.insert(pair<string, int>(BATTERY_CAPACITY, DEFAULT_BATTERY_CAPACITY));
	}
	logger.info("Configuration parameter: " + BATTERY_CAPACITY + "=" + to_string(configMap.find(BATTERY_CAPACITY)->second));
	mapIterator = configMap.find(BATTERY_CONSUMPTION_RATE);
	if (mapIterator == configMap.end()) {
		configMap.insert(pair<string, int>(BATTERY_CONSUMPTION_RATE, DEFAULT_BATTERY_CONSUMPTION_RATE));
	}
	logger.info("Configuration parameter: " + BATTERY_CONSUMPTION_RATE + "=" + to_string(configMap.find(BATTERY_CONSUMPTION_RATE)->second));
	mapIterator = configMap.find(BATTERY_RECHARGE_RATE);
	if (mapIterator == configMap.end()) {
		configMap.insert(pair<string, int>(BATTERY_RECHARGE_RATE, DEFAULT_BATTERY_RECHARGE_RATE));
	}
	logger.info("Configuration parameter: " + BATTERY_RECHARGE_RATE + "=" + to_string(configMap.find(BATTERY_RECHARGE_RATE)->second));
}

void Simulator::setHouseList(string housesPath) {
	fs::directory_iterator endIterator;
	for (fs::directory_iterator iter(housesPath); iter != endIterator; ++iter) {
		if (fs::is_regular_file(iter->status()) && EndsWith(housesPath, ".house")) {
			logger.info("Found house file in path: " + iter->path().string());
			House& house = House::deseriallize(iter->path().string());
			logger.info("Validating house");
			logger.debug("Validating the existence of a docking station");
			house.getDockingStation();
			logger.debug("Validating house walls");
			house.validateWalls();
			logger.info("House is valid");
			houseList.push_back(house);
		}
	}
}

bool EndsWith(const string& housesPath, const string& suffix) {
	if (suffix.size() > housesPath.size())
		return false;
	return equal(housesPath.begin() + housesPath.size() - suffix.size(), housesPath.end(), suffix.begin());
}

/*

if (fs::is_regular_file(iter->status()) &&  iter->path->extension() == ".house") {


string getCurrentWorkingDirectory() {
	char currentPath[FILENAME_MAX];
	if (!getCurrentWorkingDir(currentPath, sizeof(currentPath))) {
		throw exception("Failed to find current working directory.");
	}
	currentPath[sizeof(currentPath) - 1] = '\0';
	return currentPath;
}
*/