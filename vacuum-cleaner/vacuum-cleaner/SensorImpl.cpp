#include "SensorImpl.h"

SensorInformation SensorImpl::sense() const {
	SensorInformation sensorInformation;
	sensorInformation.dirtLevel = house->getDirtLevel(*currentPosition);
	sensorInformation.isWall[(int) Direction::East] = house->isWall(
		Position(currentPosition->X + 1, currentPosition->Y));
	sensorInformation.isWall[(int) Direction::West] = house->isWall(
		Position(currentPosition->X - 1, currentPosition->Y));
	sensorInformation.isWall[(int)Direction::South] = house->isWall(
		Position(currentPosition->X, currentPosition->Y + 1));
	sensorInformation.isWall[(int)Direction::North] = house->isWall(
		Position(currentPosition->X, currentPosition->Y - 1));
	return sensorInformation;
}