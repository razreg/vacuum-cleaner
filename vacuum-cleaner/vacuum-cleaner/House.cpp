#include "House.h"

using namespace std;

Logger House::logger = Logger("House");

House House::deseriallize(const string& filePath) {

	if (logger.debugEnabled()) logger.debug("Deseriallizing house...");

	string houseFileError = "house file [" + filePath + "] does not exist";
	string currLine, houseName;
	size_t nRows = 0, nCols = 0;
	size_t maxNumSteps = 0;
	vector<vector<char>> matrix;

	ifstream houseFileStream(filePath);
	bool failedToParsefile = true; // assume we are going to fail
	if (houseFileStream) {
		houseFileError = "house file [" + filePath + "] is invalid";
		failedToParsefile = false; // seems like we're lucky
		try {
			failedToParsefile = 
				!getline(houseFileStream, houseName).good() || 
				!getline(houseFileStream, currLine).good();
			if (!failedToParsefile) {
				if (logger.debugEnabled()) {
					logger.debug("House name/description=[" + houseName + "]");
				}
				maxNumSteps = max(0, stoi(currLine));
				failedToParsefile = !getline(houseFileStream, currLine).good();
				if (!failedToParsefile) {
					nRows = max(0, stoi(currLine));
					failedToParsefile = !getline(houseFileStream, currLine).good();
					if (!failedToParsefile) {
						nCols = max(0, stoi(currLine));
					}
				}
			}
			if (!failedToParsefile) {
				if (logger.debugEnabled()) {
					logger.debug("House max steps=[" + to_string(maxNumSteps) + "]");
					logger.debug("House number of rows=[" + to_string(nRows) + "], num cols=[" + to_string(nCols) + "]");
				}
				if (nRows < 3 || nCols < 3) {
					failedToParsefile = true; // house is only walls
					houseFileError = "Number of rows or cols is too small in house file [" + filePath + "]";
				}
			}
			// read matrix
			if (!failedToParsefile) {
				readHouseMatrix(houseFileStream, matrix, nRows, nCols);
			}
		}
		catch (exception e) {
			failedToParsefile = true; // not so lucky after all
		}
		houseFileStream.close();
	}

	if (failedToParsefile) {
		throw invalid_argument(houseFileError.c_str());
	}

	//creating the house based on the previously calculated fields
	return House(filePath, houseName, maxNumSteps, nRows, nCols, matrix);
}

void House::readHouseMatrix(ifstream& houseFileStream, vector<vector<char>>& matrix, size_t nRows, size_t nCols) {
	string currLine;
	size_t i = 0;
	while (i < nRows && getline(houseFileStream, currLine)) {
		if (logger.debugEnabled()) logger.debug("Current line read [" + currLine + "]");
		vector<char> row;
		for (size_t j = 0; j < nCols; ++j) {
			row.push_back((j < currLine.length()) ? currLine.at(j) : ' '); // if there is no char j then store space
			if (row[j] != DOCK && row[j] != WALL && (row[j] < '1' || row[j] > '9')) {
				row[j] = ' '; // every unrecognized character (or '0') turns to whitespace
			}
		}
		matrix.push_back(row);
		i++;
	}
	// add space rows if too few rows were read from file
	for (; i < nRows; ++i) {
		vector<char> row;
		for (size_t j = 0; j < nCols; ++j) {
			row.push_back(' ');
		}
		matrix.push_back(row);
	}
}

Position House::getDockingStation() {
	if (matrix[dockingStation.getY()][dockingStation.getX()] != DOCK) {
		validateDocking();
	}
	return dockingStation;
	
}

void House::validateDocking() {
	bool alreadyFound = false;
	for (size_t i = 1; i < numRows-1; ++i) {
		for (size_t j = 1; j < numCols-1; ++j) {
			if (matrix[i][j] == DOCK) {
				dockingStation = { j, i };
				if (logger.debugEnabled()) {
					logger.debug("Docking station found in position=" + (string)dockingStation);
				}
				if (alreadyFound) {
					throw invalid_argument("House contains more than one docking station");
				}
				alreadyFound = true;
			}
		}
	}
	if (!alreadyFound) {
		throw invalid_argument("No docking station found in house");
	}
}

void House::validateWalls() {

	for (size_t i = 0; i < numCols; ++i) {
		if (matrix[0][i] == DOCK || matrix[numRows - 1][i] == DOCK) {
			logger.warn("Docking station was located where wall was expected");
		}
		matrix[0][i] = matrix[numRows - 1][i] = WALL;
	}
	for (size_t i = 1; i < numRows - 1; ++i) {
		if (matrix[i][0] == DOCK || matrix[i][numCols - 1] == DOCK) {
			logger.warn("Docking station was located where wall was expected");
		}
		matrix[i][0] = matrix[i][numCols - 1] = WALL;
	}
}

//returns the sum of dust in the house, for the simulator to know when the robot is done cleaning.
int House::getTotalDust() {
	if (totalDust < 0) {
		totalDust = 0;
		for (size_t i = 0; i < numRows; ++i) {
			for (size_t j = 0; j < numCols; ++j) {
				totalDust += getDirtLevel(j, i);
			}
		}
	}
	return totalDust;
}

House::operator string() const {
	string house = "";
	char *arr = new char[numCols + 1];
	arr[numCols] = '\0';
	for (size_t i = 0; i < numRows; ++i) {
		for (size_t j = 0; j < numCols; ++j) {
			arr[j] = matrix[i][j];
		}
		house += arr;
		if (i < numRows - 1) {
			house += '\n';
		}
	}
	delete [] arr;
	return house;
}