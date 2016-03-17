#ifndef __COMMON__H_
#define __COMMON__H_

const LogLevel LOG_LEVEL = DEBUG;

#include <time.h>

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

enum LogLevel { DEBUG, INFO, WARN, ERROR, FATAL };
static const string loggerLevels[] = { "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

// simple logger which simply writes to cout but with nice format
class Logger {

	string caller;
	time_t rawTime;

	string getCurrentDateTime() {
		struct tm *timeInfo;
		char buffer[19];
		time(&rawTime);
		timeInfo = localtime(&rawTime);
		strftime(buffer, 19, "%Y-%m-%d  %H:%M:%S", timeInfo);
		return buffer;
	};

	void log(const string& msg, LogLevel level) {
		try {
			if (level > LOG_LEVEL) {
				cout << getCurrentDateTime() << "\t"
					<< loggerLevels[level] << "\t"
					<< caller << "\t"
					<< msg << endl;
			}
		}
		catch (exception e) {
			// never throw exception from logger!
		}
	}

public:
	Logger(string caller) : caller(caller) {};

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