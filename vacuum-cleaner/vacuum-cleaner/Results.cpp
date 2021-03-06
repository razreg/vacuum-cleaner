#include "Results.h"

Logger Results::logger = Logger("Results");

Results::Results(list<string> algorithmNames, vector<string>&& houseNames, ScoreFormula scoreFormula) :
	algorithmNames(algorithmNames), houseNames(houseNames) {
	for (string& algorithm : this->algorithmNames) {
		for (string& house : this->houseNames) {
			scoreMap[algorithm][house] = Score(scoreFormula);
		}
	}
}

void Results::print(vector<string>& errors, ostream& out) const {
	
	if (houseNames.empty() || algorithmNames.empty()) {
		return;
	}
	map <double, string, std::greater<double>> scoreLines;
	bool scoreError = false;
	printHeader(out);

	// store scores in map
	for (string algorithm : algorithmNames) {
		stringstream stream;
		printAlgorithmCell(stream, algorithm);
		double avg = 0.0;
		for (string house : houseNames) {
			int score = (*this)[algorithm].at(house).getScore();
			if (score < 0) {
				scoreError = true;
			}
			avg += score;
			printScoreCell(stream, to_string(score));
		}
		avg /= houseNames.size();
		printScoreCell(stream, avg);
		if (scoreLines.find(avg) != scoreLines.end()) {
			stringstream separatorStream;
			separatorStream << endl;
			printHorizontalLine(separatorStream);
			scoreLines[avg] += separatorStream.str() + stream.str();
		}
		else {
			scoreLines[avg] = stream.str();
		}
	}

	if (scoreError) {
		errors.push_back("Score formula could not calculate some scores, see -1 in the results table");
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

void Results::removeHouse(string houseName) {
	if (logger.debugEnabled()) logger.debug("removing invalid house from results: " + houseName);
	for (string& algorithm : this->algorithmNames) {
		scoreMap[algorithm].erase(houseName);
	}
	vector<string>::iterator it;
	if ((it = find(houseNames.begin(), houseNames.end(), houseName)) != houseNames.end()) {
		houseNames.erase(it);
	}
}

const map<string, Score>& Results::operator [] (string str) const {
	if (scoreMap.find(str) != scoreMap.end()) {
		return scoreMap.at(str);
	}
	else {
		throw out_of_range("Algorithm name [" + str + "] not in results");
	}
}

map<string, Score>& Results::operator [] (string str) {
	if (scoreMap.find(str) != scoreMap.end()) {
		return scoreMap[str];
	}
	else {
		throw out_of_range("Algorithm name [" + str + "] not in results");
	}
}