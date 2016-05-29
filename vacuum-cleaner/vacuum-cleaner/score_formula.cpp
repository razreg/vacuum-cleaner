#include "score_formula.h"

int calc_score(const map<string, int>& score_params) {

	bool valid = true;
	int positionInCompetition = getValue(score_params, "actual_position_in_competition", valid);
	//int simulationSteps = getValue(score_params, "simulation_steps", valid);
	int winnerNumSteps = getValue(score_params, "winner_num_steps", valid);
	int thisNumSteps = getValue(score_params, "this_num_steps", valid);
	int sumDirtInHouse = getValue(score_params, "sum_dirt_in_house", valid);
	int isBackInDocking = getValue(score_params, "is_back_in_docking", valid);
	int dirtCollected = getValue(score_params, "dirt_collected", valid);

	if (!valid) {
		return -1;
	}

	int pos = isBackInDocking && sumDirtInHouse == dirtCollected ? positionInCompetition : 10;
	int score = 2000
		- (pos - 1) * 50
		+ (winnerNumSteps - thisNumSteps) * 10
		- (sumDirtInHouse - dirtCollected) * 3
		+ (isBackInDocking ? 50 : -200);
	return score < 0 ? 0 : score;
}

int getValue(const map<string, int>& score_params, string paramName, bool& valid) {
	map<string, int>::const_iterator itr;
	if (valid && (valid = (itr = score_params.find(paramName)) != score_params.end())) {
		return itr->second;
	}
	else {
		return -1;
	}
}