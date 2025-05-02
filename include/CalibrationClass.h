#pragma once

// Memory for shared data
#include <memory>

// OpenCV core functions
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>	  // For pollKey()

// Runtime configuration
#include "config.h"

// For vector summing
#include <numeric>


// Forward declarations
class SystemDataManager;
struct ManagedData;


/** 
 * @brief Calibration class definition
 */
class CalibrationClass {

public:
	// Constructor
	CalibrationClass( SystemDataManager& dataHandle );

	// Public class functions
	void CalibrateDevice();
	void Update();
	void FinishCalibration();

private:
	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;

	// Private variables
	std::vector<cv::Point3i> calibrationPoints;

	// ArUco tag
	cv::Mat matAruco = cv::imread( "/home/tom/Code/nuring/images/tags/aruco-08-20mm.png" );


	// Calibration screen
	cv::Mat matCalibration = cv::Mat( CONFIG_FITTS_SCREEN_HEIGHT, CONFIG_FITTS_SCREEN_WIDTH, CV_8UC3 );
};
