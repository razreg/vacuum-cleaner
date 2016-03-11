#include "SensorImpl.h"

SensorInformation SensorImpl::sense() const {
	SensorInformation sensorInformation;
	sensorInformation.dirtLevel = house->getDirtLevel(*currentPosition);
	sensorInformation.isWall[EAST] = house->isWall(
		Position(currentPosition->X + 1, currentPosition->Y));
	sensorInformation.isWall[WEST] = house->isWall(
		Position(currentPosition->X - 1, currentPosition->Y));
	sensorInformation.isWall[SOUTH] = house->isWall(
		Position(currentPosition->X, currentPosition->Y + 1));
	sensorInformation.isWall[NORTH] = house->isWall(
		Position(currentPosition->X, currentPosition->Y - 1));
	return sensorInformation;
}