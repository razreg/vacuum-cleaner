#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <map>
#include <list>
#include <stdexcept>

#include "Score.h"
#include "Robot.h"
#include "Common.h"
//#include "SensorImpl.h"
//#include "NaiveAlgorithm.h"
//#include "House.h"


class Simulator{

	static Logger logger;

	list<House>& houseList;
	map<string, int>& configMap;

public:
	
	Simulator(map<string, int>& configMap, list<House>& houseList) : 
		configMap(configMap), houseList(houseList) {};

	void start();

};

#endif // __SIMULATOR__H_