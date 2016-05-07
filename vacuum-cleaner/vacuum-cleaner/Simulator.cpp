#include "Simulator.h"

Logger Simulator::logger = Logger("Simulator");
atomic<size_t> Simulator::housePathIndex(0);

Simulator::Simulator(map<string, int>& configMap, ScoreFormula scoreFormula, 
	vector<fs::path>& housePathVector, AlgorithmRegistrar& registrar, vector<string>& algorithmErrors) :
	configMap(configMap), housePathVector(housePathVector), registrar(registrar), 
	algorithmErrors(algorithmErrors), results() {
	maxStepsAfterWinner = configMap.find(MAX_STEPS_AFTER_WINNER)->second;
	vector<string> houseNames;
	for (fs::path& housePath : housePathVector) {
		houseNames.push_back(housePath.stem().string());
	}
	results = Results(registrar.getAlgorithmNames(), move(houseNames), scoreFormula);
}

void Simulator::execute(size_t numThreads) {
	size_t actualNumThreads = min(housePathVector.size(), numThreads);
	vector<unique_ptr<thread>> threads(actualNumThreads);
	for (auto& thread_ptr : threads) {
		thread_ptr = make_unique<thread>(&Simulator::executeThread, this);
	}
	for (auto& thread_ptr : threads) {
		thread_ptr->join();
	}
	results.print(simulationErrors);
	printErrors();
}

void Simulator::initRobotList(list<Robot>& robots, list<unique_ptr<AbstractAlgorithm>>& algorithms) {
	
	list<string> algorithmNames = registrar.getAlgorithmNames();
	logger.debug("Initializing robot list");
	auto namesIter = algorithmNames.begin();
	for (auto iter = algorithms.begin(); 
	iter != algorithms.end() && namesIter != algorithmNames.end(); ++iter, ++namesIter) {
		robots.emplace_back(configMap, **iter, *namesIter);
	}
}

void Simulator::collectScores(list<Robot>& robots, string houseName, int simulationSteps, int winnerNumSteps) {
	for (Robot& robot : robots) {
		string algorithmName = robot.getAlgorithmName();
		results[algorithmName][houseName].setIsBackInDocking(robot.inDocking());
		results[algorithmName][houseName].setSimulationSteps(simulationSteps);
		results[algorithmName][houseName].setWinnerNumSteps(winnerNumSteps);
		results[algorithmName][houseName].setSumDirtInHouse(robot.getHouse().getTotalDust());
		/*if (logger.debugEnabled()) {
			logger.debug("House final state for algorithm [" + robot.getAlgorithmName() + "]:\n"
				+ (string)robot.getHouse());
		}*/
	}
}

void Simulator::updateRobotListWithHouse(list<Robot>& robots, House& house) {
	logger.debug("Defining house [" + house.getName() + "] for robot list");
	for (Robot& robot : robots) {
		robot.restart();
		robot.setHouse(House(house));
	}
}

void Simulator::executeThread() {

	list<unique_ptr<AbstractAlgorithm>> algorithms = registrar.getAlgorithms();

	list<Robot> robots;
	initRobotList(robots, algorithms);

	size_t idx;
	while ((idx = housePathIndex.fetch_add(1)) < housePathVector.size()) {
		House house;
		fs::path filePath = housePathVector[idx];
		bool isValid = loadHouse(filePath, house);
		if (!isValid) {
			results.removeHouse(filePath.stem().string());
			continue;
		}
		updateRobotListWithHouse(robots, house);
		logger.info("Simulation started on house [" + house.getName() + "]");
		executeOnHouse(robots, house);
	}
}

