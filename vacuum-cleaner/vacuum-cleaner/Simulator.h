#pragma once
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <list>
#include <dirent.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include "House.h"
#include "NaiveAlgorithm.h"
#include "SensorImpl.h"
#include "Score.h"
#include "Robot.h"
#include "Common.h"

// exit codes
const int SUCCESS = 0;
const int INVALID_ARGUMENTS = 1;
const int INVALID_CONFIGURATION = 2;
