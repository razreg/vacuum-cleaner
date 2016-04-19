#ifndef __SCORE__H_
#define __SCORE__H_

using namespace std;

const int DIDNT_FINISH_POSITION_IN_COMPETETION = 10;

class Score {

	int positionInCompetition = 0;
	int winnerNumSteps = 0;
	int thisNumSteps = 0;
	int finalSumDirtInHouse = 0;
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

	void setFinalSumDirtInHouse(int finalSumDirtInHouse) {
		this->finalSumDirtInHouse = finalSumDirtInHouse;
	};

	void setIsBackInDocking(bool isBackInDocking) {
		this->isBackInDocking = isBackInDocking;
	};

	int incrementThisNumSteps() {
		return ++(this->thisNumSteps);
	};

	void reportBadBehavior() {
		badBehavior = true;
	};

	int getScore() const {
		int score = (badBehavior) ? 0 : max(0, 2000 
			- (positionInCompetition - 1) * 50 
			+ (winnerNumSteps - thisNumSteps) * 10 
			- finalSumDirtInHouse * 3 
			+ (isBackInDocking ? 50 : -200));
		return score;
	};

};

#endif // __SCORE__H_