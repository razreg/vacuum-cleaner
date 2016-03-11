#pragma once
#include "Position.h"
#include <string>
#include <vector>

using namespace std;

const char WALL = 'W';

class House {

	string shortName;
	string description;
	int numRows;
	int numCols;
	vector<vector<char>>* matrix;

public:

	House() : matrix(nullptr) {}

	House(string houseShortName, string houseDescription, int numRows_, int numCols_, 
		vector<vector<char>>& houseMatrix) : shortName(houseShortName), description(houseDescription), 
		numRows(numRows_), numCols(numCols_), matrix(&houseMatrix) {}

	~House() {
		if (matrix != nullptr) {
			delete[] matrix;
		}
	}

	int getDirtLevel(Position& position) const {
		char value = (*matrix)[position.X][position.Y];
		if (value < '0' || value > '9') {
			// TODO handle illegal position (wall, docking, outside the house, etc.) - throw exception?
		}
		return value - '0';
	}

	bool isWall(Position& position) const {
		// TODO handle illegal position (outside of the house)
		return toupper((*matrix)[position.X][position.Y]) == WALL;
	}

	static House& deseriallize(const string& filePath);

};