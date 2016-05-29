#ifndef _VIDEO__H_
#define _VIDEO__H_

#include "House.h"

#include <vector>
#include <string>
#include <stdlib.h>

using namespace std;

const string tempDir = "./vidtmp";

class Video {

	size_t counter;
	size_t rows;
	size_t cols;
	string imagePrefix;
	string videoOutput;

	bool makeTempDirectory(const string& dirPath);

public:

	bool init(size_t rows, size_t cols, string houseName, string algorithmName);

	void composeImage(const House& house);

	void encode() const;
};

#endif //_VIDEO__H_