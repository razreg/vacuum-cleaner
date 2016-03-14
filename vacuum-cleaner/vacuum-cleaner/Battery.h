#pragma once

class Battery {

	int capacity;
	int consumptionRate;
	int rechargeRate;

public:

	int getCapacity() {
		return capacity;
	}

	int getConsumptionRate() {
		return consumptionRate;
	}

	int getRechargeRate() {
		return rechargeRate;
	}

	void setCapacity(int capacity) {
		this->capacity = capacity;
	}

	void setConsumptionRate(int consumptionRate) {
		this->consumptionRate = consumptionRate;
	}

	void setRechargeRate(int rechargeRate) {
		this->rechargeRate = rechargeRate;
	}

};