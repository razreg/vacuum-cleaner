#ifndef _VIDEO__H_
#define _VIDEO__H_

#include "House.h"
#include "Position.h"
#include "Common.h"

#include <vector>
#include <string>

using namespace std;

const string BASE_DIR = "./tmp/";

class Video {

	enum Failure { NONE, MKDIR, ENCODE, FRAME };

	static Logger logger;
	static int randSeed;

	size_t counter;
	size_t rows;
	size_t cols;
	string imagePrefix;
	string videoOutput;
	string tempDir;

	string algorithmName;
	string houseName;

	// handling errors
	Failure failure = NONE;
	size_t failedFrames = 0;

	void makeTempDirectory();

	void removeTempDirectory(string dir) const;

	void generateTempDirName();

public:

	Video() {};

	~Video() {
		if (!tempDir.empty()) {
			removeTempDirectory(tempDir);
		}
	};

	Video(const Video&) = delete;

	Video& operator=(const Video&) = delete;

	void init(size_t rows, size_t cols, string houseName, string algorithmName, vector<string>& errors);

	void composeImage(const House& house, const Position& robotPosition);

	void encode(vector<string>& errors, bool removeTempFiles = false);

	bool framesFailed() const {
		return failure == FRAME;
	};

	string getFrameErrorString() const;
};

#endif //_VIDEO__H_