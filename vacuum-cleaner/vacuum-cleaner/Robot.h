#ifndef __ROBOT__H_
#define __ROBOT__H_

#include <utility>

#include "SensorImpl.h"
#include "Direction.h"
#include "AbstractAlgorithm.h"
#include "House.h"
#include "Battery.h"
#include "Position.h"
#include "Common.h"
#include "Video.h"

static const string directions[] = { "East", "West", "South", "North", "Stay" };

// please note that this robot is part of the simulator - it only encapsulates some simulator logic. 
// Therefore, this robot is aware of the sensor implementation just like the simulator should be.
class Robot {

	static Logger logger;

	AbstractAlgorithm& algorithm;
	string algorithmName;
	House house;
	SensorImpl sensor;
	Battery battery;
	Position position;
	Direction prevStep;

	bool captureVideo;
	Video video;
	vector<string> videoErrors;
	
	bool illegalStepPerformed = false;
	bool batteryDead = false;

	void configBattery(const map<string, int>& configMap);

	void updateSensorWithHouse();

	void configure(const map<string, int>& configMap);

public:

	Robot(const map<string, int>& configMap, AbstractAlgorithm& algorithm, string algorithmName, 
		House&& house, bool captureVideo = false) :
		algorithm(algorithm), algorithmName(algorithmName), house(house), captureVideo(captureVideo) {
		updateSensorWithHouse();
		configure(configMap);
	};

	Robot(const map<string, int>& configMap, AbstractAlgorithm& algorithm, string algorithmName, bool captureVideo = false) :
		algorithm(algorithm), algorithmName(algorithmName), captureVideo(captureVideo) {
		configure(configMap);
	};

	void setHouse(House&& house);

	void restart();

	House& getHouse() {
		return house;
	};

	Position getPosition() const {
		return position;
	};

	size_t getBatteryValue() const {
		return battery.getCurrValue();
	};

	void step();

	bool inDocking() const {
		return sensor.inDocking();
	};

	void aboutToFinish(int stepsTillFinishing) {
		algorithm.aboutToFinish(stepsTillFinishing);
	};

	void reportBadBehavior() {
		illegalStepPerformed = true;
	};

	bool performedIllegalStep() const {
		return illegalStepPerformed;
	};

	string getAlgorithmName() const {
		return algorithmName;
	};

	void setBatteryDeadNotified() {
		batteryDead = true;
	};

	bool isBatteryDeadNotified() const {
		return batteryDead;
	};

	bool isFinished() {
		return house.getTotalDust() == 0 && inDocking();
	};

	void captureSnapshot() {
		video.composeImage(house, position);
	};

	void saveVideo(bool removeTempFiles = false) {
		video.encode(videoErrors, removeTempFiles);
	};

	vector<string> getVideoErrors();

};

#endif // __ROBOT__H_