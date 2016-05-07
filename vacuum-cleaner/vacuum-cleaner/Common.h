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

typedef int(*ScoreFormula)(const map<string, int>&);

// configuration strings
const string MAX_STEPS = "MaxSteps";
const string MAX_STEPS_AFTER_WINNER = "MaxStepsAfterWinner";
const string BATTERY_CAPACITY = "BatteryCapacity";
const string BATTERY_CONSUMPTION_RATE = "BatteryConsumptionRate";
const string BATTERY_RECHARGE_RATE = "BatteryRechargeRate";

enum LogLevel { DEBUG, INFO, WARN, ERROR, FATAL, OFF };
const string loggerLevels[] = { "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

const LogLevel LOG_LEVEL = INFO;

// simple logger which simply writes to cout but with nice format
class Logger {

	static mutex printLock; // every log printout must acquire this mutex first

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
				lock_guard<mutex> lock(printLock);
				cout << getCurrentDateTime() << "\t"
					<< loggerLevels[level] << "\t"
					<< "t-" << this_thread::get_id() << "\t"
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
		for (size_t i = caller.length(); i < len-1; ++i) {
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

	bool debugEnabled() {
		return LOG_LEVEL == DEBUG;
	};
};

#endif // __COMMON__H_