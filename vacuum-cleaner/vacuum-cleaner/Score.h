#ifndef __SCORE__H_
#define __SCORE__H_

#include <algorithm>

using namespace std;

const int DIDNT_FINISH_POSITION_IN_COMPETETION = 10;

class Score {

	int positionInCopmetition = 0;
	int winnerNumSteps = 0;
	int thisNumSteps = 0;
	int dirtCollected = 0;
	int sumDirtInHouse = 0;
	bool isBackInDocking = true;  // the robot starts at the docking station
	bool badBehavior = false;

public:

	void setPositionInCopmetition(int positionInCopmetition) {
		this->positionInCopmetition = positionInCopmetition;
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
		return (badBehavior) ? 0 : max(0, 2000 
			- (positionInCopmetition - 1) * 50 
			- (winnerNumSteps - thisNumSteps) * 10 
			- (sumDirtInHouse - dirtCollected) * 3 
			+ (isBackInDocking ? 50 : -200));
	};

};

#endif // __SCORE__H_