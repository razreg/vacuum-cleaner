#include "House.h"


House& House::deseriallize(const string& filePath) {
	// TODO read file and generate a house from it
	string currLine, shortName, description;
	int numRows, numCols, j, i = 0;
	vector<vector<char>>* matrix;

	string houseFileError = "Error: house file [" + filePath + "] is invalid";

	ifstream houseFileStream(filePath);
	bool failedToParsefile = true; // assume we are going to fail
	if (houseFileStream) {
		failedToParsefile = false; // seems like we're lucky
		try {
			while (getline(houseFileStream, currLine)) {
				switch (i)
				{
					case -1: //TODO do i need this?

						//case of an error
					case 0:
						shortName = currLine;
						i++;
						break;
					case 1:
						description = currLine;
						i++;
						break;
					case 2:
						numRows = stoi(currLine);
						i++;
						break;
					case 3:
						numCols = stoi(currLine);
						i++;
						break;
					default:
						for (j = 0; j < numCols; ++j){
							try{
								(*matrix)[i][j] = currLine.at(j);
							}
							catch (exception e){
								i = -1;
								break;
							}	
						}
						i++;
						break;


				}
			}
			houseFileStream.close();
		}
		catch (exception e) {
			failedToParsefile = true; // not so lucky after all
		}
	}

	if (failedToParsefile) {
		cout << houseFileError << endl; // TODO throw custom exception (create our own class - perhaps even in the simulator header)
	}

	//creating the house based on the previously calculated fields
	House house = { shortName, description, numRows, numCols, *matrix };
	return house;

}

