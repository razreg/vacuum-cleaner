#include "House.h"

using namespace std;

Logger House::logger = Logger("House");

House& House::deseriallize(const string& filePath) {

	logger.debug("Deseriallizing house...");

	string currLine, shortName, description;
	int numRows, numCols;
	char** matrix;

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
				numRows = stoi(currLine);
				failedToParsefile = !getline(houseFileStream, currLine).good();
				numCols = stoi(currLine);
			}
			logger.debug("House number of rows=[" + to_string(numRows) + "], num cols=[" + to_string(numCols) + "]");
			// read matrix
			matrix = new char*[numRows];
			if (!failedToParsefile) {
				int i = 0;
				while (i < numRows && getline(houseFileStream, currLine)) {
					logger.debug("Current line read [" + currLine + "]");
					matrix[i] = new char[numCols];
					for (int j = 0; j < numCols; ++j) {
						matrix[i][j] = currLine.at(j);
					}
					i++;
				}
			}
		}
		catch (exception e) {
			failedToParsefile = true; // not so lucky after all
		}
		houseFileStream.close();
	}

	if (failedToParsefile) {
		string houseFileError = "house file [" + filePath + "] is invalid";
		logger.error(houseFileError);
		throw invalid_argument(houseFileError.c_str());
	}

	//creating the house based on the previously calculated fields
	House *house = new House(shortName, description, numRows, numCols, matrix);
	return *house;
}

Position House::getDockingStation() {

	if (matrix[dockingStation.Y][dockingStation.X] != DOCK) {
		for (int i = 0; i < numRows; i++) {
			for (int j = 0; j < numCols; j++) {
				if (matrix[i][j] == DOCK) {
					dockingStation = { j, i };
					logger.debug("Docking station found in position=" + (string) dockingStation);
					return dockingStation;
				}
			}
		}
		throw invalid_argument("No docking station found in house.");
	}
	return dockingStation;
	
}

void House::validateWalls() {

	for (int i = 0; i < numCols; i++) {
		if (matrix[0][i] == DOCK || matrix[numRows - 1][i] == DOCK) {
			throw invalid_argument("Docking station was located where wall was expected.");
		}
		matrix[0][i] = matrix[numRows - 1][i] = WALL;
	}
	for (int i = 1; i < numRows - 1; i++) {
		if (matrix[i][0] == DOCK || matrix[i][numCols - 1] == DOCK) {
			throw invalid_argument("Docking station was located where wall was expected.");
		}
		matrix[i][0] = matrix[i][numCols - 1] = WALL;
	}
}

//returns the sum of dust in the house, for the simulator to know when the robot is done cleaning.
int House::getTotalDust() {
	if (totalDust < 0) {
		totalDust = 0;
		for (int i = 0; i < numRows; ++i) {
			for (int j = 0; j < numCols; ++j) {
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
	for (int i = 0; i < numRows; ++i) {
		for (int j = 0; j < numCols; ++j) {
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
