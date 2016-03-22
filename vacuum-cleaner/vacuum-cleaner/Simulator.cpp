#include "Simulator.h"

using namespace std;

Logger Simulator::logger = Logger("Simulator");

void Simulator::execute() {

	// basic score matrix - will be improved according to ex2 instructions
	Score** scoreMatrix = new Score*[algorithms.size()];
	for (int i = 0; i < algorithms.size(); ++i) {
		scoreMatrix[i] = new Score[houseList.size()];
	}

	int maxSteps = configMap.find(MAX_STEPS)->second;
	int maxStepsAfterWinner = configMap.find(MAX_STEPS_AFTER_WINNER)->second;

	// initialize robot list
	logger.info("Initializing robot list");
	list<Robot*> robots;
	for (AbstractAlgorithm* algorithm : algorithms) {
		string algoName = typeid(*algorithm).name();
		logger.info("Initializing robot with algorithm [" + algoName.substr(algoName.find_last_of(' ') + 1) + "]");
		Robot *robot = new Robot(configMap, *algorithm);
		robots.push_back(robot);
	}

	int houseCount = 0;
	for (House* house : houseList) {

		logger.info("Simulation started for house number [" + to_string(houseCount) + "] - Name: " + house->getShortName());

		// update robot list
		logger.info("Defining house [" + to_string(houseCount) + "] for robot list");
		for (Robot* robot : robots) {
			House* currHouse = new House(*house); // copy constructor called
			robot->restart();
			robot->setHouse(currHouse);
		}

		// run algorithms in "Round Robin" fashion
		int steps = 0;
		int winnerNumSteps = 0;
		int stepsAfterWinner = -1; // so when we increment for first time, when winner is found, it will be set to zero
		int algorithmCount;
		int positionInCompetition = 1;
		bool someoneFinishedInRound = false;
		while (steps < maxSteps && stepsAfterWinner < maxStepsAfterWinner) {
			algorithmCount = 0;
			for (Robot* robot : robots) {
				if (!robot->performedIllegalStep()) {
					if (robot->getBatteryValue() == 0) {
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
						// perform one cleaning step and update score
						if (robot->getHouse().clean(robot->getPosition())) {
							scoreMatrix[algorithmCount][houseCount].incrementDirtCollected();
						}
						// robot finished cleaning
						if (robot->getHouse().getTotalDust() == 0) {
							logger.info("House is clean of dust for algorithm [" + robot->getAlgorithmName() + "]");
							// update the number of steps the winner has performed if no winner was found before
							if (winnerNumSteps == 0) {
								winnerNumSteps = steps + 1;
							}
							// update position in competition
							scoreMatrix[algorithmCount][houseCount]
								.setPositionInCompetition(positionInCompetition);
							someoneFinishedInRound = true;
						}
						scoreMatrix[algorithmCount][houseCount].setThisNumSteps(steps + 1);
					}
				}
				algorithmCount++;
			}
			steps++;
			if (winnerNumSteps > 0) {
				stepsAfterWinner++;
			}
			if (someoneFinishedInRound && positionInCompetition < 4) {
				positionInCompetition++;
				someoneFinishedInRound = false;
			}
		}
		// fallback if there is no winner
		if (winnerNumSteps == 0) {
			winnerNumSteps = steps;
		}
		
		// collect scores
		algorithmCount = 0;
		for (Robot* robot : robots) {
			scoreMatrix[algorithmCount][houseCount].setIsBackInDocking(robot->inDocking());
			scoreMatrix[algorithmCount][houseCount].setWinnerNumSteps(winnerNumSteps);
			scoreMatrix[algorithmCount][houseCount].setSumDirtInHouse(robot->getHouse().getTotalDust());
			if (robot->getHouse().getTotalDust() > 0) {
				scoreMatrix[algorithmCount][houseCount].setPositionInCompetition(DIDNT_FINISH_POSITION_IN_COMPETETION);
			}
			logger.debug("House final state for algorithm [" + robot->getAlgorithmName() + "]:\n"
				+ (string)robot->getHouse());
			algorithmCount++;
			delete &robot->getHouse();
		}
		logger.info("Simulation completed for house number [" + to_string(houseCount) + "] - Name: " + house->getShortName());
		houseCount++;
	}

	// printing scoreMatrix
	for (int i = 0; i < algorithms.size(); ++i) {
		for (int j = 0; j < houseList.size(); ++j) {
			cout << scoreMatrix[i][j].getScore();
			if (j < houseList.size() - 1) {
				cout << '\t';
			}
		}
		cout << endl;
	}

	// cleaning up
	int i = 0;
	for (Robot* robot : robots) {
		delete robot;
		delete[] scoreMatrix[i++];
	}
	robots.clear();
	delete[] scoreMatrix;
}
