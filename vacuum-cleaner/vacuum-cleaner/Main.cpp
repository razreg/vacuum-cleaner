#include "Main.h"

const string MainHelper::usage =
	"Usage: simulator [-config <config path>] [-house_path <house path>] "
	"[-algorithm_path <algorithm path>] [-score_formula <score .so path>] "
	"[-threads <num threads>] [-video]";

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
	bool video = false;

	// parse command line arguments
	logger.debug("Parsing command line arguments");
	bool isValid = parseArgs(argc, argv, configPath, housesPath, algorithmsPath, 
		scoreFormulaPath, threads, video);
	if (!isValid) {
		logger.fatal("Invalid arguments");
		cout << MainHelper::getUsage() << endl;
		return INVALID_ARGUMENTS;
	}
	if (video && threads > 1) {
		cout << "Cannot capture video concurrently. Please rerun with either -video or -threads" << endl;
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
		(returnCode = mainHelper.setHousePaths(housesPath)) == SUCCESS &&
		(returnCode = mainHelper.runSimulator(video)) == SUCCESS;
	
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
		cout << usage << endl;
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
			cout << usage << endl;
			isValid = false;
		}
		if (!isValid) {
			return INVALID_SCORE_FORMULA;
		}
	}
	else {
		logger.info("Using default score formula");
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
		cout << usage << endl;
		return INVALID_ALGORITHMS;
	}
	return SUCCESS;
}

int MainHelper::setHousePaths(string& housesPath) {
	bool isValid;
	try {
		logger.debug("Loading houses from directory");
		isValid = getHousePaths(housesPath);
		if (!isValid) {
			return INVALID_HOUSES;
		}
	}
	catch (exception& e) {
		logger.fatal(e.what());
		cout << usage << endl;
		return INVALID_HOUSES;
	}
	return SUCCESS;
}

int MainHelper::runSimulator(bool video) {
	Simulator simulator(configMap, scoreFormula, housePathVector, registrar, algorithmErrors, video);
	try {
		simulator.execute(threads);
	}
	catch (exception& e) {
		logger.fatal(e.what());
		return INTERNAL_FAILURE;
	}
	return SUCCESS;
}

bool MainHelper::getHousePaths(const string& housesPath) {

	fs::path dir(housesPath);
	if (fs::exists(dir)) {
		dir = fs::canonical(dir);
	}
	string errNoFiles = "cannot find house files in '" + dir.string() + "'";
	try {
		if (!isDirectory(dir)) {
			cout << usage << endl;
			cout << errNoFiles << endl;
			return false;
		}

		fs::directory_iterator end_iter;
		for (fs::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter) {
			if (fs::is_regular_file(dir_iter->status()) &&
				dir_iter->path().has_extension() &&
				dir_iter->path().extension() == ".house") {
				housePathVector.push_back(dir_iter->path());
			}
		}
		if (housePathVector.empty()) {
			cout << usage << endl;
			cout << errNoFiles << endl;
			return false;
		}
		return true;
	}
	// permission denied
	catch (const fs::filesystem_error& e) {
		if (e.code() == boost::system::errc::permission_denied) {
			cout << usage << endl;
			cout << errNoFiles << endl;
		}
		return false;
	}
}

bool MainHelper::loadConfiguration(const string& configFileDir) {

	fs::path base = fs::path(configFileDir);
	if (fs::exists(base)) {
		base = fs::canonical(base);
	}
	string errNoFiles = "cannot find config.ini file in '" + base.string() + "'";
	try {
		fs::path path = base / "config.ini";
		if (!fs::exists(path)) {
			cout << usage << endl;
			cout << errNoFiles << endl;
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
			cout << "config.ini exists in '" << base.string() << "' but cannot be opened" << endl;
			return false;
		}
		return true;
	}
	// permission denied
	catch (const fs::filesystem_error& e) {
		if (e.code() == boost::system::errc::permission_denied) {
			cout << usage << endl;
			cout << errNoFiles << endl;
		}
		return false;
	}
}

