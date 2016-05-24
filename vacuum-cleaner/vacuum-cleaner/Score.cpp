#include "Score.h"

int Score::getScore() const {
	int score;
	if (badBehavior) {
		return 0;
	}
	if (calcScore == NULL) {
		// default score function
		int pos = isBackInDocking && sumDirtInHouse == dirtCollected ?
			positionInCompetition : 10;
		score = max(0, 2000
			- (pos - 1) * 50
			+ (winnerNumSteps - thisNumSteps) * 10
			- (sumDirtInHouse - dirtCollected) * 3
			+ (isBackInDocking ? 50 : -200));
		/*
		cout << "====================================" << endl;
		cout << "isBackInDocking = " << isBackInDocking << endl;
		cout << "sumDirtInHouse = " << sumDirtInHouse << endl;
		cout << "dirtCollected = " << dirtCollected << endl;
		cout << "positionInCompetition = " << positionInCompetition << endl;
		cout << "winnerNumSteps = " << winnerNumSteps << endl;
		cout << "thisNumSteps = " << thisNumSteps << endl;
		cout << "====================================" << endl;
		*/
	}
	else {
		map<string, int> scoreParams = {
			{ "actual_position_in_competition", positionInCompetition },
			{ "simulation_steps", simulationSteps },
			{ "winner_num_steps", winnerNumSteps },
			{ "this_num_steps", thisNumSteps },
			{ "sum_dirt_in_house", sumDirtInHouse },
			{ "dirt_collected", dirtCollected },
			{ "is_back_in_docking", isBackInDocking }
		};
		try {
			score = (*calcScore)(scoreParams);
		}
		catch (exception& e) {
			score = -1;
		}
	}
	return score;
}

Score& Score::operator=(const Score& copyFromMe) {
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
}

Score& Score::operator=(Score&& moveFromMe) {
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
}