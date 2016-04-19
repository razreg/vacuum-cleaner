#ifndef __RESULTS__H_
#define __RESULTS__H_

#include "Common.h"
#include "Score.h"

#include <iomanip>

using namespace std;

class Results {

	map<string, map<string, Score>> scoreMap;
	list<string> algorithmNames;
	vector<string> houseNames;

	void printHeaderCell(ostream& out, string str) const {
		out << left << setw(9) << setfill(' ') << str.substr(0, 9) << " |";
	};

	void printAlgorithmCell(ostream& out, string str) const {
		out << "|" << left << setw(13) << setfill(' ') << str.substr(0, 13) << "|";
	};

	void printScoreCell(ostream& out, string str) const {
		out << right << setw(10) << setfill(' ') << str.substr(0, 10) << "|";
	};

	void printScoreCell(ostream& out, double num) const {
		out << right << setw(10) << setfill(' ') << fixed << setprecision(2) << num << "|";
	};

	void printHorizontalLine(ostream& out) {
		int amount = 15 + (houseNames.size() + 1) * 11;
		out << left << setw(amount) << setfill('-') << "" << endl;
	};

public:

	Results(list<string> algorithmNames, vector<string>&& houseNames);

	map<string, Score>& operator [] (string str) {
		if (scoreMap.find(str) != scoreMap.end()) {
			return scoreMap[str];
		}
		else {
			throw out_of_range("Algorithm name [" + str + "] not in results");
		}
		
	}

	void print(ostream& out = cout);

};

#endif // __RESULTS__H_