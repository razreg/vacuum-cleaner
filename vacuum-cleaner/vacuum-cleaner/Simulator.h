#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <map>
#include <list>

#include "Score.h"
#include "Robot.h"
#include "AbstractAlgorithm.h"
#include "House.h"
#include "Position.h"
#include "Common.h"

class Simulator {

	static Logger logger;

	map<string, int>& configMap;
	list<House*>& houseList;
	list<AbstractAlgorithm*>& algorithms;

	Score** Simulator::initScoreMatrix();
	void Simulator::initRobotList(list<Robot*>& robots);
	void Simulator::collectScores(Score** scoreMatrix, list<Robot*>& robots, 
		int houseCount, int winnerNumSteps);
	void Simulator::printScoreMatrix(Score** scoreMatrix);
	void Simulator::updateRobotListWithHouse(list<Robot*>& robots, House& house, int houseCount);
	void executeOnHouse(House* house, list<Robot*>& robots, Score** scoreMatrix, 
		int maxSteps, int maxStepsAfterWinner, int houseCount);

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