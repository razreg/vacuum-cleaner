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


class Simulator {

	static Logger logger;

	list<House*>& houseList;
	map<string, int>& configMap;
	list<AbstractAlgorithm*>& algorithms;

public:
	
	Simulator(map<string, int>& configMap, list<House*>& houseList, list<AbstractAlgorithm*>& algorithms) :
		configMap(configMap), houseList(houseList), algorithms(algorithms) {};

	~Simulator() {
		for (House* house : houseList) {
			delete house;
		}
		houseList.clear();
	};

	void execute();

};

#endif // __SIMULATOR__H_