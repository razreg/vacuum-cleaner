#ifndef __RESULTS__H_
#define __RESULTS__H_

#include "Common.h"
#include "Score.h"

#include <iomanip>
#include <sstream>

using namespace std;

class Results {

	static Logger logger;

	ScoreFormula scoreFormula;
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

	void printHorizontalLine(ostream& out) const {
		int amount = 15 + (houseNames.size() + 1) * 11;
		out << left << setw(amount) << setfill('-') << "" << endl;
	};

	void printHeader(ostream& out) const;

public:

	Results() {};

	Results(list<string> algorithmNames, vector<string>&& houseNames, ScoreFormula scoreFormula);

	map<string, Score>& operator [] (string str);

	const map<string, Score>& operator [] (string str) const;

	void print(vector<string>& errors, ostream& out = cout) const;

	void removeHouse(string houseName);

	bool areAllHousesInvalid() const {
		return houseNames.empty();
	};

};

#endif // __RESULTS__H_