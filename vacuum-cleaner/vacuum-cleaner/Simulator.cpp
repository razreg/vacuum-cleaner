#include "Simulator.h"

using namespace std;
namespace fs = boost::filesystem;

// TODO go over usage
// Usage: simulator -config <config_file_location> -house_path <houses_path_location>
//    or: simulator -house_path <houses_path_location> -config <config_file_location>
//    if either one of them is missing- look for it in the working directory
int main(int argc, char** argv) {
	
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
	}
	getHouseList(housesPath, houseList); // TODO catch exception and exit gracefully?
	getConfiguration(configPath, configMap); // TODO catch exception and exit gracefully?

	// TODO init NaiveAlgorithm (including setSensor ***and setConfiguration***)
	// TODO loop until finish (maxSteps, house clean, etc.) and execute the algorithm steps on the house - 
	//      each algorithm must have a house of its own so we need to clone the house before passing it 
	//      to the algorithm!
	//      in the loop - when we init a house, init a Score as well
	
	// TODO delete all houses and configuration map
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
				house.validateWalls(); // TODO handle exception
				houses.push_back(house);
			}
		}
	}
	catch (fs::filesystem_error e) {
		cout << houseError << endl; // TODO throw custom exception (create our own class - perhaps even in the simulator header)
	}
}

