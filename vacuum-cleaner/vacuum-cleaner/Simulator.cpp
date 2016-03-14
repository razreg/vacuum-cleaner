#include "Simulator.h"

using namespace std;
namespace fs = boost::filesystem;

// TODO add debug printing (mark as "DEBUG: ". It would be good to add simple logging in common.h with timestamps
int main(int argc, char** argv) {
	
	string usage = "Usage: simulator [-config <config_file_location>] [-house_path <houses_path_location>]";

	string housesPath;
	string configPath;
	list<House> houseList;
	map<string, int> configMap;
	
	// set paths to config file and houses
	string workingDir(fs::current_path()); //getting full path to working directoy
	configPath = workingDir;
	housesPath = workingDir;
	for (int i = 1; i < argc; ++i) {
		if (argv[i] == "-config") {
			configPath = argv[i + 1];
		}
		else if (argv[i] == "-house_path") {
			housesPath = argv[i + 1];
		}
		else if (i == 2 || i == 4) {
			// this argument is the first or third argument and not "-config" or "-house_path"
			cout << "Error: invalid arguments. " << usage << endl;
			return INVALID_ARGUMENTS;
		}
	}
	getHouseList(housesPath, houseList); // TODO catch exception and exit gracefully?
	getConfiguration(configPath, configMap); // TODO catch exception and exit gracefully?

	int maxSteps = configMap.find(MAX_STEPS)->second;
	int maxStepsAfterWinner = configMap.find(MAX_STEPS_AFTER_WINNER)->second;
	
	NaiveAlgorithm algorithm;

	// TODO catch exceptions that might come from algorithm or something else and exit gracefully
	for (list<House>::const_iterator it = houseList.begin(), end = houseList.end(); it != end; ++it) {

		House currHouse(*it); // copy constructor called
		SensorImpl sensor;
		sensor.setHouse(currHouse);
		Score currScore;

		Robot robot(configMap, algorithm, sensor, currHouse.getDockingStation());
		int steps = 0;
		int stepsAfterWinner = 0;

		while (steps < maxSteps && stepsAfterWinner < maxStepsAfterWinner) {
			if (robot.getBatteryValue() == 0) {
				// dead battery - we fast forward now to the point when time is up
				steps = maxSteps;
				break;
			}
			robot.step(); // this also updates the sensor and the battery but not the house
			// TODO log step (INFO)
			if (!currHouse.isInside(robot.getPosition()) || currHouse.isWall(robot.getPosition())) {
				cout << "ERROR: The algorithm has performed an illegal step." << endl;
				currScore.reportBadBehavior();
				break;
			}
			// perform one cleaning step and update score
			if (currHouse.clean(robot.getPosition())) {
				currScore.incrementDirtCollected();
			}
			steps++; // TODO increment stepsAfterWinner if winner were found (and update score)
			if (currHouse.getTotalDust() == 0) {
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
	}
	// TODO print score

	return SUCCESS;
}

void getConfiguration(const string& configFileDir, map<string, int>& configMap) {
	
	string configError = "Error: configuration file directoy [" + configFileDir + "] is invalid";

	fs::path path = configFileDir; // TODO check if safe or might throw exception
	path /= "config.ini"; // adds appropriate file separator if needed

	// create a map of key-value pairs from config file (expected format of each line: key=value)
	ifstream configFileStream(path.native());
	string currLine;
	bool failedToParseConfig = true; // assume we are going to fail
	if (configFileStream) {
		failedToParseConfig = false; // seems like we're lucky
		try {
			while (getline(configFileStream, currLine)) {
				int positionOfEquals = currLine.find("=");
				string key = currLine.substr(0, positionOfEquals);
				if (positionOfEquals != string::npos) {
					int value = stoi(currLine.substr(positionOfEquals + 1)); // possibly: invalid_argument or out_of_range
					configMap.insert(pair<string, int>(key, value));
				}
			}
			configFileStream.close();
		}
		catch (exception e) {
			failedToParseConfig = true; // not so lucky after all
		}
	}
	if (failedToParseConfig) {
		cout << configError << endl; // TODO throw custom exception (create our own class - perhaps even in the simulator header)
	}

	// fix map with defaults if missing configuration item
	map<string, int>::iterator mapIterator;
	mapIterator = configMap.find(MAX_STEPS);
	if (mapIterator == configMap.end()) {
		configMap.insert(pair<string, int>(MAX_STEPS, DEFAULT_MAX_STEPS));
	}
	mapIterator = configMap.find(MAX_STEPS_AFTER_WINNER);
	if (mapIterator == configMap.end()) {
		configMap.insert(pair<string, int>(MAX_STEPS_AFTER_WINNER, DEFAULT_MAX_STEPS_AFTER_WINNER));
	}
	mapIterator = configMap.find(BATTERY_CAPACITY);
	if (mapIterator == configMap.end()) {
		configMap.insert(pair<string, int>(BATTERY_CAPACITY, DEFAULT_BATTERY_CAPACITY));
	}
	mapIterator = configMap.find(BATTERY_CONSUMPTION_RATE);
	if (mapIterator == configMap.end()) {
		configMap.insert(pair<string, int>(BATTERY_CONSUMPTION_RATE, DEFAULT_BATTERY_CONSUMPTION_RATE));
	}
	mapIterator = configMap.find(BATTERY_RECHARGE_RATE);
	if (mapIterator == configMap.end()) {
		configMap.insert(pair<string, int>(BATTERY_RECHARGE_RATE, DEFAULT_BATTERY_RECHARGE_RATE));
	}
}


void getHouseList(string housePath, list<House>& houses) {

	string houseError = "Error: house file's path [" + housePath + "] is invalid";

	int i = 0;
	const boost::regex pattern("(.*)\.house");
	fs::directory_iterator endIterator;
	boost::smatch what;
	try {
		for (fs::directory_iterator iter(housePath); iter != endItr; ++iter) {
			if (fs::is_regular_file(iter->status()) &&
				boost::regex_match(iter->path().filename(), what, pattern)) {
				House& house = House::deseriallize(iter->path().string());
				house.getDockingStation(); // TODO handle exception (this line verifies there is a D in the house)
				house.validateWalls(); // TODO handle exception if docking station overriden
				houses.push_back(house);
			}
		}
	}
	catch (fs::filesystem_error e) {
		cout << houseError << endl; // TODO throw custom exception (create our own class - perhaps even in the simulator header)
	}
}

