#pragma once
#include <string>
#include <map>
#include <iostream>
#include <list>

// file handling
#include <fstream>
#include <stdio.h>
#include <filesystem> // TODO make sure we may use this (it is experimental!)
#include <regex>
#ifdef _WIN32
	#include <direct.h>
	#define getCurrentWorkingDir _getcwd
#endif
#ifdef linux
	#include <unistd.h>
	#define getCurrentWorkingDir getcwd
#endif

#include "House.h"
#include "NaiveAlgorithm.h"
#include "SensorImpl.h"
#include "Score.h"
#include "Robot.h"
#include "Common.h"

// exit codes
const int SUCCESS = 0;
const int INTERNAL_FAILURE = 1;
const int INVALID_ARGUMENTS = 2;
const int INVALID_CONFIGURATION = 3;
