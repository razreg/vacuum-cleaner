#include "Video.h"
#include <stdlib.h>
#include <string>

Logger Video::logger = Logger("Video");

void Video::makeTempDirectory() {
	string cmd = "mkdir -p " + tempDir;
	if (system(cmd.c_str()) == -1) {
		logger.error("Failed to create directory " + tempDir);
		// TODO handle failure
	}
	else if (logger.debugEnabled()) {
		logger.debug("Created temporary directory " + tempDir);
	}
}

void Video::removeTempDirectory() {
	string cmd = "rm -rf " + tempDir;
	if (system(cmd.c_str()) == -1) {
		logger.error("Failed to remove directory " + tempDir);
		// TODO handle failure
	}
	else if (logger.debugEnabled()) {
		logger.debug("Removed temporary directory " + tempDir);
	}
}

void Video::generateTempDirName() {
	char randStr[20];
	char alphanum[] = "0123456789" "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz";
	srand(time(NULL));
	for (size_t i = 0; i < sizeof(randStr) - 1; ++i) {
		randStr[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	randStr[sizeof(randStr) - 1] = '\0';
	tempDir = "./tmp/sim-images-" + string(randStr);
}

void Video::init(size_t rows, size_t cols, string houseName, string algorithmName) {
	this->rows = rows;
	this->cols = cols;
	this->imagePrefix = tempDir + "/" + algorithmName + "-" + houseName + "-image";
	this->videoOutput = "./" + algorithmName + "_" + houseName + ".mpg";
	counter = 1;
}

void Video::composeImage(const House& house, const Position& robotPosition) {
	string counterStr = to_string(counter);
	if (counterStr.length() > 5) {
		return;
	}
	string montageCmd = "montage -geometry 60x60 -tile " + to_string(cols) + "x" + to_string(rows) + " ";
	vector<string> tiles;
	for (size_t row = 0; row < rows; ++row) {
		for (size_t col = 0; col < cols; ++col) {
			if (robotPosition.getX() == col && robotPosition.getY() == row) {
				tiles.push_back("R");
			}
			else {
				char cell = house.getCellValue(row, col);
				tiles.push_back(cell == ' ' ? "0" : string() + cell);
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
		// TODO handle failure
	}
	else {
		counter++;
	}
}

void Video::encode() const {
	string ffmpegCmd = "ffmpeg -v 8 -y -i " + imagePrefix + "%5d.jpg " + videoOutput;
	int ret = system(ffmpegCmd.c_str());
	if (ret == -1) {
		logger.error("Failed to save video to " + videoOutput);
		// TODO handle failure
	}
	else if (logger.debugEnabled()) {
		logger.debug("Saved video to " + videoOutput);
	}
}