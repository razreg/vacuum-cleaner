#ifndef __COMMON__H_
#define __COMMON__H_

#include <time.h> // srand
#include <fstream> // files
#include <iostream>
#include <string>
#include <typeinfo> // logging algorithm class names
#include <algorithm> // min, max

using namespace std;

// configuration strings
const string MAX_STEPS = "MaxSteps";
const string MAX_STEPS_AFTER_WINNER = "MaxStepsAfterWinner";
const string BATTERY_CAPACITY = "BatteryCapacity";
const string BATTERY_CONSUMPTION_RATE = "BatteryConsumptionRate";
const string BATTERY_RECHARGE_RATE = "BatteryRechargeRate";

// default configuration values
const int DEFAULT_MAX_STEPS = 1200;
const int DEFAULT_MAX_STEPS_AFTER_WINNER = 200;
const int DEFAULT_BATTERY_CAPACITY = 400;
const int DEFAULT_BATTERY_CONSUMPTION_RATE = 1;
const int DEFAULT_BATTERY_RECHARGE_RATE = 20;

enum LogLevel { DEBUG, INFO, WARN, ERROR, FATAL, OFF };
const string loggerLevels[] = { "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

const LogLevel LOG_LEVEL = DEBUG;

// simple logger which simply writes to cout but with nice format
class Logger {

	char caller[13];
	time_t rawTime;

	string getCurrentDateTime() {
		struct tm *timeInfo;
		time(&rawTime);
		timeInfo = localtime(&rawTime);
		char buffer[21];
		strftime(buffer, 21, "%Y-%m-%d  %H:%M:%S", timeInfo);
		return buffer;
	};

	void log(const string& msg, LogLevel level) {
		try {
			if (level >= LOG_LEVEL) {
				cout << getCurrentDateTime() << "\t"
					<< loggerLevels[level] << "\t"
					<< caller << " "
					<< msg << endl;
			}
		}
		catch (exception e) {
			// never throw exception from logger!
		}
	}

public:
	Logger(string caller) {
		size_t len = sizeof(this->caller);
		strncpy(this->caller, caller.c_str(), len);
		for (int i = caller.length(); i < len-1; ++i) {
			this->caller[i] = ' ';
		}
		this->caller[len - 1] = '\0';
	};

	void fatal(const string& msg) {
		log(msg, FATAL);
	};

	void error(const string& msg) {
		log(msg, ERROR);
	};

	void warn(const string& msg) {
		log(msg, WARN);
	};

	void info(const string& msg) {
		log(msg, INFO);
	};

	void debug(const string& msg) {
		log(msg, DEBUG);
	};
};

#endif // __COMMON__H_