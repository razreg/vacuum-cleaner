#ifndef __BATTERY__H_
#define __BATTERY__H_

class Battery {

	size_t currValue;
	size_t capacity;
	size_t consumptionRate;
	size_t rechargeRate;

public:

	Battery() {};

	Battery(size_t capacity, size_t consumptionRate, size_t rechargeRate) :
		capacity(capacity), consumptionRate(consumptionRate), rechargeRate(rechargeRate) {};

	size_t getCurrValue() const {
		return currValue;
	};

	size_t setCurrValue(int newValue) {
		return currValue = newValue < 0 ? 0 : min((size_t) newValue, capacity);
	};

	size_t charge() {
		return currValue = min(currValue + rechargeRate, capacity);
	};

	size_t consume() {
		return currValue = currValue < consumptionRate ? 0 : currValue - consumptionRate;
	};

	size_t getCapacity() const {
		return capacity;
	};

	size_t getConsumptionRate() const {
		return consumptionRate;
	};

	size_t getRechargeRate() const {
		return rechargeRate;
	};

	void setCapacity(size_t capacity) {
		this->capacity = capacity;
	};

	void setConsumptionRate(size_t consumptionRate) {
		this->consumptionRate = consumptionRate;
	};

	void setRechargeRate(size_t rechargeRate) {
		this->rechargeRate = rechargeRate;
	};

	bool empty() const {
		return currValue == 0;
	};

	bool full() const {
		return currValue >= capacity;
	};

};

#endif // __BATTERY__H_