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
	#define DIR_SEPARATOR '\\' 
#endif
#ifdef __linux__
	#include <unistd.h>
	#define getCurrentWorkingDir getcwd
	#define DIR_SEPARATOR '/'
#endif

// exit codes
const int SUCCESS = 0;
const int INTERNAL_FAILURE = 1;
const int INVALID_ARGUMENTS = 2;
const int INVALID_CONFIGURATION = 3;

string getCurrentWorkingDirectory();
void loadConfiguration(const string& configFileDir, map<string, int>& configMap);
void loadHouseList(const string& housesPath, list<House*>& houseList);

#endif //__MAIN__H_