#include "score_formula.h"

int calc_score(const map<string, int>& score_params) {

	int positionInCompetition = -1;
	int simulationSteps = -1;
	int winnerNumSteps = -1;
	int thisNumSteps = -1;
	int sumDirtInHouse = -1;
	int dirtCollected = -1;
	int isBackInDocking = 0;

	for (auto it = score_params.cbegin(); it != score_params.cend(); it++) {
		if (it->first == "actual_position_in_competition") {
			positionInCompetition = it->second;
		}
		else if (it->first == "simulation_steps") {
			simulationSteps = it->second;
		}
		else if (it->first == "winner_num_steps") {
			winnerNumSteps = it->second;
		}
		else if (it->first == "this_num_steps") {
			thisNumSteps = it->second;
		}
		else if (it->first == "sum_dirt_in_house") {
			sumDirtInHouse = it->second;
		}
		else if (it->first == "dirt_collected") {
			dirtCollected = it->second;
		}
		else if (it->first == "is_back_in_docking") {
			isBackInDocking = it->second;
		}
	}

	if (positionInCompetition < 0 || simulationSteps < 0 || winnerNumSteps < 0 || 
		thisNumSteps < 0 || sumDirtInHouse < 0 || dirtCollected < 0) {
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