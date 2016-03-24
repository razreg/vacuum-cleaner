#ifndef __SCORE__H_
#define __SCORE__H_

#include "Common.h"

using namespace std;

const int DIDNT_FINISH_POSITION_IN_COMPETETION = 10;

class Score {

	static Logger logger;

	int positionInCompetition = 0;
	int winnerNumSteps = 0;
	int thisNumSteps = 0;
	int dirtCollected = 0;
	int sumDirtInHouse = 0;
	bool isBackInDocking = false;
	bool badBehavior = false;

public:

	void setPositionInCompetition(int positionInCompetition) {
		this->positionInCompetition = positionInCompetition;
	};

	void setWinnerNumSteps(int winnerNumSteps) {
		this->winnerNumSteps = winnerNumSteps;
	};

	void setThisNumSteps(int thisNumSteps) {
		this->thisNumSteps = thisNumSteps;
	};

	void setDirtCollected(int dirtCollected) {
		this->dirtCollected = dirtCollected;
	};

	void setSumDirtInHouse(int sumDirtInHouse) {
		this->sumDirtInHouse = sumDirtInHouse;
	};

	void setIsBackInDocking(bool isBackInDocking) {
		this->isBackInDocking = isBackInDocking;
	};

	int incrementThisNumSteps() {
		return ++(this->thisNumSteps);
	};

	int incrementDirtCollected() {
		return ++(this->dirtCollected);
	};

	void reportBadBehavior() {
		badBehavior = true;
	};

	int getScore() {
		int score = (badBehavior) ? 0 : max(0, 2000 
			- (positionInCompetition - 1) * 50 
			+ (winnerNumSteps - thisNumSteps) * 10 
			- (sumDirtInHouse - dirtCollected) * 3 
			+ (isBackInDocking ? 50 : -200));
		logger.debug(to_string(score) + " = ("
			+ to_string(badBehavior) + ") ? 0 : max(0, 2000 - ("
			+ to_string(positionInCompetition) + " - 1) * 50 + ("
			+ to_string(winnerNumSteps) + " - "
			+ to_string(thisNumSteps) + ") * 10 - ("
			+ to_string(sumDirtInHouse) + " - "
			+ to_string(dirtCollected) + ") * 3 + ("
			+ to_string(isBackInDocking) + " ? 50 : -200))");
		return score;
	};

};

#endif // __SCORE__H_