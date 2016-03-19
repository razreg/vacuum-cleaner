#ifndef __MAIN__H_
#define __MAIN__H_

#include "Simulator.h"

#ifdef _WIN32
	#include <direct.h>
	#define getCurrentWorkingDir _getcwd
#endif
#ifdef linux
	#include <unistd.h>
	 getCurrentWorkingDir getcwd
#endif


// exit codes
const int SUCCESS = 0;
const int INTERNAL_FAILURE = 1;
const int INVALID_ARGUMENTS = 2;
const int INVALID_CONFIGURATION = 3;

string getCurrentWorkingDirectory();

#endif //__MAIN__H_