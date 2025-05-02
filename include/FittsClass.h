#pragma once

// Memory for shared data
#include <memory>

// OpenCV core functions
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>	  // For pollKey()

// Library for randomization
#include <random>

// Runtime configuration
#include "config.h"

// Timer library
#include "LoggingClass.h"
#include "TimingClass.h"

// Forward declarations
class SystemDataManager;
class TimingClass;
class LoggingClass;
struct ManagedData;



/** 
 * @brief Display class definition
 */
class FittsClass {

public:
	// Data manager handle
	FittsClass( SystemDataManager& dataHandle, TimingClass& timerHandle, LoggingClass& loggerHandle );

	// Public functions
	void StartTest( char axis );
	void Update();
	void EndTest();



private:
	// Private functions
	unsigned short CalculateNorm( cv::Point2i pt1, cv::Point2i pt2 );

	// Private variables
	cv::Point2i errorPx		   = cv::Point2i( 0, 0 );
	cv::Point2f errorMm		   = cv::Point2f( 0, 0 );
	cv::Point2i markerPosition = cv::Point2i( 0, 0 );
	cv::Point2i touchPosition  = cv::Point2i( 0, 0 );
	short		testNumber	   = 0;
	bool		testStarted	   = false;
	bool		testComplete   = false;

	// ArUco tag
	cv::Mat matAruco01 = cv::imread( "/home/tom/Code/nuring/images/tags/aruco-01-20mm.png" );

	// Target background
	cv::Mat matBackground = cv::Mat( CONFIG_FITTS_SCREEN_HEIGHT, CONFIG_FITTS_SCREEN_WIDTH, CV_8UC3 );

	// Data manager handle
	SystemDataManager&			 dataHandle;
	TimingClass&				 timer;
	LoggingClass&				 logger;
	std::shared_ptr<ManagedData> shared;
};