#ifndef __MAIN__H_
#define __MAIN__H_

#include <dlfcn.h>
#include <cctype>

#include "Simulator.h"
#include "House.h"
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "AlgorithmRegistrar.h"

// exit codes
const int SUCCESS = 0;
const int INTERNAL_FAILURE = 1;
const int INVALID_ARGUMENTS = 2;
const int INVALID_CONFIGURATION = 3;
const int INVALID_HOUSES = 4;
const int INVALID_ALGORITHMS = 5;

const string SCORE_FORMULA_PLACEHOLDER = "&%$+#@(*-6~78)i!dfa4//=";

string getCurrentWorkingDirectory();

bool loadConfiguration(const string& configFileDir, map<string, int>& configMap, string& usage);

void populateConfigMap(ifstream& configFileStream, map<string, int>& configMap);

bool loadHouseList(const string& housesPath, list<House>& houseList, vector<string>& errors, string& usage);

bool loadAlgorithms(const string& algorithmsPath, list<unique_ptr<AbstractAlgorithm>>& algorithms,
	list<string>& algorithmNames, vector<string>& errors, string& usage);

bool parseArgs(int argc, char** argv, string& configPath, string& housesPath, string& algorithmsPath,
	string &scoreFormula, size_t& threads);

bool isConfigMapValid(map<string, int>& configMap);

void trimString(string& str);

void printErrors(vector<string>& houseErrors, vector<string>& algorithmErrors, vector<string>& simulationErrors);

bool isDirectory(fs::path& dir) {
	return fs::exists(dir) && fs::is_directory(dir);
}

void printStringVector(vector<string>& vec);

template<typename T>
bool allLoadingFailed(list<T>& loadedObjectsList, vector<string>& errors, string& usage, 
	string typeName, fs::path& dir);

#endif //__MAIN__H_