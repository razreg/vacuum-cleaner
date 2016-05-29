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
#include "Video.h"

using namespace std;
namespace fs = boost::filesystem;

class Simulator {

	static Logger logger; // internal mutex - thread-safe

	map<string, int>& configMap; // never updated in simulator - thread-safe
	int maxStepsAfterWinner; // never updated in threads - thread-safe
	vector<fs::path>& housePathVector; // accessed with housePathIndex - thread-safe
	AlgorithmRegistrar& registrar; // never updated in simulator - thread-safe

	vector<string> simulationErrors; // updates in threads only when errorMutex is locked - thread-safe
	vector<string> houseErrors; // updates in threads only when errorMutex is locked - thread-safe
	vector<string>& algorithmErrors; // never updated in simulator - thread-safe
	Results results; // updates in threads only when resultsMutex is locked - thread-safe
	bool captureVideo;

	// thread-safety devices
	atomic<size_t> housePathIndex;
	mutex errorMutex;
	mutex resultsMutex;

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

	void printErrors() const;

	void saveVideos(const list<Robot>& robots) const;

public:
	
	Simulator(map<string, int>& configMap, ScoreFormula scoreFormula, vector<fs::path>& housePathVector, 
		AlgorithmRegistrar& registrar, vector<string>& algorithmErrors, bool video = false);

	void execute(size_t numThreads);

};

#endif // __SIMULATOR__H_