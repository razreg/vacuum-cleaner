#ifndef __MAIN__H_
#define __MAIN__H_

#include "Simulator.h"
#include <stdexcept>

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

string getCurrentWorkingDirectory();

void printScoreTable(map<string, int> scoreTable);


#endif //__MAIN__H_