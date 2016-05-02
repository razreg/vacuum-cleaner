#include "Simulator.h"

using namespace std;

Logger Simulator::logger = Logger("Simulator");

Simulator::Simulator(map<string, int>& configMap, ScoreFormula scoreFormula, list<House>& houseList,
	list<unique_ptr<AbstractAlgorithm>>& algorithms, list<string>&& algorithmNames) :
	configMap(configMap), houseList(houseList), results() {
	initRobotList(algorithms, algorithmNames);
	vector<string> houseNames;
	for (House& house : houseList) {
		houseNames.push_back(house.getName());
	}
	results = Results(algorithmNames, move(houseNames), scoreFormula);
}

vector<string> Simulator::execute() {

	int maxStepsAfterWinner = configMap.find(MAX_STEPS_AFTER_WINNER)->second;

	for (House& house : houseList) {
		logger.info("Simulation started for house [" + house.getName() + "]");
		updateRobotListWithHouse(house);
		// run algorithms in "Round Robin" fashion
		executeOnHouse(house, house.getMaxSteps(), maxStepsAfterWinner);
	}
	results.print(errors);
	return errors;
}

void Simulator::initRobotList(list<unique_ptr<AbstractAlgorithm>>& algorithms, list<string>& algorithmNames) {
	logger.debug("Initializing robot list");
	auto namesIter = algorithmNames.begin();
	for (auto iter = algorithms.begin(); 
	iter != algorithms.end() && namesIter != algorithmNames.end(); ++iter, ++namesIter) {
		robots.emplace_back(configMap, **iter, *namesIter);
	}
}

void Simulator::collectScores(string houseName, int simulationSteps, int winnerNumSteps) {
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

void Simulator::updateRobotListWithHouse(House& house) {
	logger.debug("Defining house [" + house.getName() + "] for robot list");
	for (Robot& robot : robots) {
		robot.restart();
		robot.setHouse(House(house));
	}
}

void Simulator::executeOnHouse(House& house, int maxSteps, int maxStepsAfterWinner) {
	
	int steps = 0;
	int winnerNumSteps = 0;
	int stepsAfterWinner = -1; // so when we increment for first time, when winner is found, it will be set to zero
	int positionInCompetition = 1;
	while (steps < maxSteps && stepsAfterWinner < maxStepsAfterWinner) {
		int robotsFinishedInRound = 0;
		for (Robot& robot : robots) {
			if (!robot.performedIllegalStep() && !robot.isFinished()) {
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
				if (robot.getHouse().getTotalDust() == 0 && robot.inDocking()) {
					robotFinishedCleaning(robot, steps, winnerNumSteps, positionInCompetition, robotsFinishedInRound);
				}
				else {
					// this is the actual position
					results[robot.getAlgorithmName()][house.getName()]
						.setPositionInCompetition(positionInCompetition);
				}
			}
		}

		steps++;
		if (winnerNumSteps > 0) {
			stepsAfterWinner++;
		}
		positionInCompetition = positionInCompetition + robotsFinishedInRound;
	}
	// fallback if there is no winner
	if (winnerNumSteps == 0) {
		winnerNumSteps = steps;
	}
	collectScores(house.getName(), steps, winnerNumSteps);
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
	robot.setFinished();
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
		errors.push_back("Algorithm " + robot.getAlgorithmName() + " when running on House " 
			+ houseName + " went on a wall in step " + to_string(steps + 1));
	}

	// perform one cleaning step
	if (robot.getHouse().clean(robot.getPosition())) {
		results[algorithmName][houseName].incrementDirtCollected();
	} 
	results[algorithmName][houseName].setThisNumSteps(steps + 1);
}
