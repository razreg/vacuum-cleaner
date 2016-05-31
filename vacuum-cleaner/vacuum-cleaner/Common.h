#ifndef __COMMON__H_
#define __COMMON__H_

#include <dlfcn.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring> // strncpy
#include <algorithm> // min, max, find
#include <fstream> // files
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <list>
#include <functional>
#include <memory>

// threads
#include <thread>
#include <mutex>
#include <atomic>

#include "uniqueptr.h"

using namespace std;
namespace fs = boost::filesystem;

typedef int(*ScoreFormula)(const map<string, int>&);

// utility functions
void trimString(string& str);

// configuration strings
const string MAX_STEPS = "MaxSteps";
const string MAX_STEPS_AFTER_WINNER = "MaxStepsAfterWinner";
const string BATTERY_CAPACITY = "BatteryCapacity";
const string BATTERY_CONSUMPTION_RATE = "BatteryConsumptionRate";
const string BATTERY_RECHARGE_RATE = "BatteryRechargeRate";

enum LogLevel { DEBUG, INFO, WARN, ERROR, FATAL, OFF };
const string loggerLevels[] = { "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

const LogLevel LOG_LEVEL = DEBUG;

// simple logger which simply writes to cout but with nice format
class Logger {

	static mutex printLock; // every log printout must acquire this mutex first

	char caller[13];
	time_t rawTime;

	string getCurrentDateTime();

	void log(const string& msg, LogLevel level);

public:

	Logger(string caller);

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

	bool debugEnabled() {
		return LOG_LEVEL == DEBUG;
	};
};

#endif // __COMMON__H_