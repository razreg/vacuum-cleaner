#pragma once
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "House.h"

// exit codes
const int SUCCESS = 0;
const int INVALID_ARGUMENTS = 1;
const int INVALID_CONFIGURATION = 2;

// default configuration values
const int DEFAULT_MAX_STEPS = 1200;
const int DEFAULT_MAX_STEPS_AFTER_WINNER = 200;
const int DEFAULT_BATTERY_CAPACITY = 400;
const int DEFAULT_BATTERY_CONSUMPTION_RATE = 1;
const int DEFAULT_BATTERY_RECHARGE_RATE = 20;

struct Configuration {
	int maxSteps;
	int maxStepsAfterWinner;
	int batteryCapacity;
	int batteryConsumptionRate;
	int batteryRachargeRate;
};