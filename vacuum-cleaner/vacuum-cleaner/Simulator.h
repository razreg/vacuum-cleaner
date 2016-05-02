#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include "Results.h"
#include "Robot.h"
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "House.h"
#include "Position.h"
#include "Common.h"

class Simulator {

	static Logger logger;

	map<string, int>& configMap;
	list<House>& houseList;
	list<Robot> robots;
	Results results;
	vector<string> errors;

	void initRobotList(list<unique_ptr<AbstractAlgorithm>>& algorithms, list<string>& algorithmNames);

	void collectScores(string houseName, int simulationSteps, int winnerNumSteps);

	void updateRobotListWithHouse(House& house);

	void executeOnHouse(House& house, int maxSteps, int maxStepsAfterWinner);

	void robotFinishedCleaning(Robot& robot, int steps, int& winnerNumSteps, 
		int positionInCompetition, int& robotsFinishedInRound);

	void performStep(Robot& robot, int steps, int maxSteps, 
		int maxStepsAfterWinner, int stepsAfterWinner);

public:
	
	Simulator(map<string, int>& configMap, ScoreFormula scoreFormula, list<House>& houseList, 
		list<unique_ptr<AbstractAlgorithm>>& algorithms, list<string>&& algorithmNames);

	vector<string> execute(); // returns simulation errors

};

#endif // __SIMULATOR__H_