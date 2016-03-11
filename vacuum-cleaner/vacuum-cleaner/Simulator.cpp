#include "Simulator.h"

using namespace std;

// Usage: simulator -config <config_file_location> -house_path <houses_path_location>
int main(int argc, char** argv) {
	
	string usageError = "Error: Invalid arguments. "
		"Usage: simulator -config <config_file_location> -house_path <houses_path_location>";

	if (argc != 5 || argv[1] != "-config" || argv[3] != "-house") {
		cout << usageError << endl;
		return INVALID_ARGUMENTS;
	}
	Configuration& config = getConfiguration(argv[2]); // TODO catch exception and exit gracefully?

	// TODO load houses into an array of House objects
	int num = 8; // TODO change 8 to the number of *.house files in the directory (need to list files)
	House* houses = new House[num]; 
	for (int i = 0; i < num; ++i) {
		houses[i] = House::deseriallize(""); // TODO give path to next house (we have to list the houses in the dir)
	}

	// TODO init NaiveAlgorithm (including setSensor)
	// TODO loop until finish (maxSteps, house clean, etc.) and execute the algorithm steps on the house - 
	//      each algorithm must have a house of its own so we need to clone the house before passing it 
	//      to the algorithm!
	//      in the loop - when we init a house, init a Score as well
	
	delete [] houses;
	return SUCCESS;
}

Configuration getConfiguration(string configFilePath) {
	
	string configError = "Error: configuration file [" + configFilePath + "] is invalid";

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
}