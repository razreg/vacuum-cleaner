#include "Simulator.h"

using namespace std;

Logger Simulator::logger = Logger("Simulator");

void Simulator::execute() {

	int maxSteps = configMap.find(MAX_STEPS)->second;
	int maxStepsAfterWinner = configMap.find(MAX_STEPS_AFTER_WINNER)->second;

	int houseCount = 0;
	for (House* house : houseList) {
		logger.info("Simulation started for house number [" + to_string(houseCount) + "] - Name: " + house->getShortName());
		updateRobotListWithHouse(*house, houseCount);
		// run algorithms in "Round Robin" fashion
		executeOnHouse(house, maxSteps, maxStepsAfterWinner, houseCount);
		houseCount++;
	}
	printScoreMatrix();
}

void Simulator::initScoreMatrix() {
	scoreMatrix = new Score*[robots.size()];
	for (size_t i = 0; i < robots.size(); ++i) {
		scoreMatrix[i] = new Score[houseList.size()];
	}
}

void Simulator::initRobotList(list<AbstractAlgorithm*>& algorithms) {
	logger.info("Initializing robot list");
	for (AbstractAlgorithm* algorithm : algorithms) {
		string algoName = typeid(*algorithm).name();
		logger.info("Initializing robot with algorithm [" + algoName.substr(algoName.find_last_of(' ') + 1) + "]");
		Robot *robot = new Robot(configMap, *algorithm);
		robots.push_back(robot);
	}
}

void Simulator::collectScores(int houseCount, int winnerNumSteps) {
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

void Simulator::printScoreMatrix() {
	for (size_t i = 0; i < robots.size(); ++i) {
		for (size_t j = 0; j < houseList.size(); ++j) {
			cout << scoreMatrix[i][j].getScore();
			if (j < houseList.size() - 1) {
				cout << '\t';
			}
		}
		cout << endl;
	}
}

void Simulator::updateRobotListWithHouse(House& house, int houseCount) {
	logger.info("Defining house [" + to_string(houseCount) + "] for robot list");
	for (Robot* robot : robots) {
		House* currHouse = new House(house); // copy constructor called
		robot->restart();
		robot->setHouse(currHouse);
	}
}

// TODO extract methods
void Simulator::executeOnHouse(House* house, int maxSteps, int maxStepsAfterWinner, int houseCount) {
	
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
				else {
					performStep(*robot, steps, maxSteps, maxStepsAfterWinner, stepsAfterWinner,
						algorithmCount, houseCount);
				}

				// robot finished cleaning?
				if (robot->getHouse().getTotalDust() == 0 && robot->inDocking()) {
					robotFinishedCleaning(*robot, steps, winnerNumSteps, algorithmCount, 
						houseCount, positionInCompetition, robotsFinishedInRound);
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
	collectScores(houseCount, winnerNumSteps);
	logger.info("Simulation completed for house number [" + to_string(houseCount) + "] - Name: " + house->getShortName());
}

void Simulator::robotFinishedCleaning(Robot& robot, int steps, int& winnerNumSteps, 
	int algorithmCount, int houseCount, int positionInCompetition, int& robotsFinishedInRound) {

	logger.info("Algorithm [" + robot.getAlgorithmName() + "] has successfully cleaned the house and returned to docking station");
	// update the number of steps the winner has performed if no winner was found before
	if (winnerNumSteps == 0) {
		winnerNumSteps = steps + 1;
	}
	// update position in competition
	scoreMatrix[algorithmCount][houseCount]
		.setPositionInCompetition(positionInCompetition);
	robotsFinishedInRound++;
	robot.setFinished();
}

void Simulator::performStep(Robot& robot, int steps, int maxSteps, int maxStepsAfterWinner, 
	int stepsAfterWinner, int algorithmCount, int houseCount) {
	// notify on aboutToFinish if there is a winner or steps == maxSteps - maxStepsAfterWinner
	if (stepsAfterWinner == 0 || steps == maxSteps - maxStepsAfterWinner) {
		logger.debug("Notifying algorithm [" + robot.getAlgorithmName() + "] that the simulation is about to end");
		robot.aboutToFinish(maxStepsAfterWinner);
	}
	robot.step(); // this also updates the sensor and the battery but not the house
	if (!robot.getHouse().isInside(robot.getPosition()) ||
		robot.getHouse().isWall(robot.getPosition())) {
		logger.warn("Algorithm [" + robot.getAlgorithmName() +
			"] has performed an illegal step. Robot in position="
			+ (string)robot.getPosition());
		scoreMatrix[algorithmCount][houseCount].reportBadBehavior();
		robot.reportBadBehavior();
	}
	robot.getHouse().clean(robot.getPosition()); // perform one cleaning step
	scoreMatrix[algorithmCount][houseCount].setThisNumSteps(steps + 1);
}