void Simulator::executeOnHouse(list<Robot>& robots, House& house) {

	int maxSteps = house.getMaxSteps();
	int steps = 0;
	int winnerNumSteps = 0;
	int stepsAfterWinner = -1; // so when we increment for first time, when winner is found, it will be set to zero
	int positionInCompetition = 1;
	size_t robotsFinishedTotal = 0;
	while (steps < maxSteps && stepsAfterWinner < maxStepsAfterWinner && robotsFinishedTotal < robots.size()) {
		int robotsFinishedInRound = 0;
		robotsFinishedTotal = 0;
		for (Robot& robot : robots) {
			if (robot.isFinished()) {
				robotsFinishedTotal++;
			}
			else if (!robot.performedIllegalStep()) {
				if (robot.getBatteryValue() == 0) {
					// if we didn't alreay notify that the battery died
					if (!robot.isBatteryDeadNotified()) {
						robot.setBatteryDeadNotified();
						logger.info("Robot using algorithm [" + robot.getAlgorithmName() + "] has dead battery");
					}
					results[robot.getAlgorithmName()][house.getName()].setThisNumSteps(steps + 1); // increment steps but stay
				}
				else {
					performStep(robot, steps, maxSteps, maxStepsAfterWinner, stepsAfterWinner);
				}

				// robot finished cleaning?
				if (robot.isFinished()) {
					robotFinishedCleaning(robot, steps, winnerNumSteps, positionInCompetition, robotsFinishedInRound);
				}
				else {
					// this is the actual position
					results[robot.getAlgorithmName()][house.getName()]
						.setPositionInCompetition(positionInCompetition);
				}
			}
		}

		// this may only happen if the house was clean when simulation started (edge case)
		if (robotsFinishedTotal < robots.size() || robotsFinishedInRound > 0) {
			steps++;
		}
		if (winnerNumSteps > 0) {
			stepsAfterWinner++;
		}
		positionInCompetition = positionInCompetition + robotsFinishedInRound;
	}
	// fallback if there is no winner
	if (winnerNumSteps == 0) {
		winnerNumSteps = steps;
	}
	collectScores(robots, house.getName(), steps, winnerNumSteps);
	logger.info("Simulation completed for house [" + house.getName() + "]");
}

void Simulator::robotFinishedCleaning(Robot& robot, int steps, int& winnerNumSteps, 
	int positionInCompetition, int& robotsFinishedInRound) {

	logger.info("Algorithm [" + robot.getAlgorithmName() + "] has successfully cleaned the house and returned to docking station");
	// update the number of steps the winner has performed if no winner was found before
	if (winnerNumSteps == 0) {
		winnerNumSteps = steps + 1;
	}
	// update position in competition
	results[robot.getAlgorithmName()][robot.getHouse().getName()]
		.setPositionInCompetition(positionInCompetition);
	robotsFinishedInRound++;
}

void Simulator::performStep(Robot& robot, int steps, int maxSteps, int maxStepsAfterWinner, int stepsAfterWinner) {

	string houseName = robot.getHouse().getName();
	string algorithmName = robot.getAlgorithmName();

	// notify on aboutToFinish if there is a winner or steps == maxSteps - maxStepsAfterWinner
	if (stepsAfterWinner == 0 || steps == max(0, maxSteps - maxStepsAfterWinner)) {
		if (logger.debugEnabled()) {
			logger.debug("Notifying algorithm [" + robot.getAlgorithmName() + "] that the simulation is about to end");
		}
		robot.aboutToFinish(min(maxStepsAfterWinner, maxSteps - steps));
	}
	robot.step(); // this also updates the sensor and the battery but not the house
	if (!robot.getHouse().isInside(robot.getPosition()) ||
		robot.getHouse().isWall(robot.getPosition())) {
		logger.warn("Algorithm [" + robot.getAlgorithmName() +
			"] has performed an illegal step. Robot in position="
			+ (string)robot.getPosition());
		results[algorithmName][houseName].reportBadBehavior();
		robot.reportBadBehavior();
		simulationErrors.push_back("Algorithm " + robot.getAlgorithmName() + " when running on House " 
			+ houseName + " went on a wall in step " + to_string(steps + 1));
	}

	// perform one cleaning step
	if (robot.getHouse().clean(robot.getPosition())) {
		results[algorithmName][houseName].incrementDirtCollected();
	} 
	results[algorithmName][houseName].setThisNumSteps(steps + 1);
}

bool Simulator::loadHouse(fs::path filePath, House& house) {
	try {
		house = House::deseriallize(filePath);
		logger.debug("Validating house");
		logger.debug("Validating house walls");
		house.validateWalls();
		logger.debug("Validating the existence of exactly one docking station");
		house.validateDocking();
		logger.debug("House is valid");
	}
	catch (exception& e) {
		logger.debug("House is invalid");
		houseErrors.push_back(e.what());
		return false;
	}
	return true;
}

void Simulator::printErrors() {
	if (results.areAllHousesInvalid()) {
		cout << "All house files in target folder '"
			<< housePathVector.front().parent_path().string() 
			<< "' cannot be opened or are invalid:" << endl;
		for (string& err : houseErrors) cout << err << endl;
		return;
	}
	if (!houseErrors.empty() || !algorithmErrors.empty() || !simulationErrors.empty()) {
		cout << endl;
		cout << "Errors:" << endl;
	}
	for (string& err : houseErrors) cout << err << endl;
	for (string& err : algorithmErrors) cout << err << endl;
	for (string& err : simulationErrors) cout << err << endl;
}