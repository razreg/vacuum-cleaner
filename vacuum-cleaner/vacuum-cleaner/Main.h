#ifndef __MAIN__H_
#define __MAIN__H_

#include "Simulator.h"
#include "House.h"
#include "AbstractAlgorithm.h"
//#include "AlgorithmFactory.h"
#include "AlgorithmRegistrar.h"

// file handling
#include <dlfcn.h>
#include <cstdio>
#include <regex>
#include <unistd.h>

// exit codes
const int SUCCESS = 0;
const int INTERNAL_FAILURE = 1;
const int INVALID_ARGUMENTS = 2;
const int INVALID_CONFIGURATION = 3;
const int INVALID_HOUSES = 4;
const int INVALID_ALGORITHMS = 5;

string getCurrentWorkingDirectory();

bool loadConfiguration(const string& configFileDir, map<string, int>& configMap, string& usage);

void populateConfigMap(ifstream& configFileStream, map<string, int>& configMap);

bool loadHouseList(const string& housesPath, list<House>& houseList, vector<string>& errors, string& usage);

bool loadAlgorithms(const string& algorithmsPath, list<unique_ptr<AbstractAlgorithm>>& algorithms,
	list<string>& algorithmNames, vector<string>& errors, string& usage);

bool parseArgs(int argc, char** argv, string& configPath, string& housesPath, string& algorithmsPath);

bool isConfigMapValid(map<string, int>& configMap);

void trimString(string& str);

void printErrors(vector<string>& houseErrors, vector<string>& algorithmErrors);

bool isDirectory(fs::path& dir, string& usage);

template<typename T>
bool allLoadingFailed(list<T>& loadedObjectsList, vector<string>& errors, string& usage, 
	string typeName, fs::path& dir);

#endif //__MAIN__H_