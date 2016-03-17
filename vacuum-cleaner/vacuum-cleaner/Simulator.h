#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <string>
#include <map>
#include <iostream>
#include <list>

// file handling
#include <fstream>
#include <stdio.h>
#include <filesystem> // TODO remove - do not use lib from experimental
#include <regex>
#ifdef _WIN32
	#include <direct.h>
	#define getCurrentWorkingDir _getcwd
#endif
#ifdef linux
	#include <unistd.h>
	#define getCurrentWorkingDir getcwd
#endif

#include "NaiveAlgorithm.h"
#include "Score.h"
#include "Robot.h"

// exit codes
const int SUCCESS = 0;
const int INTERNAL_FAILURE = 1;
const int INVALID_ARGUMENTS = 2;
const int INVALID_CONFIGURATION = 3;

#endif // __SIMULATOR__H_