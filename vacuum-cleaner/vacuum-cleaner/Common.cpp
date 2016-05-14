#include "Common.h"

mutex Logger::printLock;

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