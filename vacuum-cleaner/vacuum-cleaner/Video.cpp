#include "Video.h"
#include <stdlib.h>
#include <string>

bool Video::makeTempDirectory(const string& dirPath) {
	string cmd = "mkdir -p " + dirPath;
	return system(cmd.c_str()) != -1;
}

bool Video::init(size_t rows, size_t cols, string houseName, string algorithmName) {
	this->rows = rows;
	this->cols = cols;
	this->imagePrefix = tempDir + "/" + algorithmName + "-" + houseName + "-image";
	this->videoOutput = tempDir + "/" + algorithmName + "_" + houseName + ".mpg";
	counter = 1;
	return makeTempDirectory(tempDir);
};

void Video::composeImage(const House& house) {
	string counterStr = to_string(counter);
	if (counterStr.length() > 5) {
		return;
	}
	string montageCmd = "montage -geometry 60x60 -tile " + to_string(cols) + "x" + to_string(rows) + " ";
	vector<string> tiles;
	for (size_t row = 0; row < rows; ++row) {
		for (size_t col = 0; col < cols; ++col) {
			char cell = house.getCellValue(row, col);
			tiles.push_back(cell == ' ' ? "0" : string() + cell);
		}
	}
	for (auto &path : tiles) {
		montageCmd += path + " ";
	}
	montageCmd += imagePrefix + string(5 - counterStr.length(), '0') + counterStr + ".jpg";
	int ret = system(montageCmd.c_str());
	if (ret == -1) {
		// TODO handle error
	}
	else {
		counter++;
	}
}

void Video::encode() const {
	string ffmpegCmd = "ffmpeg -y -loglevel fatal -i " + imagePrefix + "%5d.jpg " + videoOutput;
	int ret = system(ffmpegCmd.c_str());
	if (ret == -1) {
		// TODO handle error
	}
}