#include "Main.h"

using namespace std;
namespace fs = boost::filesystem;

Logger logger = Logger("Main");

int main(int argc, char** argv) {

	string usage = 
		"Usage: simulator [config <config path>] [house_path <house path>] [algorithm_path <algorithm path>]";

	list<House> houseList;
	map<string, int> configMap;
	list<AbstractAlgorithm*> algorithms; // TODO don't use simple pointers!

	// set paths
	string workingDir;
	try {
		workingDir = getCurrentWorkingDirectory();
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INTERNAL_FAILURE;
	}
	string configPath = workingDir;
	string housesPath = workingDir;
	string algorithmsPath = workingDir;
	logger.info("Parsing command line arguments");
	bool isValid = parseArgs(argc, argv, configPath, housesPath, algorithmsPath);
	if (!isValid) {
		logger.fatal("Invalid arguments");
		cout << usage << endl;
		return INVALID_ARGUMENTS;
	}
	logger.info("Using config file directory path as [" + configPath + "]");
	logger.info("Using house files directory path as [" + housesPath + "]");

	// Configuration
	try {
		logger.info("Loading configuration from directory");
		isValid = loadConfiguration(configPath, configMap, usage);
		if (!isValid) {
			return INVALID_CONFIGURATION;
		}
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INVALID_CONFIGURATION;
	}

	// TODO Algorithms
	vector<string> algorithmErrors;
	NaiveAlgorithm naiveAlgorithm;
	algorithms.push_back(&naiveAlgorithm);

	// Houses
	vector<string> houseErrors;
	try {
		logger.info("Loading houses from directory");
		isValid = loadHouseList(housesPath, houseList, houseErrors, usage);
		if (!isValid) {
			return INVALID_HOUSES;
		}
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INVALID_HOUSES;
	}

	// Start simulator
	Simulator simulator(configMap, houseList, algorithms);
	try {
		simulator.execute();
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INTERNAL_FAILURE;
	}

	printErrors(houseErrors, algorithmErrors);
	return SUCCESS;
}

bool parseArgs(int argc, char** argv, string& configPath, string& housesPath, string& algorithmsPath) {
	bool valid = true;
	for (int i = 1; valid && i < argc; ++i) {
		if ((string(argv[i])).compare("-config") == 0) {
			valid = argc > i + 1;
			if (valid) {
				configPath = argv[i + 1];
			}
		}
		else if ((string(argv[i])).compare("-house_path") == 0) {
			valid = argc > i + 1;
			if (valid) {
				housesPath = argv[i + 1];
			}
		}
		else if ((string(argv[i])).compare("-algorithm_path") == 0) {
			valid = argc > i + 1;
			if (valid) {
				algorithmsPath = argv[i + 1];
			}
		}
		else if (i % 2 == 1) {
			valid = false;
		}
	}
	return valid;
}

string getCurrentWorkingDirectory() {
	char currentPath[FILENAME_MAX];
	if (!getCurrentWorkingDir(currentPath, sizeof(currentPath))) {
		throw invalid_argument("Failed to find current working directory.");
	}
	currentPath[sizeof(currentPath) - 1] = '\0';
	return currentPath;
}

bool loadHouseList(const string& housesPath, list<House>& houseList, vector<string>& errors, string& usage) {
	
	fs::path dir(housesPath);
	if (!fs::exists(dir) || !fs::is_directory(dir)) {
		cout << usage << endl;
		return false;
	}

	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter) {
		if (fs::is_regular_file(dir_iter->status())) {
			if (dir_iter->path().has_extension() && dir_iter->path().extension() == ".house") {
				try {
					House house = House::deseriallize(dir_iter->path());
					logger.info("Validating house");
					if (logger.debugEnabled()) logger.debug("Validating house walls");
					house.validateWalls();
					if (logger.debugEnabled()) logger.debug("Validating the existence of exactly one docking station");
					house.validateDocking();
					logger.info("House is valid");
					houseList.push_back(move(house));
				}
				catch (exception& e) {
					errors.push_back(e.what());
				}
			}
		}
	}

	if (houseList.empty()) {
		if (errors.empty()) {
			cout << usage << endl;
		}
		else {
			cout << "All house files in target folder '" << dir.string() << "' cannot be opened or are invalid:" << endl;
			for (string& err : errors) {
				cout << err << endl;
			}
		}
		return false;
	}

	houseList.sort([](const House& a, const House& b) {return a.getName() < b.getName();});
	return true;
}

// create a map of key-value pairs from config file (expected format of each line: key=value)
bool loadConfiguration(const string& configFileDir, map<string, int>& configMap, string& usage) {
	
	fs::path path = fs::path(configFileDir) / "config.ini";
	if (!fs::exists(path)) {
		cout << usage << endl;
		return false;
	}

	ifstream configFileStream(path.string());
	string currLine;
	if (configFileStream) {
		populateConfigMap(configFileStream, configMap);
		try {
			configFileStream.close();
		}
		catch (exception& e) {
			logger.error(e.what());
		}
		if (!isConfigMapValid(configMap)) {
			return false;
		}
	}
	else {
		cout << "config.ini exists in '" << path.string() << "' but cannot be opened" << endl;
		return false;
	}
	return true;
}

void populateConfigMap(ifstream& configFileStream, map<string, int>& configMap) {
	string currLine;
	while (getline(configFileStream, currLine)) {
		if (logger.debugEnabled()) logger.debug("Read line from config file: " + currLine);
		try {
			size_t positionOfEquals = currLine.find("=");
			string key = currLine.substr(0, (int)positionOfEquals);
			trimString(key);
			if (positionOfEquals != string::npos) {
				int value = stoi(currLine.substr((int)positionOfEquals + 1)); // possibly: invalid_argument or out_of_range
				configMap.insert(pair<string, int>(key, max(0, value)));
			}
		}
		catch (exception& e) {
			// if we get all params at the end of the process these exceptions should not stop us
			logger.error(e.what());
		}
	}
}

bool isConfigMapValid(map<string, int>& configMap) {

	vector<string> params = {"MaxStepsAfterWinner", "BatteryCapacity", 
		"BatteryConsumptionRate", "BatteryRechargeRate"};
	vector<string> missing;

	for (string& param : params) {
		if (configMap.find(param) == configMap.end()) {
			missing.push_back(param);
		}
	}
	if (!missing.empty()) {
		cout << "config.ini missing " << missing.size() << " parameter(s): ";
		for (size_t i = 0; i < missing.size(); ++i) {
			cout << missing[i];
			if (i < missing.size() - 1) {
				cout << ", ";
			}
		}
		cout << endl;
		return false;
	}
	return true;	
}

void trimString(string& str) {
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	str = str.substr(first, (last - first + 1));
}

void printErrors(vector<string>& houseErrors, vector<string>& algorithmErrors) {
	if (houseErrors.empty() && algorithmErrors.empty()) {
		return;
	}
	cout << endl;
	cout << "Errors:" << endl;
	for (string& err : houseErrors) {
		cout << err << endl;
	}
	for (string& err : algorithmErrors) {
		cout << err << endl;
	}
}