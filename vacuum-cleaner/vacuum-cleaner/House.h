#ifndef __HOUSE__H_
#define __HOUSE__H_

#include "Position.h"
#include "Common.h"

using namespace std;
namespace fs = boost::filesystem;

const char WALL = 'W';
const char DOCK = 'D';

class House {

	static Logger logger;

	string filename;
	string name;
	size_t maxSteps;
	size_t numRows;
	size_t numCols;
	vector<vector<char>> matrix;
	Position dockingStation;
	int totalDust = -1;

	static void readHouseMatrix(ifstream& houseFileStream, vector<vector<char>>& matrix, 
		size_t nRows, size_t nCols);

public:

	House() {};

	House(string houseName, size_t maxSteps, size_t numRows, size_t numCols,
		vector<vector<char>>& houseMatrix) : name(houseName), maxSteps(maxSteps),
		numRows(numRows), numCols(numCols), matrix(houseMatrix) {};

	// TODO move implementation to House.cpp
	House(const House& copyFromMe) : name(copyFromMe.name), maxSteps(copyFromMe.maxSteps),
		numRows(copyFromMe.numRows), numCols(copyFromMe.numCols), dockingStation(copyFromMe.dockingStation) {
		for (size_t i = 0; i < numRows; ++i) {
			vector<char> row;
			for (size_t j = 0; j < numCols; ++j) {
				row.push_back(copyFromMe.matrix[i][j]);
			}
			matrix.push_back(row);
		}
	};

	House(House&& moveFromMe) noexcept : name(move(moveFromMe.name)),
		maxSteps(moveFromMe.maxSteps), numRows(moveFromMe.numRows), numCols(moveFromMe.numCols), 
		matrix(move(moveFromMe.matrix)), dockingStation(move(moveFromMe.dockingStation)) {};

	House& operator=(const House& copyFromMe) {
		if (this != &copyFromMe) {
			name = copyFromMe.name;
			maxSteps = copyFromMe.maxSteps;
			numRows = copyFromMe.numRows;
			numCols = copyFromMe.numCols;
			dockingStation = copyFromMe.dockingStation;
			matrix = copyFromMe.matrix;
		}
		return *this;
	};

	~House() {};
	
	operator string() const;

	string getName() const {
		return this->name;
	};

	size_t getMaxSteps() const {
		return this->maxSteps;
	};

	int getNumRows() const {
		return this->numRows;
	};

	int getnumCols() const {
		return this->numCols;
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

	static House deseriallize(fs::path filePath);
};

#endif // __HOUSE__H_
