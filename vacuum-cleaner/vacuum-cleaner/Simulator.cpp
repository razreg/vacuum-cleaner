#include "Simulator.h"

using namespace std;
namespace fs = boost::filesystem;

// Usage: simulator -config <config_file_location> -house_path <houses_path_location>
//    or: simulator -house_path <houses_path_location> -config <config_file_location>
//    if either one of them is missing- look for it in the working directory
int main(int argc, char** argv) {
	
	string usageError = "Error: Invalid arguments. "
		"Usage: simulator -config <config_file_location> -house_path <houses_path_location>, or: simulator -house_path <houses_path_location> -config <config_file_location> ";
	string houses_path;
	string config_path;
	list<string> housePathList;
	map<string, int> configMap;

	if (argc != 1 && argc != 3 && argc != 5){
		cout << usageError << endl;
		return INVALID_ARGUMENTS;
	}
	
	//getting full path to working directoy
	fs::path full_path_working_directory(fs::current_path());

	// both argument are missing
	if (argc == 1){ 
		config_path = full_path_working_directory.string();
		houses_path = full_path_working_directory.string();		
	}
	else if (argc == 3){
		if (argv[1] == "-config"){
			config_path = argv[2];
			configMap = getConfiguration(config_path); // TODO catch exception and exit gracefully?
			houses_path = full_path_working_directory.string();
		}
		else if (argv[1] == "-house_path"){
			houses_path = argv[2];
			housePathList = getHouseList(houses_path);
			config_path = full_path_working_directory.string();
		}
		else{
			cout << usageError << endl;
			return INVALID_ARGUMENTS;
		}
	}
	else{
		if (argv[1] == "-config" && argv[3] == "-house_path"){
			config_path = argv[2];
			houses_path = argv[4];
			configMap = getConfiguration(config_path); // TODO catch exception and exit gracefully?
			housePathList = getHouseList(houses_path);
		}
		else if (argv[1] == "-house_path" && argv[3] == "-config")
		{
			config_path = argv[4];
			houses_path = argv[2];
			configMap = getConfiguration(config_path); // TODO catch exception and exit gracefully?
			housePathList = getHouseList(houses_path);
		}
		else{
			cout << usageError << endl;
			return INVALID_ARGUMENTS;
		}
	}

	// TODO -make sure that if we get here than the paths are valid
	// TODO load houses into an array of House objects
	House* houses = new House[housePathList.size()];
	int i = 0;
	string slash("/");
	for (list<string>::const_iterator item = housePathList.begin(); item != housePathList.end(); ++item){
		if (EndsWith(houses_path, "/")){
			houses[i] = House::deseriallize(houses_path + *item);
		}
		else{
			houses[i] = House::deseriallize(houses_path + slash + *item);
		}
		check_house_surroundings(houses[i]); //for each house- make sure boundaries are walls
		i++;
	}

	// TODO init NaiveAlgorithm (including setSensor ***and setConfiguration***)
	// TODO loop until finish (maxSteps, house clean, etc.) and execute the algorithm steps on the house - 
	//      each algorithm must have a house of its own so we need to clone the house before passing it 
	//      to the algorithm!
	//      in the loop - when we init a house, init a Score as well
	
	delete [] houses;
	return SUCCESS;
}

map<string, int> getConfiguration(string configFilePath) {
	
	string configError = "Error: configuration file [" + configFilePath + "] is invalid";
	if (EndsWith(configFilePath, "/")){
		configFilePath = configFilePath + "config.ini";
	}
	else{
		configFilePath = configFilePath + "/config.ini";
	}

	// create a map of key-value pairs from config file (expected format of each line: key=value)
	map<string, int> configMap;
	ifstream configFileStream(configFilePath);
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
					configMap.insert(key, value);
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

	return configMap;

	// raz- I just copied what you did here to NaiveAlgorithm.cpp.. hope I am not wrong!
	/*
	// build configuration from map
	Configuration config;
	map<string, int>::iterator mapIterator;
	mapIterator = configMap.find("MaxSteps");
	config.maxSteps = 
		(mapIterator != configMap.end()) ? mapIterator->second : DEFAULT_MAX_STEPS;
	mapIterator = configMap.find("MaxStepsAfterWinner");
	config.maxStepsAfterWinner = 
		(mapIterator != configMap.end()) ? mapIterator->second : DEFAULT_MAX_STEPS_AFTER_WINNER;
	mapIterator = configMap.find("BatteryCapacity");
	config.batteryCapacity = 
		(mapIterator != configMap.end()) ? mapIterator->second : DEFAULT_BATTERY_CAPACITY;
	mapIterator = configMap.find("BatteryConsumptionRate");
	config.batteryConsumptionRate =
		(mapIterator != configMap.end()) ? mapIterator->second : DEFAULT_BATTERY_CONSUMPTION_RATE;
	mapIterator = configMap.find("BatteryRachargeRate");
	config.batteryRachargeRate =
		(mapIterator != configMap.end()) ? mapIterator->second : DEFAULT_BATTERY_RECHARGE_RATE;
	return config;

	*/
}

//the method returns a list of houses files within the given path
list<string> getHouseList(string housePath){
	DIR *dir;
	struct dirent *fileInPath;
	list<string> houses;

	string houseError = "Error: house file's path [" + housePath + "] is invalid";
	
	const char *path = housePath.c_str(); // converts string to char*
	if ((dir = opendir(path)) != NULL){
		while ((fileInPath = readdir(dir)) != NULL){
			if (EndsWith(fileInPath->d_name, ".house"))
				houses.push_back(fileInPath->d_name);
		}
	}

	else{
		cout << houseError << endl; // TODO throw custom exception (create our own class - perhaps even in the simulator header)
	}
	
	return houses;
}

//returns true if filename ends with suffix
bool EndsWith(const string& filename, const string& suffix) {
	if (suffix.size() > filename.size()) 
		return false;
	return equal(filename.begin() + filename.size() - suffix.size(), filename.end(), suffix.begin());
}

//makes sure that the surrounding of the house is a wall
void check_house_surroundings(House& house){
	int i, j;
	int lastRow = house.getNumRows - 1;
	int lastCol = house.getnumCols - 1;

	for (i = 0; i < house.getNumRows(); i++){
		if (i == 0)
			for (j = 0; j < house.getnumCols(); j++){
				if (house.getMatrix[0][j] != WALL)
					house.getMatrix[0][j] = WALL;
			}

		else if (i == lastRow)
			for (j = 0; j < house.getnumCols(); j++){
				if (house.getMatrix[lastRow][j] != WALL)
					house.getMatrix[lastRow][j] = WALL;
			}
		else{
			if (house.getMatrix[i][0] != WALL)
				house.getMatrix[i][0] = WALL;
			if (house.getMatrix[i][lastCol] != WALL)
				house.getMatrix[i][lastCol] = WALL;
		}

	}
}

//returns the sum of dust in the house, for the simulator to know when the robot is done cleaning.
//should be called before the robot starts working.
int totalDust(House& house){
	int i, j, sum = 0;
	Position pos = { 0, 0 };
	for (i = 0; i < house.getNumRows(); i++){
		for (j = 0; j < house.getnumCols(); j++){
			pos = { i, j };
			sum += house.getDirtLevel(pos);
		}
	}

	return sum;
}
