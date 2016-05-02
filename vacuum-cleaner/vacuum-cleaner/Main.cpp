#include "Main.h"

using namespace std;
namespace fs = boost::filesystem;

Logger logger = Logger("Main");
AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();

// TODO break to methods and consider making main an object
int main(int argc, char** argv) {

	string usage =
		"Usage: simulator [-config <config path>] [-house_path <house path>] "
		"[-algorithm_path <algorithm path>] [-score_formula <score .so path>] " 
		"[-threads <num threads>]";

	list<House> houseList;
	map<string, int> configMap;
	ScoreFormula scoreFormula = NULL;
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
	string scoreFormulaPath = SCORE_FORMULA_PLACEHOLDER;
	size_t threads = 1;

	logger.debug("Parsing command line arguments");
	bool isValid = parseArgs(argc, argv, configPath, housesPath, algorithmsPath, scoreFormulaPath, threads);
	if (!isValid) {
		logger.fatal("Invalid arguments");
		cout << usage << endl;
		return INVALID_ARGUMENTS;
	}
	if (logger.debugEnabled()) {
		logger.debug("Using config file directory path as [" + configPath + "]");
		logger.debug("Using house files directory path as [" + housesPath + "]");
	}

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

	// Score formula
	void* libHandle = nullptr;
	if (SCORE_FORMULA_PLACEHOLDER != scoreFormulaPath) {
		try {
			logger.info("Loading score_formula.so from directory");
			isValid = loadScoreFormula(scoreFormulaPath, scoreFormula, libHandle, usage);
		}
		catch (exception& e) {
			logger.fatal(e.what());
			isValid = false;
		}
		if (!isValid) {
			return INVALID_SCORE_FORMULA;
		}
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
	Simulator simulator(configMap, scoreFormula, houseList, algorithms, move(algorithmNames));
	try {
		simulationErrors = simulator.execute();
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INTERNAL_FAILURE;
	}

	printErrors(houseErrors, algorithmErrors, simulationErrors);
	if (libHandle != nullptr) {
		dlclose(libHandle);
	}
	return SUCCESS;
}

bool parseArgs(int argc, char** argv, string& configPath, string& housesPath, string& algorithmsPath, 
	string &scoreFormula, size_t& threads) {

	bool valid = true;
	for (int i = 1; valid && i < argc; ++i) {
		if (string(argv[i]) == "-config") {
			if ((valid = argc > i + 1)) configPath = argv[i + 1];
		}
		else if (string(argv[i]) == "-house_path") {
			if ((valid = argc > i + 1)) housesPath = argv[i + 1];
		}
		else if (string(argv[i]) == "-algorithm_path") {
			if ((valid = argc > i + 1)) algorithmsPath = argv[i + 1];
		}
		else if (string(argv[i]) == "-score_formula") {
			if ((valid = argc > i + 1)) scoreFormula = argv[i + 1];
		}
		else if (string(argv[i]) == "-threads") {
			if ((valid = argc > i + 1)) {
				string threadsStr = argv[i + 1];
				try {
					threads = max(1, stoi(threadsStr));
				}
				catch (exception& e) {
					threads = 1;
				}
			}
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
	if (!isDirectory(dir)) {
		cout << usage << endl;
		cout << "cannot find house files in '" << dir.string() << "'" << endl;
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
		cout << "cannot find config.ini file in '" << path.string() << "'" << endl;
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
				string valueStr = currLine.substr((int)positionOfEquals + 1);
				int value;
				if (valueStr.empty() || find_if(valueStr.begin(),
					valueStr.end(), [](char c) { return !isdigit(c); }) != valueStr.end()) {
					value = -1;
				}
				else {
					value = stoi(valueStr); // possibly: invalid_argument or out_of_range
				}
				configMap[key] = value;
			}
		}
		catch (exception& e) {
			// if we get all params at the end of the process these exceptions should not stop us
			logger.error(e.what());
		}
	}
}

bool isConfigMapValid(map<string, int>& configMap) {
	bool ret = true;
	vector<string> params = { "MaxStepsAfterWinner", "BatteryCapacity",
		"BatteryConsumptionRate", "BatteryRechargeRate" };
	vector<string> missing;
	vector<string> bad;

	for (string& param : params) {
		if (configMap.find(param) == configMap.end()) {
			missing.push_back(param);
		}
		else if (configMap[param] < 0) {
			bad.push_back(param);
		}
	}
	if (!missing.empty()) {
		cout << "config.ini missing " << missing.size() << " parameter(s): ";
		printStringVector(missing);
		ret = false;
	}
	if (!bad.empty()) {
		cout << "config.ini having bad values for " << bad.size() << " parameter(s): ";
		printStringVector(bad);
		ret = false;
	}
	return ret;
}

void printStringVector(vector<string>& vec) {
	for (size_t i = 0; i < vec.size(); ++i) {
		cout << vec[i];
		if (i < vec.size() - 1) {
			cout << ", ";
		}
	}
	cout << endl;
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
	if (!isDirectory(dir)) {
		cout << usage << endl;
		cout << "cannot find algorithm files in '" << dir.string() << "'" << endl;
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
						": file cannot be loaded or is not a valid .so");
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

bool loadScoreFormula(const string& scoreFormulaPath, ScoreFormula& scoreFormula, void* libHandle, string& usage) {

	fs::path base = fs::path(scoreFormulaPath);
	fs::path path = base / "score_formula.so";
	if (!fs::exists(path)) {
		cout << usage << endl;
		cout << "cannot find score_formula.so file in '" << base.string() << "'" << endl;
		return false;
	}

	libHandle = dlopen(path.c_str(), RTLD_NOW);
	if (libHandle == NULL) {
		if (logger.debugEnabled()) {
			logger.debug("Failed to load score_formula.so. Details: " + string(dlerror()));
		}
		cout << "score_formula.so exists in '" << path.string() 
			<< "' but cannot be opened or is not a valid .so" << endl;
		return false;
	}
	void* func = dlsym(libHandle, SCORE_FORMULA_METHOD_NAME); // TODO cast exception?
	scoreFormula = reinterpret_cast<ScoreFormula>(reinterpret_cast<long>(func));
	char* err;
	if ((err = dlerror()) != NULL) {
		logger.debug("Failed to load score_formula.so. Details: " + string(err));
		cout << "score_formula.so is a valid.so but it does not have a valid score formula" << endl;
		dlclose(libHandle);
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
			cout << "cannot find " << typeName << " files in '" << dir.string() << "'" << endl;
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
