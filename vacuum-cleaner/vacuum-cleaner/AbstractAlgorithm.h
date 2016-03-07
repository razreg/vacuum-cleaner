#include "AbstractSensor.h"

class AbstractAlgorithm {
public:
	// setSensor is called once when the Algorithm is initialized
	virtual void setSensor(AbstractSensor& sensor) = 0;
	// step is called by the simulation for each time unit
	virtual Direction step() = 0;
	// this method is called by the simulation either when there is a winner or
	// when steps == MaxSteps - MaxStepsAfterWinner
	// parameter stepsTillFinishing == MaxStepsAfterWinner
	virtual Direction aboutToFinish(int stepsTillFinishing) = 0;
};