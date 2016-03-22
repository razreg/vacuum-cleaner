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

	Position(int xPos = 0, int yPos = 0) : X(xPos), Y(yPos) {};

	bool operator==(const Position &other) const {
		return this->X == other.X && this->Y == other.Y;
	};

	bool operator!=(const Position &other) const {
		return !(*this == other);
	};

	operator string() const {
		return "(" + to_string(X) + ", " + to_string(Y) + ")";
	};
};

class House {

	static Logger logger;

	string shortName;
	string description;
	int numRows;
	int numCols;
	char** matrix;
	Position dockingStation;
	int totalDust = -1;

public:

	House() : matrix(nullptr) {};

	House(string houseShortName, string houseDescription, int numRows_, int numCols_,
		char** houseMatrix) : shortName(houseShortName), description(houseDescription),
		numRows(numRows_), numCols(numCols_), matrix(houseMatrix) {};

	House(const House& copyFromMe) : shortName(copyFromMe.shortName), description(copyFromMe.description),
		numRows(copyFromMe.numRows), numCols(copyFromMe.numCols), dockingStation(copyFromMe.dockingStation) {
		matrix = new char*[numRows];
		for (int i = 0; i < numRows; ++i) {
			matrix[i] = new char[numCols];
			for (int j = 0; j < numCols; ++j) {
				matrix[i][j] = copyFromMe.matrix[i][j];
			}
		}
	};

	~House() {
		if (matrix != nullptr) {
			for (int i = 0; i < numRows; ++i) {
				delete [] matrix[i];
			}
			delete [] matrix;
		}
	};
	
	operator string() const;

	string getShortName(){
		return this->shortName;
	};

	string getDescription(){
		return this->description;
	};

	int getNumRows(){
		return this->numRows;
	};

	int getnumCols(){
		return this->numCols;
	};

	char** getMatrix(){
		return this->matrix;
	};

	// dust level at given position - 0 if dust level is undefined (wall, docking station, etc.)
	int getDirtLevel(const Position& position) const {
		return getDirtLevel(position.X, position.Y);
	};

	int getDirtLevel(int x, int y) const {
		if (matrix[y][x] < '0' || matrix[y][x] > '9') {
			return 0;
		}
		return matrix[y][x] - '0';
	};

	bool isWall(const Position& position) const {
		if (isInside(position)) {
			return toupper(matrix[position.Y][position.X]) == WALL;
		}
		return false;
	};

	// true iff position inside house boundaries
	bool isInside(const Position& position) const {
		return position.X >= 0 && position.X < numCols && position.Y >= 0 && position.Y < numRows;
	};

	Position getDockingStation();
	
	void validateWalls();

	// true iff dust was vacuumed
	bool clean(const Position& position) {
		char* cell = &matrix[position.Y][position.X];
		if (*cell > '0' && *cell <= '9') {
			(*cell)--;
			totalDust--;
			if (*cell == '0') {
				*cell = ' ';
			}
			return true;
		}
		return false;
	};

	int getTotalDust();

	static House& deseriallize(const string& filePath);
};

#endif // __HOUSE__H_