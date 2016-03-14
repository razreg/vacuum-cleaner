#include "House.h"

House& House::deseriallize(const string& filePath) {

	string currLine, shortName, description;
	int numRows, numCols;
	char** matrix;

	string houseFileError = "Error: house file [" + filePath + "] is invalid";

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
		cout << houseFileError << endl; // TODO throw custom exception (create our own class - perhaps even in the simulator header)
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
		//TODO throw exception
	}
	return dockingStation;
	
}

void House::validateWalls() const {

	for (int i = 0; i < numCols; i++) {
		if (matrix[0][i] == DOCK || matrix[numRows - 1][i] == DOCK) {
			// throw exception - docking station overriden
		}
		matrix[0][i] = matrix[numRows - 1][i] = WALL;
	}
	for (int i = 1; i < numRows - 1; i++) {
		if (matrix[i][0] == DOCK || matrix[i][numCols - 1] == DOCK) {
			// throw exception - docking station overriden
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