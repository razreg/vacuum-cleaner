#include "Results.h"

Results::Results(vector<string>& algorithmNames, vector<string>& houseNames) : 
	algorithmNames(algorithmNames), houseNames(houseNames) {
	for (string algorithm : this->algorithmNames) {
		for (string house : this->houseNames) {
			scoreMap[algorithm][house] = Score();
		}
	}
}

void Results::print(ostream& out) {

	printAlgorithmCell(out, string(" "));
	for (string house : houseNames) {
		printHeaderCell(out, house);
	}
	printHeaderCell(out, string("AVG"));
	out << endl;
	for (string algorithm : algorithmNames) {
		printAlgorithmCell(out, algorithm);
		double avg;
		for (string house : houseNames) {
			int score = scoreMap[algorithm][house].getScore();
			printScoreCell(out, to_string(score));
		}
		printScoreCell(out, avg);
		out << endl;
	}
}