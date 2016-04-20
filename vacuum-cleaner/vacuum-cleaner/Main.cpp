#include "Main.h"

using namespace std;
namespace fs = boost::filesystem;

Logger logger = Logger("Main");
AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();

int main(int argc, char** argv) {

	string usage =
		"Usage: simulator [-config <config path>] [-house_path <house path>] [-algorithm_path <algorithm path>]";

	list<House> houseList;
	map<string, int> configMap;
	list<unique_ptr<AbstractAlgorithm>> algorithms;
	list<string> algorithmNames;

	vector<string> algorithmErrors;
	vector<string> houseErrors;
	vector<string> simulationErrors;

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
	logger.debug("Parsing command line arguments");
	bool isValid = parseArgs(argc, argv, configPath, housesPath, algorithmsPath);
	if (!isValid) {
		logger.fatal("Invalid arguments");
		cout << usage << endl;
		return INVALID_ARGUMENTS;
	}
	logger.debug("Using config file directory path as [" + configPath + "]");
	logger.debug("Using house files directory path as [" + housesPath + "]");

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

	// Algorithms
	logger.debug("Loading algorithms from directory");
	try {
		isValid = loadAlgorithms(algorithmsPath, algorithms, algorithmNames, algorithmErrors, usage);
		if (!isValid) {
			return INVALID_ALGORITHMS;
		}
	}
	catch (exception& e) {
		logger.fatal(e.what()); // see AlgorithmRegistrar::setNameForLastAlgorithm
		return INVALID_ALGORITHMS;
	}

	// Houses
	try {
		logger.debug("Loading houses from directory");
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
	Simulator simulator(configMap, houseList, algorithms, move(algorithmNames));
	try {
		simulationErrors = simulator.execute();
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INTERNAL_FAILURE;
	}

	printErrors(houseErrors, algorithmErrors, simulationErrors);
	return SUCCESS;
}

bool parseArgs(int argc, char** argv, string& configPath, string& housesPath, string& algorithmsPath) {
	bool valid = true;
	for (int i = 1; valid && i < argc; ++i) {
		if ((string(argv[i])).compare("-config") == 0) {
			if ((valid = argc > i + 1)) {
				configPath = argv[i + 1];
			}
		}
		else if ((string(argv[i])).compare("-house_path") == 0) {
			if ((valid = argc > i + 1)) {
				housesPath = argv[i + 1];
			}
		}
		else if ((string(argv[i])).compare("-algorithm_path") == 0) {
			if ((valid = argc > i + 1)) {
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
	if (!getcwd(currentPath, sizeof(currentPath))) {
		throw invalid_argument("Failed to find current working directory.");
	}
	currentPath[sizeof(currentPath) - 1] = '\0';
	return currentPath;
}

bool loadHouseList(const string& housesPath, list<House>& houseList, vector<string>& errors, string& usage) {

	fs::path dir(housesPath);
	if (!isDirectory(dir, usage)) {
		return false;
	}

	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter) {
		if (fs::is_regular_file(dir_iter->status())) {
			if (dir_iter->path().has_extension() && dir_iter->path().extension() == ".house") {
				try {
					House house = House::deseriallize(dir_iter->path());
					logger.debug("Validating house");
					if (logger.debugEnabled()) logger.debug("Validating house walls");
					house.validateWalls();
					if (logger.debugEnabled()) logger.debug("Validating the existence of exactly one docking station");
					house.validateDocking();
					logger.debug("House is valid");
					houseList.push_back(move(house));
				}
				catch (exception& e) {
					errors.push_back(e.what());
				}
			}
		}
	}

	houseList.sort([](const House& a, const House& b) {return a.getName() < b.getName(); });
	return !allLoadingFailed(houseList, errors, usage, "house", dir);
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
				configMap[key] = max(0, value);
			}
		}
		catch (exception& e) {
			// if we get all params at the end of the process these exceptions should not stop us
			logger.error(e.what());
		}
	}
}

bool isConfigMapValid(map<string, int>& configMap) {

	vector<string> params = { "MaxStepsAfterWinner", "BatteryCapacity",
		"BatteryConsumptionRate", "BatteryRechargeRate" };
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

void printErrors(vector<string>& houseErrors, vector<string>& algorithmErrors, vector<string>& simulationErrors) {
	if (!houseErrors.empty() || !algorithmErrors.empty() || !simulationErrors.empty()) {
		cout << endl;
		cout << "Errors:" << endl;
	}
	for (string& err : houseErrors) {
		cout << err << endl;
	}
	for (string& err : algorithmErrors) {
		cout << err << endl;
	}
	for (string& err : simulationErrors) {
		cout << err << endl;
	}
}

bool loadAlgorithms(const string& algorithmsPath, list<unique_ptr<AbstractAlgorithm>>& algorithms,
	list<string>& algorithmNames, vector<string>& errors, string& usage) {

	fs::path dir(algorithmsPath);
	if (!isDirectory(dir, usage)) {
		return false;
	}

	// load libraries into registrar
	fs::directory_iterator end_iter;
	for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter) {
		if (fs::is_regular_file(dir_iter->status())) {
			if (dir_iter->path().has_extension() && dir_iter->path().extension() == ".so") {
				if (logger.debugEnabled()) {
					logger.debug("Loading algorithm from file [" + dir_iter->path().filename().string() + "]");
				}
				int result = 
					registrar.loadAlgorithm(dir_iter->path().string(), dir_iter->path().stem().string());
				if (result == AlgorithmRegistrar::FILE_CANNOT_BE_LOADED) {
					errors.push_back(dir_iter->path().filename().string() +
						": file cannot be loaded or is not a valid.so");
				}
				else if (result == AlgorithmRegistrar::NO_ALGORITHM_REGISTERED) {
					errors.push_back(dir_iter->path().filename().string() +
						": valid .so but no algorithm was registered after loading it");
				}
			}
		}
	}

	if (registrar.size() != 0) {
		algorithms = registrar.getAlgorithms();
		algorithmNames = registrar.getAlgorithmNames();
	}

	return !allLoadingFailed(algorithms, errors, usage, "algorithm", dir);
}

bool isDirectory(fs::path& dir, string& usage) {
	if (!fs::exists(dir) || !fs::is_directory(dir)) {
		cout << usage << endl;
		return false;
	}
	return true;
}

template<typename T>
bool allLoadingFailed(list<T>& loadedObjectsList, vector<string>& errors, string& usage, 
	string typeName, fs::path& dir) {

	if (loadedObjectsList.empty()) {
		if (errors.empty()) {
			cout << usage << endl;
		}
		else {
			cout << "All " << typeName << " files in target folder '" 
				<< dir.string() << "' cannot be opened or are invalid:" << endl;
			for (string& err : errors) {
				cout << err << endl;
			}
		}
		return true;
	}
	return false;
}
