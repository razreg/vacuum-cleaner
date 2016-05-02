#include "Results.h"

Results::Results(list<string> algorithmNames, vector<string>&& houseNames) : 
	algorithmNames(algorithmNames), houseNames(houseNames) {
	for (string algorithm : this->algorithmNames) {
		for (string house : this->houseNames) {
			scoreMap[algorithm][house] = Score();
		}
	}
}

void Results::print(ostream& out) const {
	
	if (houseNames.empty() || algorithmNames.empty()) {
		return;
	}
	map <double, string, std::greater<double>> scoreLines;
	printHeader(out);

	// store scores in map
	for (string algorithm : algorithmNames) {
		stringstream stream;
		printAlgorithmCell(stream, algorithm);
		double avg = 0.0;
		for (string house : houseNames) {
			int score = (*this)[algorithm].at(house).getScore();
			avg += score;
			printScoreCell(stream, to_string(score));
		}
		avg /= houseNames.size();
		printScoreCell(stream, avg);
		scoreLines[avg] = stream.str();
	}

	// print map order by average score desc
	for (auto const& score : scoreLines) {
		out << score.second << endl;
		printHorizontalLine(out);
	}
}

void Results::printHeader(ostream& out) const {
	printHorizontalLine(out);
	printAlgorithmCell(out, string(" "));
	for (string house : houseNames) {
		printHeaderCell(out, house);
	}
	printHeaderCell(out, string("AVG"));
	out << endl;
	printHorizontalLine(out);
}