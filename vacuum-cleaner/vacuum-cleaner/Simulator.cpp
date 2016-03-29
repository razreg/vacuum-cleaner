#include "Simulator.h"

using namespace std;

Logger Simulator::logger = Logger("Simulator");

void Simulator::execute() {

	// basic score matrix - will be improved according to ex2 instructions
	Score** scoreMatrix = initScoreMatrix();

	int maxSteps = configMap.find(MAX_STEPS)->second;
	int maxStepsAfterWinner = configMap.find(MAX_STEPS_AFTER_WINNER)->second;

	// initialize robot list
	list<Robot*> robots;
	initRobotList(robots);

	int houseCount = 0;
	for (House* house : houseList) {
		logger.info("Simulation started for house number [" + to_string(houseCount) + "] - Name: " + house->getShortName());
		updateRobotListWithHouse(robots, *house, houseCount);
		// run algorithms in "Round Robin" fashion
		executeOnHouse(house, robots, scoreMatrix, maxSteps, maxStepsAfterWinner, houseCount);
		houseCount++;
	}
	printScoreMatrix(scoreMatrix);

	// cleaning up
	int i = 0;
	for (Robot* robot : robots) {
		delete robot;
		delete[] scoreMatrix[i++];
	}
	robots.clear();
	delete[] scoreMatrix;
}

Score** Simulator::initScoreMatrix() {
	Score** scoreMatrix = new Score*[algorithms.size()];
	for (size_t i = 0; i < algorithms.size(); ++i) {
		scoreMatrix[i] = new Score[houseList.size()];
	}
	return scoreMatrix;
}

void Simulator::initRobotList(list<Robot*>& robots) {
	logger.info("Initializing robot list");
	for (AbstractAlgorithm* algorithm : algorithms) {
		string algoName = typeid(*algorithm).name();
		logger.info("Initializing robot with algorithm [" + algoName.substr(algoName.find_last_of(' ') + 1) + "]");
		Robot *robot = new Robot(configMap, *algorithm);
		robots.push_back(robot);
	}
}

void Simulator::collectScores(Score** scoreMatrix, list<Robot*>& robots, int houseCount, int winnerNumSteps) {
	int algorithmCount = 0;
	for (Robot* robot : robots) {
		scoreMatrix[algorithmCount][houseCount].setIsBackInDocking(robot->inDocking());
		scoreMatrix[algorithmCount][houseCount].setWinnerNumSteps(winnerNumSteps);
		scoreMatrix[algorithmCount][houseCount].setFinalSumDirtInHouse(robot->getHouse().getTotalDust());
		if (robot->getHouse().getTotalDust() > 0 || !robot->inDocking()) {
			scoreMatrix[algorithmCount][houseCount].setPositionInCompetition(DIDNT_FINISH_POSITION_IN_COMPETETION);
		}
		logger.debug("House final state for algorithm [" + robot->getAlgorithmName() + "]:\n"
			+ (string)robot->getHouse());
		algorithmCount++;
		delete &robot->getHouse();
	}
}

void Simulator::printScoreMatrix(Score** scoreMatrix) {
	for (size_t i = 0; i < algorithms.size(); ++i) {
		for (size_t j = 0; j < houseList.size(); ++j) {
			cout << scoreMatrix[i][j].getScore();
			if (j < houseList.size() - 1) {
				cout << '\t';
			}
		}
		cout << endl;
	}
}

void Simulator::updateRobotListWithHouse(list<Robot*>& robots, House& house, int houseCount) {
	logger.info("Defining house [" + to_string(houseCount) + "] for robot list");
	for (Robot* robot : robots) {
		House* currHouse = new House(house); // copy constructor called
		robot->restart();
		robot->setHouse(currHouse);
	}
}

// TODO make scoreMatrix a member
void Simulator::executeOnHouse(House* house, list<Robot*>& robots, Score** scoreMatrix, int maxSteps, int maxStepsAfterWinner, int houseCount) {
	
	int steps = 0;
	int winnerNumSteps = 0;
	int stepsAfterWinner = -1; // so when we increment for first time, when winner is found, it will be set to zero
	int algorithmCount;
	int positionInCompetition = 1;
	int robotsFinishedInRound;
	while (steps < maxSteps && stepsAfterWinner < maxStepsAfterWinner) {
		algorithmCount = 0;
		robotsFinishedInRound = 0;
		for (Robot* robot : robots) {
			if (!robot->performedIllegalStep() && !robot->isFinished()) {
				if (robot->getBatteryValue() <= 0) {
					// if we didn't alreay notify that the battery died
					if (!robot->isBatteryDeadNotified()) {
						robot->setBatteryDeadNotified();
						logger.info("Robot using algorithm [" + robot->getAlgorithmName() + "] has dead battery");
					}
					scoreMatrix[algorithmCount][houseCount].setThisNumSteps(steps + 1); // increment steps but stay
				}
				else if (robot->getHouse().getTotalDust() > 0) {
					// notify on aboutToFinish if there is a winner or steps == maxSteps - maxStepsAfterWinner
					if (stepsAfterWinner == 0 || steps == maxSteps - maxStepsAfterWinner) {
						logger.debug("Notifying algorithm [" + robot->getAlgorithmName() + "] that the simulation is about to end");
						robot->aboutToFinish(maxStepsAfterWinner);
					}
					robot->step(); // this also updates the sensor and the battery but not the house
					if (!robot->getHouse().isInside(robot->getPosition()) ||
						robot->getHouse().isWall(robot->getPosition())) {
						logger.warn("Algorithm [" + robot->getAlgorithmName() +
							"] has performed an illegal step. Robot in position="
							+ (string)robot->getPosition());
						scoreMatrix[algorithmCount][houseCount].reportBadBehavior();
						robot->reportBadBehavior();
					}
					robot->getHouse().clean(robot->getPosition()); // perform one cleaning step
					scoreMatrix[algorithmCount][houseCount].setThisNumSteps(steps + 1);
				}

				// robot finished cleaning?
				if (robot->getHouse().getTotalDust() == 0 && robot->inDocking()) {
					logger.info("Algorithm [" + robot->getAlgorithmName() + "] has successfully cleaned the house and returned to docking station");
					// update the number of steps the winner has performed if no winner was found before
					if (winnerNumSteps == 0) {
						winnerNumSteps = steps + 1;
					}
					// update position in competition
					scoreMatrix[algorithmCount][houseCount]
						.setPositionInCompetition(positionInCompetition);
					robotsFinishedInRound++;
					robot->setFinished();
				}
			}
			algorithmCount++;
		}
		steps++;
		if (winnerNumSteps > 0) {
			stepsAfterWinner++;
		}
		positionInCompetition = min(4, positionInCompetition + robotsFinishedInRound);
	}
	// fallback if there is no winner
	if (winnerNumSteps == 0) {
		winnerNumSteps = steps;
	}
	collectScores(scoreMatrix, robots, houseCount, winnerNumSteps);
	logger.info("Simulation completed for house number [" + to_string(houseCount) + "] - Name: " + house->getShortName());
}
