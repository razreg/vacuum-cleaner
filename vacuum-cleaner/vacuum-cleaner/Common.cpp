#include "Common.h"

mutex Logger::printLock;

Logger::Logger(string caller) {
	size_t len = sizeof(this->caller);
	strncpy(this->caller, caller.c_str(), len);
	for (size_t i = caller.length(); i < len - 1; ++i) {
		this->caller[i] = ' ';
	}
	this->caller[len - 1] = '\0';
}

string Logger::getCurrentDateTime() {
	struct tm *timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	char buffer[21];
	strftime(buffer, 21, "%Y-%m-%d  %H:%M:%S", timeInfo);
	return buffer;
}

void Logger::log(const string& msg, LogLevel level) {
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

// removes leading and trailing whitespaces
void trimString(string& str) {
	if (str.length() == 0) {
		return;
	}
	size_t first = max(str.find_first_not_of(' '), str.find_first_not_of('\t'));
	char lasts[] = { ' ', '\t', '\r', '\n', '\0' };
	size_t last = str.length() - 1;
	for (int i = 0; lasts[i] != '\0'; ++i) {
		last = min(last, str.find_last_not_of(lasts[i]));
	}
	str = str.substr(first, (last - first + 1));
}