#include "House.h"

using namespace std;

Logger House::logger = Logger("House");

House& House::deseriallize(const string& filePath) {

	logger.debug("Deseriallizing house...");

	string currLine, shortName, description;
	size_t nRows = 0, nCols = 0;
	char** matrix = nullptr;

	ifstream houseFileStream(filePath);
	bool failedToParsefile = true; // assume we are going to fail
	if (houseFileStream) {
		failedToParsefile = false; // seems like we're lucky
		try {
			// read first three lines
			failedToParsefile = 
				!getline(houseFileStream, shortName).good() ||
				!getline(houseFileStream, description).good() ||
				!getline(houseFileStream, currLine).good();
			logger.debug("House short name=[" + shortName + "], description=[" + description + "]");
			// read next two lines
			if (!failedToParsefile) {
				nRows = stoi(currLine);
				failedToParsefile = !getline(houseFileStream, currLine).good();
				nCols = stoi(currLine);
			}
			logger.debug("House number of rows=[" + to_string(nRows) + "], num cols=[" + to_string(nCols) + "]");
			// read matrix
			matrix = new char*[nRows];
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
		string houseFileError = "house file [" + filePath + "] is invalid or does not exist";
		throw invalid_argument(houseFileError.c_str());
	}

	//creating the house based on the previously calculated fields
	House *house = new House(shortName, description, nRows, nCols, matrix);
	return *house;
}

void House::readHouseMatrix(ifstream& houseFileStream, char** matrix, size_t nRows, size_t nCols) {
	string currLine;
	size_t i = 0;
	while (i < nRows && getline(houseFileStream, currLine)) {
		logger.debug("Current line read [" + currLine + "]");
		matrix[i] = new char[nCols];
		for (size_t j = 0; j < nCols; ++j) {
			matrix[i][j] = (j < currLine.length()) ? currLine.at(j) : ' '; // if there is no char j then store space
			if (matrix[i][j] != DOCK && matrix[i][j] != WALL && (matrix[i][j] < '1' || matrix[i][j] > '9')) {
				matrix[i][j] = ' '; // every unrecognized character (or '0') turns to whitespace
			}
		}
		i++;
	}
	// add space rows if too few rows were read from file
	for (; i < nRows; ++i) {
		matrix[i] = new char[nCols];
		for (size_t j = 0; j < nCols; ++j) {
			matrix[i][j] = ' ';
		}
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
				logger.debug("Docking station found in position=" + (string)dockingStation);
				if (alreadyFound) {
					throw invalid_argument("House contains more than one docking station");
				}
				alreadyFound = true;
				dockingStation = { j, i };
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