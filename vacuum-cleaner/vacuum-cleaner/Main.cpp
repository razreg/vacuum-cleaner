#include "Main.h"

const string MainHelper::usage =
	"Usage: simulator [-config <config path>] [-house_path <house path>] "
	"[-algorithm_path <algorithm path>] [-score_formula <score .so path>] "
	"[-threads <num threads>]";

AlgorithmRegistrar& MainHelper::registrar = AlgorithmRegistrar::getInstance();

int main(int argc, char** argv) {

	MainHelper mainHelper;
	int returnCode = SUCCESS;

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

	// parse command line arguments
	logger.debug("Parsing command line arguments");
	bool isValid = parseArgs(argc, argv, configPath, housesPath, algorithmsPath, scoreFormulaPath, threads);
	if (!isValid) {
		logger.fatal("Invalid arguments");
		cout << MainHelper::getUsage() << endl;
		return INVALID_ARGUMENTS;
	}
	if (logger.debugEnabled()) {
		logger.debug("Using config file directory path as [" + configPath + "]");
		logger.debug("Using house files directory path as [" + housesPath + "]");
	}
	mainHelper.setThreads(threads);

	// load everything and run simulation
	isValid =
		(returnCode = mainHelper.setConfiguration(configPath)) == SUCCESS && 
		(returnCode = mainHelper.setScoreFormula(scoreFormulaPath)) == SUCCESS &&
		(returnCode = mainHelper.setAlgorithms(algorithmsPath)) == SUCCESS &&
		(returnCode = mainHelper.setHouses(housesPath)) == SUCCESS &&
		(returnCode = mainHelper.runSimulator()) == SUCCESS;
	
	if (isValid) mainHelper.printErrors();
	return returnCode;
}

int MainHelper::setConfiguration(string& configPath) {
	bool isValid;
	try {
		logger.info("Loading configuration from directory");
		isValid = loadConfiguration(configPath);
		if (!isValid) {
			return INVALID_CONFIGURATION;
		}
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INVALID_CONFIGURATION;
	}
	return SUCCESS;
}

int MainHelper::setScoreFormula(string& scoreFormulaPath) {
	bool isValid;
	if (SCORE_FORMULA_PLACEHOLDER != scoreFormulaPath) {
		try {
			logger.info("Loading score_formula.so from directory");
			isValid = loadScoreFormula(scoreFormulaPath);
		}
		catch (exception& e) {
			logger.fatal(e.what());
			isValid = false;
		}
		if (!isValid) {
			return INVALID_SCORE_FORMULA;
		}
	}
	return SUCCESS;
}

int MainHelper::setAlgorithms(string& algorithmsPath) {
	bool isValid;
	logger.debug("Loading algorithms from directory");
	try {
		isValid = loadAlgorithms(algorithmsPath);
		if (!isValid) {
			return INVALID_ALGORITHMS;
		}
	}
	catch (exception& e) {
		logger.fatal(e.what()); // see AlgorithmRegistrar::setNameForLastAlgorithm
		return INVALID_ALGORITHMS;
	}
	return SUCCESS;
}

int MainHelper::setHouses(string& housesPath) {
	bool isValid;
	try {
		logger.debug("Loading houses from directory");
		isValid = loadHouseList(housesPath);
		if (!isValid) {
			return INVALID_HOUSES;
		}
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INVALID_HOUSES;
	}
	return SUCCESS;
}

int MainHelper::runSimulator() {
	Simulator simulator(configMap, scoreFormula, houseList, algorithms, algorithmNames);
	try {
		simulationErrors = simulator.execute();
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INTERNAL_FAILURE;
	}
	return SUCCESS;
}

bool MainHelper::loadHouseList(const string& housesPath) {

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
					houseErrors.push_back(e.what());
				}
			}
		}
	}

	houseList.sort([](const House& a, const House& b) {return a.getName() < b.getName(); });
	return !allLoadingFailed(houseList, houseErrors, "house", dir);
}

bool MainHelper::loadConfiguration(const string& configFileDir) {

	fs::path path = fs::path(configFileDir) / "config.ini";
	if (!fs::exists(path)) {
		cout << usage << endl;
		cout << "cannot find config.ini file in '" << path.string() << "'" << endl;
		return false;
	}

	ifstream configFileStream(path.string());
	string currLine;
	if (configFileStream) {
		populateConfigMap(configFileStream);
		try {
			configFileStream.close();
		}
		catch (exception& e) {
			logger.error(e.what());
		}
		if (!isConfigMapValid()) {
			return false;
		}
	}
	else {
		cout << "config.ini exists in '" << path.string() << "' but cannot be opened" << endl;
		return false;
	}
	return true;
}

void MainHelper::populateConfigMap(ifstream& configFileStream) {
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

bool MainHelper::isConfigMapValid() {
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

void MainHelper::printErrors() {
	if (!houseErrors.empty() || !algorithmErrors.empty() || !simulationErrors.empty()) {
		cout << endl;
		cout << "Errors:" << endl;
	}
	for (string& err : houseErrors) cout << err << endl;
	for (string& err : algorithmErrors) cout << err << endl;
	for (string& err : simulationErrors) cout << err << endl;
}

bool MainHelper::loadAlgorithms(const string& algorithmsPath) {

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
					algorithmErrors.push_back(dir_iter->path().filename().string() +
						": file cannot be loaded or is not a valid .so");
				}
				else if (result == AlgorithmRegistrar::NO_ALGORITHM_REGISTERED) {
					algorithmErrors.push_back(dir_iter->path().filename().string() +
						": valid .so but no algorithm was registered after loading it");
				}
			}
		}
	}

	if (registrar.size() != 0) {
		algorithms = registrar.getAlgorithms();
		algorithmNames = registrar.getAlgorithmNames();
	}

	return !allLoadingFailed(algorithms, algorithmErrors, "algorithm", dir);
}

bool MainHelper::loadScoreFormula(const string& scoreFormulaPath) {

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
		return false;
	}

	return true;
}

template<typename T>
bool MainHelper::allLoadingFailed(list<T>& loadedObjectsList, vector<string>& errors, 
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

void printStringVector(vector<string>& vec) {
	for (size_t i = 0; i < vec.size(); ++i) {
		cout << vec[i];
		if (i < vec.size() - 1) {
			cout << ", ";
		}
	}
	cout << endl;
}