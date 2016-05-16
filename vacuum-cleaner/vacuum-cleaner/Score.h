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

	Score& operator=(Score&& moveFromMe) {
		if (this != &moveFromMe) {
			swap(this->calcScore, moveFromMe.calcScore);
			this->positionInCompetition = moveFromMe.positionInCompetition;
			this->simulationSteps = moveFromMe.simulationSteps;
			this->winnerNumSteps = moveFromMe.winnerNumSteps;
			this->thisNumSteps = moveFromMe.thisNumSteps;
			this->sumDirtInHouse = moveFromMe.sumDirtInHouse;
			this->isBackInDocking = moveFromMe.isBackInDocking;
			this->dirtCollected = moveFromMe.dirtCollected;
			this->badBehavior = moveFromMe.badBehavior;
		}
		return *this;
	};

	Score& operator=(const Score& copyFromMe) {
		if (this != &copyFromMe) {
			this->calcScore = copyFromMe.calcScore;
			this->positionInCompetition = copyFromMe.positionInCompetition;
			this->simulationSteps = copyFromMe.simulationSteps;
			this->winnerNumSteps = copyFromMe.winnerNumSteps;
			this->thisNumSteps = copyFromMe.thisNumSteps;
			this->sumDirtInHouse = copyFromMe.sumDirtInHouse;
			this->isBackInDocking = copyFromMe.isBackInDocking;
			this->dirtCollected = copyFromMe.dirtCollected;
			this->badBehavior = copyFromMe.badBehavior;
		}
		return *this;
	};

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

	void reportBadBehavior() {
		badBehavior = true;
	};

	int getScore() const {
		int score;
		if (badBehavior) {
			return 0;
		}
		if (calcScore == NULL) {
			// default score function
			int pos = isBackInDocking && sumDirtInHouse == 0 ? 
				positionInCompetition : 10;
			score = max(0, 2000
				- (pos - 1) * 50
				+ (winnerNumSteps - thisNumSteps) * 10
				- sumDirtInHouse * 3
				+ (isBackInDocking ? 50 : -200));
			/*
			std::cout << endl << "Score: " << "(" << badBehavior << ") ? 0 : max(0, 2000 - ("
				<< (isBackInDocking && sumDirtInHouse == 0 ? positionInCompetition : 10)
				<< " - 1) * 50 + (" << winnerNumSteps << " - "
				<< thisNumSteps << ") * 10 - " << sumDirtInHouse << " * 3 + ("
				<< isBackInDocking << " ? 50 : -200)) == " << score << endl << endl;
				*/
		}
		else {
			map<string, int> scoreParams = {
				{"actual_position_in_competition", positionInCompetition},
				{"simulation_steps", simulationSteps},
				{"winner_num_steps", winnerNumSteps},
				{"this_num_steps", thisNumSteps},
				{"sum_dirt_in_house", sumDirtInHouse}, // TODO sum_dirt_in_house should be the initial dust amount!!!
				{"dirt_collected", dirtCollected},
				{"is_back_in_docking", isBackInDocking}
			};
			try {
				score = (*calcScore)(scoreParams);
			}
			catch (exception& e) {
				score = -1;
			}
		}
		return score;
	};

};

#endif // __SCORE__H_