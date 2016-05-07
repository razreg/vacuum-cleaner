#ifndef __SIMULATOR__H_
#define __SIMULATOR__H_

#include "Results.h"
#include "Robot.h"
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "House.h"
#include "Position.h"
#include "Common.h"
#include "AlgorithmRegistrar.h"

using namespace std;
namespace fs = boost::filesystem;

class Simulator {

	static Logger logger;
	static atomic<size_t> housePathIndex;

	map<string, int>& configMap;
	int maxStepsAfterWinner;
	vector<fs::path>& housePathVector;
	AlgorithmRegistrar& registrar;
	vector<string> simulationErrors;
	vector<string> houseErrors;
	vector<string>& algorithmErrors;
	Results results;

	void initRobotList(list<Robot>& robots, list<unique_ptr<AbstractAlgorithm>>& algorithms);

	void collectScores(list<Robot>& robots, string houseName, int simulationSteps, int winnerNumSteps);

	void updateRobotListWithHouse(list<Robot>& robots, House& house);

	void executeThread();

	bool loadHouse(fs::path filePath, House& house);

	void executeOnHouse(list<Robot>& robots, House& house);

	void robotFinishedCleaning(Robot& robot, int steps, int& winnerNumSteps, 
		int positionInCompetition, int& robotsFinishedInRound);

	void performStep(Robot& robot, int steps, int maxSteps, 
		int maxStepsAfterWinner, int stepsAfterWinner);

	void printErrors();

public:
	
	Simulator(map<string, int>& configMap, ScoreFormula scoreFormula, 
		vector<fs::path>& housePathVector, AlgorithmRegistrar& registrar, vector<string>& algorithmErrors);

	void execute(size_t numThreads);

};

#endif // __SIMULATOR__H_