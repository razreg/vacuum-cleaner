#pragma once
#include "AbstractSensor.h"
#include "House.h"

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