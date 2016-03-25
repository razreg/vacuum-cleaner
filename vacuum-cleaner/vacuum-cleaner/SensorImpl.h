#ifndef __SENSORIMPL__H_
#define __SENSORIMPL__H_

#include "AbstractSensor.h"
#include "House.h"
#include "Direction.h"
#include "Position.h"
#include "SensorInformation.h"

class SensorImpl : public AbstractSensor {

	House* house;
	Position* currentPosition;

public:

	void setPosition(Position& position) {
		currentPosition = &position;
	}

	void setHouse(House& house) {
		this->house = &house;
	}

	SensorInformation sense() const override;

	bool inDocking() const {
		return house->getDockingStation() == *currentPosition;
	};
};

#endif // __SENSORIMPL__H_