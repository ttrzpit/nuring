#pragma once

// System data manager
#include "SystemDataManager.h"


// OpenCV
#include <opencv2/core.hpp>

// Memory for shared data
#include <memory>

// Configuration
#include <config.h>

// OpenCV core functions
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>	  // For pollKey()

// Forward declarations
class SystemDataManager;
struct ManagedData;

class TasksClass {

public:
	// Constructor
	TasksClass( SystemDataManager& dataHandle );

	void Calibration();
	void CalibrationStart();
	void CalibrationUpdate();
	void CalibrationFinish();

	void FittsStart();
	void FittsUpdate();
	void FittsFinish();

private:
	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;

	// Private variables
	std::string winCalibration = "Calibration Interface";

	// ArUco tag
	cv::Mat matAruco08 = cv::imread( "/home/tom/Code/nuring/images/tags/aruco-08-20mm-scaled.png" );

	// Target background
	cv::Mat matTaskBackground = cv::Mat( CONFIG_TOUCHSCREEN_HEIGHT_PX, CONFIG_TOUCHSCREEN_WIDTH_PX, CV_8UC3 );

	// Marker position variables
	unsigned short posX = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco08.rows / 2 );
	unsigned short posY = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco08.rows / 2 );

	bool isComplete	 = false;
	bool isFinishing = false;
};
