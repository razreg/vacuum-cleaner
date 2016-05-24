#ifndef __SCORE__H_
#define __SCORE__H_

#include "Common.h"

using namespace std;

class Score {

	ScoreFormula calcScore = NULL;

	int positionInCompetition = 1;
	int simulationSteps = 0;
	int winnerNumSteps = 0;
	int thisNumSteps = 0;
	int sumDirtInHouse = 0;
	int isBackInDocking = 0;
	int dirtCollected = 0;

	bool badBehavior = false;

public:

	Score() {};

	Score(ScoreFormula scoreFormula) : calcScore(scoreFormula) {};

	~Score() {};

	Score(const Score& copyFromMe) : calcScore(copyFromMe.calcScore) {};

	Score(Score&& moveFromMe) {
		swap(this->calcScore, moveFromMe.calcScore);
	};

	Score& operator=(Score&& moveFromMe);

	Score& operator=(const Score& copyFromMe);

	void setPositionInCompetition(int positionInCompetition) {
		this->positionInCompetition = positionInCompetition;
	};

	void setWinnerNumSteps(int winnerNumSteps) {
		this->winnerNumSteps = winnerNumSteps;
	};

	void setSimulationSteps(int simulationSteps) {
		this->simulationSteps = simulationSteps;
	};

	void setThisNumSteps(int thisNumSteps) {
		this->thisNumSteps = thisNumSteps;
	};

	void setSumDirtInHouse(int finalSumDirtInHouse) {
		this->sumDirtInHouse = finalSumDirtInHouse;
	};

	void setIsBackInDocking(bool isBackInDocking) {
		this->isBackInDocking = isBackInDocking ? 1 : 0;
	};

	void incrementDirtCollected() {
		dirtCollected++;
	};

	void setDirtCollected(int dirtCollected) {
		this->dirtCollected = dirtCollected;
	};

	void reportBadBehavior() {
		badBehavior = true;
	};

	int getScore() const;

};

#endif // __SCORE__H_