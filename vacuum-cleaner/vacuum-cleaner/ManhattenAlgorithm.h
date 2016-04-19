#ifndef __MAN_ALGORITHM__H_
#define __MAN_ALGORITHM__H_

#include "BaseAlgorithm.h"

using namespace std;

// This class is virtual ("abstract") - cannot be instantiated
class ManhattenAlgorithm : public BaseAlgorithm {

	static const int MAX_BYPASS = 4;
	int xPos, yPos; // coordinates relative to docking station

					// lets the algorithm know that it is now in bypass mode and should not try to return 
					// in manhatten distance even if in need to get to docking station
	int stepsToBypassWall;

	Direction returnToDocking(SensorInformation& sensorInformation);

	Direction keepCleaningOutOfDocking(SensorInformation& sensorInformation);

protected:

	virtual void restartAlgorithm() override {
		BaseAlgorithm::restartAlgorithm();
		xPos = yPos = 0;
		stepsToBypassWall = 0;
	};

	virtual bool isReadyToMoveOn(SensorInformation& sensorInformation) const = 0;

public:

	virtual Direction step() override;

	virtual bool inDockingStation() const override {
		return xPos == 0 && yPos == 0;
	};

	// updates the vector used to return back to docking station
	virtual void storeDataForReturnTrip(Direction direction) override {
		xPos += direction == Direction::East ? 1 : direction == Direction::West ? -1 : 0;
		yPos += direction == Direction::South ? 1 : direction == Direction::North ? -1 : 0;
	};

};

#endif // __MAN_ALGORITHM__H_
