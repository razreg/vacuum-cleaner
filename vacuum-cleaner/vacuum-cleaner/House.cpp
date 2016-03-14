#include "House.h"

using namespace std;

House& House::deseriallize(const string& filePath) {

	string currLine, shortName, description;
	int numRows, numCols;
	char** matrix;

	string houseFileError = "house file [" + filePath + "] is invalid";

	ifstream houseFileStream(filePath);
	bool failedToParsefile = true; // assume we are going to fail
	if (houseFileStream) {
		failedToParsefile = false; // seems like we're lucky
		try {
			// read first three lines
			failedToParsefile = 
				getline(houseFileStream, shortName).good() &&
				getline(houseFileStream, description).good() &&
				getline(houseFileStream, currLine).good();
			// read next two lines
			if (!failedToParsefile) {
				numRows = stoi(currLine);
				failedToParsefile = getline(houseFileStream, currLine).good();
				numCols = stoi(currLine);
			}
			// read matrix
			matrix = new char*[numRows];
			if (!failedToParsefile) {
				int i = 0;
				while (i < numRows && getline(houseFileStream, currLine)) {
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
		cout << "ERROR: " << houseFileError << endl; 
		throw exception(houseFileError.c_str());
	}

	//creating the house based on the previously calculated fields
	House house = { shortName, description, numRows, numCols, matrix };
	return house;

}

Position House::getDockingStation() {

	if (matrix[dockingStation.X][dockingStation.Y] != DOCK) {
		for (int i = 0; i < numRows; i++) {
			for (int j = 0; j < numCols; j++) {
				if (matrix[i][j] == DOCK) {
					dockingStation = { i, j };
					return dockingStation;
				}
			}
		}
		throw exception("No docking station found in house.");
	}
	return dockingStation;
	
}

void House::validateWalls() const {

	for (int i = 0; i < numCols; i++) {
		if (matrix[0][i] == DOCK || matrix[numRows - 1][i] == DOCK) {
			throw exception("Docking station was located where wall was expected.");
		}
		matrix[0][i] = matrix[numRows - 1][i] = WALL;
	}
	for (int i = 1; i < numRows - 1; i++) {
		if (matrix[i][0] == DOCK || matrix[i][numCols - 1] == DOCK) {
			throw exception("Docking station was located where wall was expected.");
		}
		matrix[i][0] = matrix[i][numCols - 1] = WALL;
	}
}

//returns the sum of dust in the house, for the simulator to know when the robot is done cleaning.
int House::getTotalDust() {
	if (totalDust < 0) {
		totalDust = 0;
		for (int i = 0; i < numRows; i++) {
			for (int j = 0; j < numCols; j++) {
				totalDust += getDirtLevel(i, j);
			}
		}
	}
	return totalDust;
}