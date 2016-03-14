#pragma once
#include "AbstractSensor.h"
#include "House.h"

extern const int EAST, WEST, SOUTH, NORTH;

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
};