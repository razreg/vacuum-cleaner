#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <map>
#include <list>
#include <stdexcept>

// file handling
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <stdio.h>
//#include <filesystem> // TODO remove - do not use lib from experimental
#include <regex>
	

#include "Score.h"
#include "Robot.h"
//#include "Common.h"
//#include "SensorImpl.h"
//#include "NaiveAlgorithm.h"
//#include "House.h"


class Simulator{

	list<House> houseList;
	map<string, int> configMap;

public:
	
	//TODO constructor-destructor
	Simulator();

	~Simulator();

	void setConfiguration(const string& configFileDir);

	void setHouseList(string housesPath);

	list<House> getHouseList() {
		return this->houseList;
	}

	map<string, int> getConfigMap() {
		return this->configMap;
	}

	bool EndsWith(const string& filename, const string& suffix);

};

#endif // __SIMULATOR__H_