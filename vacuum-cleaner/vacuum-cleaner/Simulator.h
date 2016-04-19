#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include <list>

#include "Results.h"
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
	Results* results = nullptr;

	void initRobotList(list<unique_ptr<AbstractAlgorithm>>& algorithms, list<string>& algorithmNames);

	void collectScores(string houseName, int winnerNumSteps);

	void updateRobotListWithHouse(House& house);

	void executeOnHouse(House& house, int maxSteps, int maxStepsAfterWinner);

	void robotFinishedCleaning(Robot& robot, int steps, int& winnerNumSteps, 
		int positionInCompetition, int& robotsFinishedInRound);

	void performStep(Robot& robot, int steps, int maxSteps, int maxStepsAfterWinner, int stepsAfterWinner);

public:
	
	// TODO move implementation to Simulator.cpp
	Simulator(map<string, int>& configMap, list<House>& houseList, 
		list<unique_ptr<AbstractAlgorithm>>& algorithms, list<string>&& algorithmNames) :
		configMap(configMap), houseList(houseList) {
		initRobotList(algorithms, algorithmNames);
		vector<string> houseNames;
		for (House& house : houseList) {
			houseNames.push_back(house.getName());
		}
		// TODO insert algorithm names
		results = new Results(algorithmNames, move(houseNames)); // TODO remove new?
	};

	// TODO create copy constructor + copy assignment operator
	~Simulator() {
		if (results != nullptr) {
			delete results;
		}
	};

	void execute();

};

#endif // __SIMULATOR__H_