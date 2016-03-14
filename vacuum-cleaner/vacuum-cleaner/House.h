#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

const char WALL = 'W';
const char DOCK = 'D';

struct Position {

	int X;
	int Y;

	Position(int xPos, int yPos) : X(xPos), Y(yPos) {};

};

class House {

	string shortName;
	string description;
	int numRows;
	int numCols;
	vector<vector<char>>* matrix;
	Position dockingStation = {0, 0};

public:

	House() : matrix(nullptr) {}

	House(string houseShortName, string houseDescription, int numRows_, int numCols_, 
		vector<vector<char>>& houseMatrix) : shortName(houseShortName), description(houseDescription), 
		numRows(numRows_), numCols(numCols_), matrix(&houseMatrix) {}

	~House() {
		if (matrix != nullptr) {
			delete[] matrix;
		}
		delete &dockingStation;
	}

	string getShortName(){
		return this->shortName;
	}

	string getDescription(){
		return this->description;
	}

	int getNumRows(){
		return this->numRows;
	}

	int getnumCols(){
		return this->numCols;
	}

	vector<vector<char>>* getMatrix(){
		return this->matrix;
	}

	// dust level at given position - 0 if dust level is undefined (wall, docking station, etc.)
	int getDirtLevel(Position& position) const {
		return getDirtLevel(position.X, position.Y);
	}

	int getDirtLevel(int x, int y) const {
		if ((*matrix)[x][y] < '0' || (*matrix)[x][y] > '9') {
			return 0;
		}
		return (*matrix)[x][y] - '0';
	}

	bool isWall(Position& position) const {
		// TODO handle illegal position (outside of the house)
		return toupper((*matrix)[position.X][position.Y]) == WALL;
	}

	Position getDockingStation();
	
	void validateWalls() const;

	int getTotalDust() const;

	static House& deseriallize(const string& filePath);
};