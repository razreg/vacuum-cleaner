#ifndef __HOUSE__H_
#define __HOUSE__H_

#include <sstream>

#include "Position.h"
#include "Common.h"

using namespace std;
namespace fs = boost::filesystem;

const char WALL = 'W';
const char DOCK = 'D';

class House {

	static Logger logger;

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
		numRows(numRows), numCols(numCols), matrix(houseMatrix) {
	};

	House(House&& moveFromMe) noexcept : name(move(moveFromMe.name)),
		maxSteps(moveFromMe.maxSteps), numRows(moveFromMe.numRows), numCols(moveFromMe.numCols),
		matrix(move(moveFromMe.matrix)), dockingStation(move(moveFromMe.dockingStation)) {
	};

	House& operator=(House&& moveFromMe);

	House(const House& copyFromMe);

	House& operator=(const House& copyFromMe);

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

	int getDirtLevel(size_t x, size_t y) const;

	bool isWall(const Position& position) const;

	// true iff position inside house boundaries
	bool isInside(const Position& position) const {
		return position.getX() >= 0 && position.getX() < numCols && 
			position.getY() >= 0 && position.getY() < numRows;
	};

	Position getDockingStation();
	
	void validateWalls();

	void validateDocking();

	// true iff dust was vacuumed
	bool clean(const Position& position);

	int getTotalDust();

	static House deseriallize(fs::path filePath);
};

#endif // __HOUSE__H_
