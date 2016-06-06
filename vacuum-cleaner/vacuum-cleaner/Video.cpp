#include "Video.h"
#include <stdlib.h>
#include <string>

Logger Video::logger = Logger("Video");
int Video::randSeed = 1;

void Video::makeTempDirectory() {
	string cmd = "mkdir -p " + tempDir;
	if (system(cmd.c_str()) == -1) {
		logger.error("Failed to create directory " + tempDir);
		failure = MKDIR;
	}
	else if (logger.debugEnabled()) {
		logger.debug("Created temporary directory " + tempDir);
	}
}

void Video::removeTempDirectory(string dir, vector<string>& errors) const {
	try {
		fs::path path(dir);
		if (fs::is_directory(path)) {
			string cmd = "rm -rf " + dir;
			if (system(cmd.c_str()) == -1) {
				logger.error("Failed to remove directory " + dir);
				errors.push_back("Error: In the simulation " + algorithmName + ", "
					+ houseName + ": folder removal of " + tempDir + " failed");
			}
			else if (logger.debugEnabled()) {
				logger.debug("Removed temporary directory " + dir);
			}
		}
		if (dir != BASE_DIR) {
			path = fs::path(BASE_DIR);
			if (fs::is_directory(path) && fs::is_empty(path)) {
				removeTempDirectory(BASE_DIR, errors);
			}
		}
	}
	catch (exception& e) {
		logger.error("Exception thrown while trying to remove temporary folder");
	}
}

void Video::generateTempDirName() {
	char randStr[20];
	char alphanum[] = "0AaBbCc1DdEeFf2GgHhIiJ3jKkLlM4mNnOoP5pQqRr6SsTtUu7VvWwX8xYyZz9";
	srand(randSeed++);
	for (size_t i = 0; i < sizeof(randStr) - 1; ++i) {
		randStr[i] = alphanum[rand() % sizeof(randStr)];
	}
	randStr[sizeof(randStr) - 1] = '\0';
	tempDir = BASE_DIR + "tmp-" + string(randStr);
}

void Video::init(size_t rows, size_t cols, string houseName, 
	string algorithmName, vector<string>& errors) {

	failure = NONE;
	failedFrames = 0;

	if (!tempDir.empty()) removeTempDirectory(tempDir, errors);
	generateTempDirName();
	makeTempDirectory();
	if (failure == MKDIR) {
		errors.push_back("Error: In the simulation " + algorithmName + ", " 
			+ houseName + ": folder creation " + tempDir + " failed");
		return;
	}

	this->algorithmName = algorithmName;
	this->houseName = houseName;
	this->rows = rows;
	this->cols = cols;
	this->imagePrefix = tempDir + "/" + algorithmName + "-" + houseName + "-image";
	this->videoOutput = "./" + algorithmName + "_" + houseName + ".mpg";
	counter = 0;
}

void Video::composeImage(const House& house, const Position& robotPosition) {
	if (failure != NONE) {
		if (failure == FRAME) failedFrames++;
		return;
	}
	string counterStr = to_string(counter);
	if (counterStr.length() > 5) {
		return;
	}
	string montageCmd = "montage -geometry 60x60 -tile " + to_string(cols) + "x" + to_string(rows) + " ";
	vector<string> tiles;
	for (size_t row = 0; row < rows; ++row) {
		for (size_t col = 0; col < cols; ++col) {
			if (robotPosition.getX() == col && robotPosition.getY() == row) {
				tiles.push_back(PATH_TO_AVATARS + "R");
			}
			else {
				char cell = house.getCellValue(row, col);
				tiles.push_back(cell == ' ' ? PATH_TO_AVATARS + "0" : PATH_TO_AVATARS + cell);
			}
		}
	}
	for (auto &path : tiles) {
		montageCmd += path + " ";
	}
	string frameFileName = imagePrefix + string(5 - counterStr.length(), '0') + counterStr + ".jpg";
	montageCmd += frameFileName;
	int ret = system(montageCmd.c_str());
	if (ret == -1) {
		logger.error("Failed to save frame to " + frameFileName);
		failure = FRAME;
		failedFrames++;
	}
	else {
		counter++;
	}
}

void Video::encode(vector<string>& errors, bool removeTempFiles) {
	if (failure != NONE) return;
	string ffmpegCmd = "ffmpeg -y -i " + imagePrefix + "%05d.jpg " + videoOutput;
	int ret = system(ffmpegCmd.c_str());
	if (ret == -1) {
		failure = ENCODE;
		logger.error("Failed to save video to " + videoOutput);
		errors.push_back("Error: In the simulation " + algorithmName + ", " 
			+ houseName + ": video file creation failed");
	}
	else if (logger.debugEnabled()) {
		logger.debug("Saved video to " + videoOutput);
	}
	if (removeTempFiles) removeTempDirectory(tempDir, errors);
}

string Video::getFrameErrorString() const {
	return "Error: In the simulation " + algorithmName + ", " + houseName +
		": the creation of " + to_string(failedFrames) + " images has failed";
}