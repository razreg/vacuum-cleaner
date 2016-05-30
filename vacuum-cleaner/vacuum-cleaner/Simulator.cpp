#include "Simulator.h"

Logger Simulator::logger = Logger("Simulator");

Simulator::Simulator(map<string, int>& configMap, ScoreFormula scoreFormula, vector<fs::path>& housePathVector, 
	AlgorithmRegistrar& registrar, vector<string>& algorithmErrors, bool video) :
	configMap(configMap), housePathVector(housePathVector), registrar(registrar), 
	algorithmErrors(algorithmErrors), results(), housePathIndex(0), captureVideo(video) { 
	
	maxStepsAfterWinner = configMap.find(MAX_STEPS_AFTER_WINNER)->second;
	vector<string> houseNames;
	for (fs::path& housePath : housePathVector) {
		houseNames.push_back(housePath.stem().string());
	}
	sort(houseNames.begin(), houseNames.end());
	results = Results(registrar.getAlgorithmNames(), move(houseNames), scoreFormula);
}

void Simulator::execute(size_t numThreads) {
	size_t numHouses = housePathVector.size();
	size_t actualNumThreads = min(numHouses, numThreads);
	logger.info("Running simulation with " + to_string(actualNumThreads) + " thread(s)" + 
		(numHouses < numThreads ? " (" + to_string(numThreads) + 
			" requested but only " + to_string(numHouses) + " house files were found)": ""));
	if (actualNumThreads > 1 && !captureVideo) {
		vector<unique_ptr<thread>> threads(actualNumThreads);
		for (auto& thread_ptr : threads) {
			thread_ptr = make_unique<thread>(&Simulator::executeThread, this);
		}
		for (auto& thread_ptr : threads) {
			thread_ptr->join();
		}
	}
	else {
		executeThread();
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
		robots.emplace_back(configMap, **iter, *namesIter, captureVideo);
	}
}

