#pragma once
#include <map>
#include "AbstractSensor.h"
#include "Simulator.h"

class AbstractAlgorithm {

public:

	/**
	* setSensor is called once when the Algorithm is initialized
	*/
	virtual void setSensor(AbstractSensor& sensor) = 0;

	/**
	*setConfiguration is called once when the Algorithm is initialized
	*/
	virtual void setConfiguration(map<string, int> config) = 0;

	/**
	* step is called by the simulation for each time unit
	*/

	virtual Direction step() = 0;

	/**
	* this method is called by the simulation either when there is a winner or
	* when steps == MaxSteps - MaxStepsAfterWinner
	* parameter stepsTillFinishing == MaxStepsAfterWinner
	*/
	virtual void aboutToFinish(int stepsTillFinishing) = 0;
};