void MainHelper::populateConfigMap(ifstream& configFileStream) {
	string currLine;
	while (getline(configFileStream, currLine)) {
		if (logger.debugEnabled()) {
			trimString(currLine);
			logger.debug("Read line from config file: " + currLine);
		}
		try {
			size_t positionOfEquals = currLine.find("=");
			string key = currLine.substr(0, (int)positionOfEquals);
			trimString(key);
			if (positionOfEquals != string::npos) {
				string valueStr = currLine.substr((int)positionOfEquals + 1);
				trimString(valueStr);
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

bool MainHelper::loadAlgorithms(const string& algorithmsPath) {

	fs::path dir(algorithmsPath);
	if (fs::exists(dir)) {
		dir = fs::canonical(dir);
	}
	string errNoFiles = "cannot find algorithm files in '" + dir.string() + "'";
	try {
		if (!isDirectory(dir)) {
			cout << usage << endl;
			cout << errNoFiles << endl;
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

		if (registrar.size() == 0) {
			if (algorithmErrors.empty()) {
				cout << usage << endl;
				cout << errNoFiles << endl;
			}
			else {
				cout << "All algorithm files in target folder '"
					<< dir.string() << "' cannot be opened or are invalid:" << endl;
				for (string& err : algorithmErrors) cout << err << endl;
			}
			return false;
		}
		return true;
	}
	// permission denied
	catch (const fs::filesystem_error& e) {
		if (e.code() == boost::system::errc::permission_denied) {
			cout << usage << endl;
			cout << errNoFiles << endl;
		}
		return false;
	}
}

bool MainHelper::loadScoreFormula(const string& scoreFormulaPath) {

	fs::path base = fs::path(scoreFormulaPath);
	if (fs::exists(base)) {
		base = fs::canonical(base);
	}
	string errNoFiles = "cannot find score_formula.so file in '" + base.string() + "'";
	try {
		fs::path path = base / "score_formula.so";
		if (!fs::exists(path)) {
			cout << usage << endl;
			cout << errNoFiles << endl;
			return false;
		}

		libHandle = dlopen(path.c_str(), RTLD_NOW);
		if (libHandle == NULL) {
			if (logger.debugEnabled()) {
				logger.debug("Failed to load score_formula.so. Details: " + string(dlerror()));
			}
			cout << "score_formula.so exists in '" << base.string()
				<< "' but cannot be opened or is not a valid .so" << endl;
			return false;
		}
		dlerror(); // clear dlerror
		void* func = dlsym(libHandle, SCORE_FORMULA_METHOD_NAME);
		scoreFormula = reinterpret_cast<ScoreFormula>(reinterpret_cast<long>(func));
		char* err;
		if ((err = dlerror()) != NULL) {
			logger.debug("Failed to load score_formula.so. Details: " + string(err));
			cout << "score_formula.so is a valid .so but it does not have a valid score formula" << endl;
			return false;
		}
		return true;
	}
	// permission denied
	catch (const fs::filesystem_error& e) {
		if (e.code() == boost::system::errc::permission_denied) {
			cout << usage << endl;
			cout << errNoFiles << endl;
		}
		return false;
	}
}

bool parseArgs(int argc, char** argv, string& configPath, string& housesPath, string& algorithmsPath,
	string &scoreFormula, size_t& threads, bool& video) {

	bool valid = true;
	for (int i = 1; valid && i < argc; ++i) {
		if (string(argv[i]) == "-config") {
			if ((valid = argc > i + 1)) configPath = argv[++i];
		}
		else if (string(argv[i]) == "-house_path") {
			if ((valid = argc > i + 1)) housesPath = argv[++i];
		}
		else if (string(argv[i]) == "-algorithm_path") {
			if ((valid = argc > i + 1)) algorithmsPath = argv[++i];
		}
		else if (string(argv[i]) == "-score_formula") {
			if ((valid = argc > i + 1)) scoreFormula = argv[++i];
		}
		else if (string(argv[i]) == "-threads") {
			if ((valid = argc > i + 1)) {
				string threadsStr = argv[++i];
				try {
					threads = max(1, stoi(threadsStr));
				}
				catch (exception& e) {
					threads = 1;
				}
			}
		}
		else if (string(argv[i]) == "-video") {
			video = true;
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