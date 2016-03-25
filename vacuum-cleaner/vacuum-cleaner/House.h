#ifndef __HOUSE__H_
#define __HOUSE__H_

#include "Position.h"
#include "Common.h"

using namespace std;

const char WALL = 'W';
const char DOCK = 'D';

class House {

	static Logger logger;

	string shortName;
	string description;
	size_t numRows;
	size_t numCols;
	char** matrix;
	Position dockingStation;
	int totalDust = -1;

public:

	House() : matrix(nullptr) {};

	House(string houseShortName, string houseDescription, size_t numRows_, size_t numCols_,
		char** houseMatrix) : shortName(houseShortName), description(houseDescription),
		numRows(numRows_), numCols(numCols_), matrix(houseMatrix) {};

	House(const House& copyFromMe) : shortName(copyFromMe.shortName), description(copyFromMe.description),
		numRows(copyFromMe.numRows), numCols(copyFromMe.numCols), dockingStation(copyFromMe.dockingStation) {
		matrix = new char*[numRows];
		for (size_t i = 0; i < numRows; ++i) {
			matrix[i] = new char[numCols];
			for (size_t j = 0; j < numCols; ++j) {
				matrix[i][j] = copyFromMe.matrix[i][j];
			}
		}
	};

	~House() {
		if (matrix != nullptr) {
			for (size_t i = 0; i < numRows; ++i) {
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

	// dust level at given position. 0 if dust level is undefined (wall, docking station, etc.)
	int getDirtLevel(const Position& position) const {
		return getDirtLevel(position.getX(), position.getY());
	};

	int getDirtLevel(size_t x, size_t y) const {
		if (matrix[y][x] < '0' || matrix[y][x] > '9') {
			return 0;
		}
		return matrix[y][x] - '0';
	};

	bool isWall(const Position& position) const {
		if (isInside(position)) {
			return toupper(matrix[position.getY()][position.getX()]) == WALL;
		}
		return false;
	};

	// true iff position inside house boundaries
	bool isInside(const Position& position) const {
		return position.getX() >= 0 && position.getX() < numCols && 
			position.getY() >= 0 && position.getY() < numRows;
	};

	Position getDockingStation();
	
	void validateWalls();

	void validateDocking();

	// true iff dust was vacuumed
	bool clean(const Position& position) {
		char* cell = &matrix[position.getY()][position.getX()];
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