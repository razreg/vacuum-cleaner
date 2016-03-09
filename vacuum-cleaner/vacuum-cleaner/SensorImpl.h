#pragma once
#include "AbstractSensor.h"

class SensorImpl : public AbstractSensor {

public:

	SensorInformation sense() const;
};