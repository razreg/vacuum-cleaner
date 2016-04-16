#ifndef __ALGORITHM_2__H_
#define __ALGORITHM_2__H_

#include <cstdlib>

#include "AbstractAlgorithm.h"
#include "Common.h"

using namespace std;

class Algorithm2 : public AbstractAlgorithm {

	static Logger logger;

	const AbstractSensor* sensor;

	Direction previousDirection; 
	Direction returnedDir;
	vector<Direction> movesBack; 
	vector<Direction> outOFDock;
	int maxStepsAfterWinner;
	int maxSteps;
	int capacity;
	int consumptionRate;
	int rechargeRate;
	int currValue;
	int Xpos, Ypos;

public:

	Algorithm2() {
		srand(time(NULL));
	};

	void setSensor(const AbstractSensor& sensor) override {
		this->sensor = &sensor;
		currValue = capacity;
		Xpos = 0;
		Ypos = 0;
	};

	void setConfiguration(map<string, int> config) override;

	Direction step() override;

	void aboutToFinish(int stepsTillFinishing) override {};

	bool inDockingStation() {
		return Xpos == 0 && Ypos == 0;
	}

	bool isDirectionInList(vector<Direction> directions, Direction direction) {
		vector<Direction>::iterator it;
		it = find(directions.begin(), directions.end(), direction);
		return (it != directions.end());
	}

	void setCurrValue() {
		if (inDockingStation()) { //back to docking station
			currValue = min(currValue -1 + rechargeRate, capacity); // charge battery
			movesBack.erase(movesBack.begin(), movesBack.end()); // empty movesback
		}
		else currValue = max(0, currValue - consumptionRate); // consume battery
	}

	//updates the vector which is responsible for returning back to docking station (opposite of the current returned direction)
	void updateMovesBack() {
		switch (returnedDir)  {
		case (Direction::North) :
			movesBack.push_back(Direction::South);
			break;
		case (Direction::South) :
			movesBack.push_back(Direction::North);
			break;
		case (Direction::East) :
			movesBack.push_back(Direction::West);
			break;
		case (Direction::West) :
			movesBack.push_back(Direction::East);
			break;
		default:
			break;
		}
	}

	//updates the returned direction for the current time unit (which is the previous direction for the next time unit)
	void updatePrevAndReturnDirection(Direction direction) {
		switch (direction)  {
		case (Direction::North) :
			returnedDir = Direction::North;
			previousDirection = Direction::North;
			break;
		case (Direction::South) :
			returnedDir = Direction::South;
			previousDirection = Direction::South;
			break;
		case (Direction::East) :
			returnedDir = Direction::East;
			previousDirection = Direction::East;
			break;
		case (Direction::West) :
			returnedDir = Direction::West;
			previousDirection = Direction::West;
			break;
		case (Direction::Stay) :
			returnedDir = Direction::Stay;
			previousDirection = Direction::Stay;
			break;
		default:
			break;
		}
	}

	//updates the position of the robot
	void updatePosition(Direction direction){
		switch (direction)  {
		case (Direction::North) :
			++Ypos;
			break;
		case (Direction::South) :
			--Ypos;
			break;
		case (Direction::East) :
			++Xpos;
			break;
		case (Direction::West) :
			--Xpos;
			break;
		default:
			break;
		}
	}

	//updates the vector that says which direction was already taken from docking station
	void updateOutOfDock(){
		if (inDockingStation()){
			vector<Direction>::iterator it;
			it = find(outOFDock.begin(), outOFDock.end(), returnedDir);
			switch (returnedDir)  {
			case (Direction::North) :
				if (it == outOFDock.end())
					outOFDock.push_back(Direction::North);
				break;
			case (Direction::South) :
				if (it == outOFDock.end())
					outOFDock.push_back(Direction::South);
				break;
			case (Direction::East) :
				if (it == outOFDock.end())
					outOFDock.push_back(Direction::East);
				break;
			case (Direction::West) :
				if (it == outOFDock.end())
					outOFDock.push_back(Direction::West);
				break;
			default:
				break;
			}
		}
	}
};

#endif // __ALGORITHM_2__H_