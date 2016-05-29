#ifndef _VIDEO__H_
#define _VIDEO__H_

#include "House.h"
#include "Position.h"
#include "Common.h"

#include <vector>
#include <string>
#include <stdlib.h>
#include <ctime>

using namespace std;

class Video {

	static Logger logger;

	size_t counter;
	size_t rows;
	size_t cols;
	string imagePrefix;
	string videoOutput;
	string tempDir;

	void makeTempDirectory();

	void removeTempDirectory();

	void generateTempDirName();

public:

	Video() {
		generateTempDirName();
		makeTempDirectory();
	};

	~Video() {
		removeTempDirectory();
	};

	Video(const Video&) = delete;

	Video& operator=(const Video&) = delete;

	void init(size_t rows, size_t cols, string houseName, string algorithmName);

	void composeImage(const House& house, const Position& robotPosition);

	void encode() const;
};

#endif //_VIDEO__H_