void Simulator::collectScores(list<Robot>& robots, string houseName, int simulationSteps, int winnerNumSteps) {
	for (Robot& robot : robots) {
		string algorithmName = robot.getAlgorithmName();
		lock_guard<mutex> lockResults(resultsMutex);
		results[algorithmName][houseName].setIsBackInDocking(robot.inDocking());
		results[algorithmName][houseName].setSimulationSteps(simulationSteps);
		results[algorithmName][houseName].setWinnerNumSteps(winnerNumSteps);
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

	logger.debug("Thread started");
	
	size_t houseCount = 0;
	size_t failedHouseCount = 0;

	list<unique_ptr<AbstractAlgorithm>> algorithms = registrar.getAlgorithms();

	list<Robot> robots;
	initRobotList(robots, algorithms);

	size_t idx;
	while ((idx = housePathIndex.fetch_add(1)) < housePathVector.size()) {
		House house;
		fs::path filePath = housePathVector[idx];
		houseCount++;
		bool isValid = loadHouse(filePath, house);
		if (!isValid) {
			failedHouseCount++;
			lock_guard<mutex> lockResults(resultsMutex);
			results.removeHouse(filePath.stem().string());
			continue;
		} 
		updateRobotListWithHouse(robots, house);
		logger.info("Simulation started on house [" + house.getName() + "]");
		executeOnHouse(robots, house);
	}

	logger.info("Thread closing after running on " + to_string(houseCount) + " house(s)" + 
		(failedHouseCount > 0 ? " (" + to_string(failedHouseCount) + " invalid)" : ""));
}

void Simulator::executeOnHouse(list<Robot>& robots, House& house) {

	int maxSteps = house.getMaxSteps();
	int steps = 0;
	int winnerNumSteps = 0;
	int stepsAfterWinner = -1; // so when we increment for first time, when winner is found, it will be set to zero
	int positionInCompetition = 1;
	size_t robotsFinishedTotal = 0;

	size_t initialSumDirtInHouse = house.getTotalDust();
	for (Robot& robot : robots) {
		results[robot.getAlgorithmName()][house.getName()]
			.setSumDirtInHouse(initialSumDirtInHouse);
	}

	while (steps < maxSteps && stepsAfterWinner < maxStepsAfterWinner 
		&& robotsFinishedTotal < robots.size()) {

		int robotsFinishedInRound = 0;
		robotsFinishedTotal = 0;
		for (Robot& robot : robots) {
			if (robot.isFinished() || robot.performedIllegalStep()) {
				robotsFinishedTotal++;
			}
			else if (!robot.performedIllegalStep()) {
				if (robot.getBatteryValue() == 0) {
					// if we didn't alreay notify that the battery died
					if (!robot.isBatteryDeadNotified()) {
						robot.setBatteryDeadNotified();
						logger.info("Robot using algorithm [" + robot.getAlgorithmName() + "] has dead battery");
					}
					lock_guard<mutex> lockResults(resultsMutex);
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
					lock_guard<mutex> lockResults(resultsMutex);
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
	if (captureVideo) saveVideos(robots);
	logger.info("Simulation completed for house [" + house.getName() + "]");
}

void Simulator::saveVideos(list<Robot>& robots) {
	for (Robot& robot : robots) {
		// wait a bit at the end of the video so it won't end so abruptly
		for (int i = 0; i < 5; ++i) {
			robot.captureSnapshot();
		}
		robot.saveVideo();
	}
}

void Simulator::robotFinishedCleaning(Robot& robot, int steps, int& winnerNumSteps, 
	int positionInCompetition, int& robotsFinishedInRound) {

	logger.info("Algorithm [" + robot.getAlgorithmName() + "] has successfully cleaned the house and returned to docking station");
	// update the number of steps the winner has performed if no winner was found before
	if (winnerNumSteps == 0) {
		winnerNumSteps = steps + 1;
	}
	// update position in competition
	lock_guard<mutex> lockResults(resultsMutex);
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
	if (captureVideo) robot.captureSnapshot();
	if (!robot.getHouse().isInside(robot.getPosition()) ||
		robot.getHouse().isWall(robot.getPosition())) {
		logger.warn("Algorithm [" + robot.getAlgorithmName() +
			"] has performed an illegal step. Robot in position="
			+ (string)robot.getPosition());
		{
			lock_guard<mutex> lockResults(resultsMutex);
			results[algorithmName][houseName].reportBadBehavior();
		}
		robot.reportBadBehavior();

		lock_guard<mutex> lockErrors(errorMutex);
		simulationErrors.push_back("Algorithm " + robot.getAlgorithmName() + " when running on House " 
			+ houseName + " went on a wall in step " + to_string(steps + 1));
	}

	// perform one cleaning step
	if (robot.getHouse().clean(robot.getPosition())) {
		lock_guard<mutex> lockResults(resultsMutex);
		results[algorithmName][houseName].incrementDirtCollected();
	} 
	lock_guard<mutex> lockResults(resultsMutex);
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
		lock_guard<mutex> lockErrors(errorMutex);
		houseErrors.push_back(e.what());
		return false;
	}
	return true;
}

void Simulator::printErrors() const {
	if (results.areAllHousesInvalid()) {
		fs::path path = housePathVector.front().parent_path();
		if (fs::exists(path)) {
			path = fs::canonical(path);
		}
		cout << "All house files in target folder '"
			<< path.string()
			<< "' cannot be opened or are invalid:" << endl;
		for (const string& err : houseErrors) cout << err << endl;
		return;
	}
	if (!houseErrors.empty() || !algorithmErrors.empty() || !simulationErrors.empty()) {
		cout << endl;
		cout << "Errors:" << endl;
	}
	for (const string& err : houseErrors) cout << err << endl;
	for (const string& err : algorithmErrors) cout << err << endl;
	for (const string& err : simulationErrors) cout << err << endl;
}