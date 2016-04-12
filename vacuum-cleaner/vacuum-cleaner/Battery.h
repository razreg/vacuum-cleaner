#ifndef __BATTERY__H_
#define __BATTERY__H_

#include "Common.h"

class Battery {

	int currValue;
	int capacity;
	int consumptionRate;
	int rechargeRate;

public:

	Battery() {};

	Battery(int capacity, int consumptionRate, int rechargeRate) :
		capacity(capacity), consumptionRate(consumptionRate), rechargeRate(rechargeRate) {};

	int getCurrValue() {
		return currValue;
	};

	int setCurrValue(int newValue) {
		return currValue = max(0, min(newValue, capacity));
	};

	int charge() {
		return currValue = min(currValue + rechargeRate, capacity);
	};

	int consume() {
		return currValue = max(0, currValue - consumptionRate);
	};

	int getCapacity() {
		return capacity;
	};

	int getConsumptionRate() {
		return consumptionRate;
	};

	int getRechargeRate() {
		return rechargeRate;
	};

	void setCapacity(int capacity) {
		this->capacity = capacity;
	};

	void setConsumptionRate(int consumptionRate) {
		this->consumptionRate = consumptionRate;
	};

	void setRechargeRate(int rechargeRate) {
		this->rechargeRate = rechargeRate;
	};

};

#endif // __BATTERY__H_