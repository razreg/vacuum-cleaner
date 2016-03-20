#ifndef __HOUSE__H_
#define __HOUSE__H_

#include <vector>
#include <fstream>
#include <stdexcept>
#include "Common.h"

using namespace std;

const char WALL = 'W';
const char DOCK = 'D';

struct Position {
	int X;
	int Y;

	Position(int xPos, int yPos) : X(xPos), Y(yPos) {};

	bool operator==(const Position &other) const {
		return this->X == other.X && this->Y == other.Y;
	};
	bool operator!=(const Position &other) const {
		return !(*this == other);
	};
};

//Logger logger("House");

class House {

	
	string shortName;
	string description;
	int numRows;
	int numCols;
	char** matrix;
	Position dockingStation = {0, 0};
	int totalDust = -1;

	/*for hard-coded*
	string shortName;
	string description;
	const int numRows = 19;
	const int numCols = 80;
	char matrix[19][80 + 1] = {
		//             1         2         3         4         5         6         7        
		//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		"W  99   D              1234321                                                 W", // 1
		"W  99      WWWWWWW     1234321                     W                       1   W", // 2
		"W              W                                   W   555                 2   W", // 3
		"W              W                                   W   555                 3   W", // 4
		"W              W           WWWWWWWWWWWWWWWWWWWWWWWWW                       4   W", // 5
		"W              W                                                           5   W", // 6
		"W              W                                                           6   W", // 7
		"W                          WWWWWWWWWWWWWWWWWWWWWW  WWWWWWW                 7   W", // 8
		"W         1         2         3         4         5W 999 W  6         7        W", // 9
		"W              W           444                     W 999 W                 9   W", // 10
		"W              W           444                     W 999 W                 8   W", // 11
		"W              W                                   W     W                 7   W", // 12
		"W              W                                   WW   WW                 6   W", // 13
		"W              W                                    W   W                  5   W", // 14
		"W              W                                                           4   W", // 15
		"W              W                                                           3   W", // 16
		"W              W                                                               W", // 17
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW" // 18
	};
	**********/


public:

	/*for hard-coded*

	House(string houseShortName, string houseDescription) : shortName(houseShortName), description(houseDescription) {}

	House(const House& copyFromMe) : shortName(copyFromMe.shortName), description(copyFromMe.description),
		numRows(copyFromMe.numRows), numCols(copyFromMe.numCols) {
		char matrix[19][81];
		for (int i = 0; i < numRows; ++i) {
			for (int j = 0; j < numCols; ++j) {
				matrix[i][j] = copyFromMe.matrix[i][j];
			}
		}
	}
	**********/

	
	House() : matrix(nullptr) {}

	House(string houseShortName, string houseDescription, int numRows_, int numCols_, 
		char** houseMatrix) : shortName(houseShortName), description(houseDescription), 
		numRows(numRows_), numCols(numCols_), matrix(houseMatrix) {}


	House(const House& copyFromMe) : shortName(copyFromMe.shortName), description(copyFromMe.description),
		numRows(copyFromMe.numRows), numCols(copyFromMe.numCols) {
		matrix = new char*[numRows];
		for (int i = 0; i < numRows; ++i) {
			matrix[i] = new char[numCols];
			for (int j = 0; j < numCols; ++j) {
				matrix[i][j] = copyFromMe.matrix[i][j];
			}
		}
	}

	~House() {
		if (matrix != nullptr) {
			for (int i = 0; i < numRows; ++i) {
				delete [] matrix[i];
			}
			delete [] matrix;
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

	
	char** getMatrix(){
		return this->matrix;
	}
	

	// dust level at given position - 0 if dust level is undefined (wall, docking station, etc.)
	int getDirtLevel(const Position& position) const {
		return getDirtLevel(position.X, position.Y);
	}

	int getDirtLevel(int x, int y) const {
		if (matrix[x][y] < '0' || matrix[x][y] > '9') {
			return 0;
		}
		return matrix[x][y] - '0';
	}

	bool isWall(const Position& position) const {
		if (isInside(position)) {
			return toupper(matrix[position.X][position.Y]) == WALL;
		}
		return false;
	}

	// true iff position inside house boundaries
	bool isInside(const Position& position) const {
		return position.X >= 0 && position.X < numCols && position.Y >= 0 && position.Y < numRows;
	}

	Position getDockingStation();
	
	void validateWalls();

	// true iff dust was vacuumed
	bool clean(const Position& position) {
		char* cell = &matrix[position.X][position.Y];
		if (*cell > '0' && *cell <= '9') {
			(*cell)--;
			return true;
		}
		return false;
	};

	int getTotalDust();

	static House& deseriallize(const string& filePath);
};

#endif // __HOUSE__H_