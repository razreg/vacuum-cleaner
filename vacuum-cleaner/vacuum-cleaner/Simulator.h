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
	list<House>& houseList;
	list<Robot> robots;
	Score** scoreMatrix; // basic score matrix - will be improved according to ex2 instructions

	void initScoreMatrix();

	void initRobotList(list<AbstractAlgorithm*>& algorithms);

	void collectScores(int houseCount, int winnerNumSteps);

	void printScoreMatrix();

	void updateRobotListWithHouse(House& house, int houseCount);

	void executeOnHouse(House& house, int maxSteps, int maxStepsAfterWinner, int houseCount);

	void robotFinishedCleaning(Robot& robot, int steps, int& winnerNumSteps, int algorithmCount, 
		int houseCount, int positionInCompetition, int& robotsFinishedInRound);

	void performStep(Robot& robot, int steps, int maxSteps, int maxStepsAfterWinner,
		int stepsAfterWinner, int algorithmCount, int houseCount, House& house); //**IDO**

public:
	
	Simulator(map<string, int>& configMap, list<House>& houseList, list<AbstractAlgorithm*>& algorithms) :
		configMap(configMap), houseList(houseList) {
		initRobotList(algorithms);
		initScoreMatrix();
	};

	~Simulator() {
		// TODO destroy scoreMatrix and create copy constructor + copy assignment operator
	};

	void execute();

};

#endif // __SIMULATOR__H_