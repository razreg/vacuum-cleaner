#ifndef __RESULTS__H_
#define __RESULTS__H_

#include "Common.h"
#include "Score.h"

#include <iomanip>

using namespace std;

class Results {

	map<string, map<string, Score>> scoreMap;
	vector<string> algorithmNames;
	vector<string> houseNames;

	void printHeaderCell(ostream& out, string& str) const {
		out << left << setw(9) << setfill(' ') << str << " |";
	}

	void printAlgorithmCell(ostream& out, string& str) const {
		out << "|" << left << setw(13) << setfill(' ') << str << "|";
	}

	void printScoreCell(ostream& out, string& str) const {
		out << right << setw(10) << setfill(' ') << str << "|";
	}

	void printScoreCell(ostream& out, double num) const {
		out << right << setw(10) << setfill(' ') << setprecision(2) << num << "|";
	}

public:

	Results(vector<string>& algorithmNames, vector<string>& houseNames);

	void print(ostream& out);

};

#endif // __RESULTS__H_