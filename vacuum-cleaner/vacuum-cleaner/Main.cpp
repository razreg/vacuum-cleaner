#include "Main.h"

using namespace std;
namespace fs = boost::filesystem;

Logger logger = Logger("Main");

int main(int argc, char** argv) {

	string usage = 
		"Usage: simulator [­config <config path>] [­house_path <house path>] [­algorithm_path <algorithm path>]";

	list<House> houseList;
	map<string, int> configMap;
	list<AbstractAlgorithm*> algorithms;

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
	bool invalid = parseArgs(argc, argv, configPath, housesPath, algorithmsPath);
	if (invalid) {
		logger.fatal("Invalid arguments");
		cout << usage << endl;
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

	// TODO load algorithms
	Algorithm1 algo1; //**IDO**
	//Algorithm2 algo2; //**IDO**
	algorithms.push_back(&algo1);//**IDO**
	//algorithms.push_back(&algo2);//**IDO**

	//NaiveAlgorithm naiveAlgorithm; 
	//algorithms.push_back(&naiveAlgorithm);

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

bool parseArgs(int argc, char** argv, string& configPath, string& housesPath, string& algorithmsPath) {
	bool invalid = false;
	for (int i = 1; !invalid && i < argc; ++i) {
		if ((string(argv[i])).compare("-config") == 0) {
			invalid = argc <= i + 1;
			if (!invalid) {
				configPath = argv[i + 1];
			}
		}
		else if ((string(argv[i])).compare("-house_path") == 0) {
			invalid = argc <= i + 1;
			if (!invalid) {
				housesPath = argv[i + 1];
			}
		}
		else if ((string(argv[i])).compare("-algorithm_path") == 0) {
			invalid = argc <= i + 1;
			if (!invalid) {
				algorithmsPath = argv[i + 1];
			}
		}
		else if (i % 2 == 1) {
			invalid = true;
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

bool loadHouseList(const string& housesPath, list<House>& houseList) {
	
	fs::path dir(housesPath);
	bool valid = fs::exists(dir) && fs::is_directory(dir);
	if (valid) {
		fs::directory_iterator end_iter;
		for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter) {
			if (fs::is_regular_file(dir_iter->status())) {
				if (dir_iter->path().has_extension() && dir_iter->path().extension() == ".house") {
					House house = House::deseriallize(dir_iter->path().string());
					logger.info("Validating house");
					if (logger.debugEnabled()) logger.debug("Validating house walls");
					house.validateWalls();
					if (logger.debugEnabled()) logger.debug("Validating the existence of exactly one docking station");
					house.validateDocking();
					logger.info("House is valid");
					houseList.push_back(move(house));
				}
			}
		}
		houseList.sort([](const House& a, const House& b) {
			return a.getPath() < b.getPath();
		});
	}

	return valid;
}

// create a map of key-value pairs from config file (expected format of each line: key=value)
void loadConfiguration(const string& configFileDir, map<string, int>& configMap) {
	
	fs::path path = fs::path(configFileDir) / "config.ini";
	ifstream configFileStream(path.string());
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
}

void populateConfigMap(ifstream& configFileStream, map<string, int>& configMap) {
	string currLine;
	while (getline(configFileStream, currLine)) {
		if (logger.debugEnabled()) logger.debug("Read line from config file: " + currLine);
		size_t positionOfEquals = currLine.find("=");
		string key = currLine.substr(0, (int)positionOfEquals);
		trimString(key);
		if (positionOfEquals != string::npos) {
			int value = stoi(currLine.substr((int)positionOfEquals + 1)); // possibly: invalid_argument or out_of_range
			configMap.insert(pair<string, int>(key, max(0, value)));
		}
	}
}

void trimString(string& str) {
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	str = str.substr(first, (last - first + 1));
}
