#pragma once
#include "Position.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

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

	int getDirtLevel(Position& position) const {
		char value = (*matrix)[position.X][position.Y];
		if (value == ' '){ 
			//space is equal to 0
			return 0;
		}
		if (value < '0' || value > '9') {
			// TODO handle illegal position (wall, docking, outside the house, etc.) - throw exception?
		}
		return value - '0';
	}

	bool isWall(Position& position) const {
		// TODO handle illegal position (outside of the house)
		return toupper((*matrix)[position.X][position.Y]) == WALL;
	}

	Position getDockingStation(){
		int i, j;
		Position pos = { 0, 0 };
		for (i = 0; i < numRows; i++){
			for (j = 0; j < numCols; j++){
				if ((*matrix)[i][j] == 'D'){
					pos = { i, j };
					return pos;
				}
					
			}
		}

		//TODO return error message? exit?
	}

	static House& deseriallize(const string& filePath);
};