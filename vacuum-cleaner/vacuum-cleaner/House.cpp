#include "House.h"

Logger House::logger = Logger("House");

House::House(const House& copyFromMe) : name(copyFromMe.name), maxSteps(copyFromMe.maxSteps),
numRows(copyFromMe.numRows), numCols(copyFromMe.numCols), dockingStation(copyFromMe.dockingStation) {
	for (size_t i = 0; i < numRows; ++i) {
		vector<char> row;
		for (size_t j = 0; j < numCols; ++j) {
			row.push_back(copyFromMe.matrix[i][j]);
		}
		matrix.push_back(row);
	}
}

House House::deseriallize(fs::path filePath) {

	if (logger.debugEnabled()) logger.debug("Deseriallizing house...");

	bool failedToParsefile = false;
	string filename = filePath.filename().string();
	string houseFileError = filename + ": invalid house file format";

	// House data
	string currLine, houseName;
	size_t nRows = 0, nCols = 0;
	size_t maxNumSteps = 0;
	vector<vector<char>> matrix;

	if ((failedToParsefile = !fs::exists(filePath))) {
		houseFileError = filename + ": cannot open file";
	}
	else {
		ifstream houseFileStream(filePath.string());
		if ((failedToParsefile = !houseFileStream.good())) {
			houseFileError = filename + ": cannot open file";
		}
		else {
			if (!(failedToParsefile = !getline(houseFileStream, houseName).good())) {
				if (logger.debugEnabled()) {
					logger.debug("House name/description=[" + houseName + "]");
				}
				
				for (size_t i = 2; i <= 4 && 
					!(failedToParsefile = !getline(houseFileStream, currLine).good()); ++i) {
					int temp = -1;
					try {
						temp = stoi(currLine);
					}
					catch (exception& e) {
						failedToParsefile = true;
						houseFileError = filename + ": line number " + to_string(i) + " in house file shall be a positive number, found: " + currLine;
						break;
					}
					if (temp <= 0) {
						failedToParsefile = true;
						houseFileError = filename + ": line number " + to_string(i) + " in house file shall be a positive number, found: " + to_string(temp);
						break;
					}
					else {
						switch (i) {
						case 2:
							maxNumSteps = temp;
							break;
						case 3:
							nRows = temp;
							break;
						case 4:
							nCols = temp;
							break;
						}
					}
				}

				if (!failedToParsefile) {
					if (logger.debugEnabled()) {
						logger.debug("House max steps=[" + to_string(maxNumSteps) + "]");
						logger.debug("House number of rows=[" + to_string(nRows) + "], num cols=[" + to_string(nCols) + "]");
					}
					readHouseMatrix(houseFileStream, matrix, nRows, nCols); // TODO what happens when nCols or nRows < 3
				}
			}
			houseFileStream.close();
		}
	}

	if (failedToParsefile) throw invalid_argument(houseFileError.c_str());

	//creating the house based on the previously calculated fields
	return House(filePath.stem().string(), maxNumSteps, nRows, nCols, matrix);
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
					throw invalid_argument(filename + ": too many docking stations (more than one D in house)");
				}
				alreadyFound = true;
			}
		}
	}
	if (!alreadyFound) {
		throw invalid_argument(filename + ": missing docking station (no D in house)");
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