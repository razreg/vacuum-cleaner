#ifndef __SENSORIMPL__H_
#define __SENSORIMPL__H_

#include "NaiveAlgorithm.h"
//#include "AbstractSensor.h"
//#include "House.h"


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

	bool inDocking() {
		return house->getDockingStation() == *currentPosition;
	};
};

#endif // __SENSORIMPL__H_