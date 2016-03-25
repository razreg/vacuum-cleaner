#include "SensorImpl.h"

SensorInformation SensorImpl::sense() const {
	SensorInformation sensorInformation;
	sensorInformation.dirtLevel = house->getDirtLevel(*currentPosition);
	sensorInformation.isWall[(int) Direction::East] = house->isWall(Position(currentPosition).moveEast());
	sensorInformation.isWall[(int) Direction::West] = house->isWall(Position(currentPosition).moveWest());
	sensorInformation.isWall[(int)Direction::South] = house->isWall(Position(currentPosition).moveSouth());
	sensorInformation.isWall[(int)Direction::North] = house->isWall(Position(currentPosition).moveNorth());
	return sensorInformation;
}