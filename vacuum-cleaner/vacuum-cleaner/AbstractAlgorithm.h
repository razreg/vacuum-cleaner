#ifndef __ABSTRACT_ALGORITHM__H_
#define __ABSTRACT_ALGORITHM__H_

#include <map>
using namespace std;

#include "AbstractSensor.h"
#include "Direction.h"

class AbstractAlgorithm
{
public:

	virtual ~AbstractAlgorithm() {} //TODO found this in the one they published!

	// setSensor is called once when the Algorithm is initialized 
	virtual void setSensor(const AbstractSensor& sensor) = 0;

	// setConfiguration is called once when the Algorithm is initialized - see below 
	virtual void setConfiguration(map<string, int> config) = 0;

	// step is called by the simulation for each time unit 
	virtual Direction step() = 0;

	// this method is called by the simulation either when there is a winner or 
	// when steps == MaxSteps - MaxStepsAfterWinner 
	// parameter stepsTillFinishing == MaxStepsAfterWinner 
	virtual void aboutToFinish(int stepsTillFinishing) = 0;
};

#endif //__ABSTRACT_ALGORITHM__H_