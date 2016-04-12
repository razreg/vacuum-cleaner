#ifndef __MAIN__H_
#define __MAIN__H_

#include "Simulator.h"
#include "NaiveAlgorithm.h"
#include "House.h"

// file handling
#include <cstdio>
#include <regex>
#ifdef _WIN32
	#include <direct.h>
	#define getCurrentWorkingDir _getcwd
#endif
#ifdef __linux__
	#include <unistd.h>
	#define getCurrentWorkingDir getcwd
#endif

// exit codes
const int SUCCESS = 0;
const int INTERNAL_FAILURE = 1;
const int INVALID_ARGUMENTS = 2;
const int INVALID_CONFIGURATION = 3;
const int INVALID_HOUSES = 4;

string getCurrentWorkingDirectory();

bool loadConfiguration(const string& configFileDir, map<string, int>& configMap, string& usage);

void populateConfigMap(ifstream& configFileStream, map<string, int>& configMap);

bool loadHouseList(const string& housesPath, list<House>& houseList, vector<string>& errors, string& usage);

bool parseArgs(int argc, char** argv, string& configPath, string& housesPath, string& algorithmsPath);

bool isConfigMapValid(map<string, int>& configMap);

void trimString(string& str);

void printErrors(vector<string>& houseErrors, vector<string>& algorithmErrors);

#endif //__MAIN__H_