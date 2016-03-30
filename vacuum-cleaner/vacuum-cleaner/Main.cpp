#include "Main.h"

using namespace std;

Logger logger = Logger("Main");

// the house for this exercise is loaded from the file simple1.house, located in the current working directory 
// or in the path given in the command line argument
int main(int argc, char** argv) {

	string usage = "Usage: simulator [-config <config_file_location>] [-house_path <houses_location>]";

	list<House*> houseList;
	map<string, int> configMap;
	list<AbstractAlgorithm*> algorithms;

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
	logger.info("Parsing command line arguments");
	bool invalid = parseArgs(argc, argv, configPath, housesPath);
	if (invalid) {
		logger.fatal("Invalid arguments. " + usage);
		return INVALID_ARGUMENTS;
	}
	logger.info("Using config file directory path as [" + configPath + "]");
	logger.info("Using house files directory path as [" + housesPath + "]");
	try {
		logger.info("Loading houses from directory");
		loadHouseList(housesPath, houseList);
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INVALID_ARGUMENTS;
	}

	try {
		logger.info("Loading configuration from directory");
		loadConfiguration(configPath, configMap);
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INVALID_CONFIGURATION;
	}

	NaiveAlgorithm naiveAlgorithm;
	algorithms.push_back(&naiveAlgorithm);
	Simulator simulator(configMap, houseList, algorithms);
	try {
		simulator.execute();
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INTERNAL_FAILURE;
	}

	return SUCCESS;
}

bool parseArgs(int argc, char** argv, string& configPath, string& housesPath) {
	bool invalid = false;
	for (int i = 1; i < argc; ++i) {
		if ((string(argv[i])).compare("-config") == 0) {
			if (argc > i + 1) {
				configPath = argv[i + 1];
			}
			else {
				invalid = true;
			}
		}
		else if ((string(argv[i])).compare("-house_path") == 0) {
			if (argc > i + 1) {
				housesPath = argv[i + 1];
			}
			else {
				invalid = true;
			}
		}
		else if (i == 1 || i == 3) {
			invalid = true;
		}
		if (invalid) {
			break;
		}
	}
	return invalid;
}

string getCurrentWorkingDirectory() {
	char currentPath[FILENAME_MAX];
	if (!getCurrentWorkingDir(currentPath, sizeof(currentPath))) {
		throw invalid_argument("Failed to find current working directory.");
	}
	currentPath[sizeof(currentPath) - 1] = '\0';
	return currentPath;
}

void loadHouseList(const string& housesPath, list<House*>& houseList) {
	
	string path = housesPath;
	if (housesPath.back() != '/' && housesPath.back() != '\\') {
		path += DIR_SEPARATOR;
	}
	path += "simple1.house";

	House& house = House::deseriallize(path);
	logger.info("Validating house");
	logger.debug("Validating house walls");
	house.validateWalls();
	logger.debug("Validating the existence of exactly one docking station");
	house.validateDocking();
	logger.info("House is valid");
	houseList.push_back(&house);
}

void loadConfiguration(const string& configFileDir, map<string, int>& configMap) {
	 
	string path = configFileDir;
	if (configFileDir.back() != '/' && configFileDir.back() != '\\') {
		path += DIR_SEPARATOR;
	}
	path += "config.ini";

	// create a map of key-value pairs from config file (expected format of each line: key=value)
	ifstream configFileStream(path);
	string currLine;
	bool failedToParseConfig = true; // assume we are going to fail
	if (configFileStream) {
		failedToParseConfig = false; // seems like we're lucky
		try {
			populateConfigMap(configFileStream, configMap);
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
	fixConfig(configMap); // fix map with defaults if missing configuration item
}

void populateConfigMap(ifstream& configFileStream, map<string, int>& configMap) {
	string currLine;
	while (getline(configFileStream, currLine)) {
		logger.debug("Read line from config file: " + currLine);
		size_t positionOfEquals = currLine.find("=");
		string key = currLine.substr(0, (int)positionOfEquals);
		size_t first = key.find_first_not_of(' ');
		size_t last = key.find_last_not_of(' ');
		key = key.substr(first, (last - first + 1));
		if (positionOfEquals != string::npos) {
			int value = stoi(currLine.substr((int)positionOfEquals + 1)); // possibly: invalid_argument or out_of_range
			configMap.insert(pair<string, int>(key, max(0, value)));
		}
	}
}

void fixConfig(map<string, int>& configMap) {
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

bool endsWith(const string& housesPath, const string& suffix) {
	if (suffix.size() > housesPath.size())
		return false;
	return equal(housesPath.begin() + housesPath.size() - suffix.size(), housesPath.end(), suffix.begin());
}

