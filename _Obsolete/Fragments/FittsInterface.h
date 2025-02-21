#pragma once

// OpenCV libraries
#include <opencv2/core.hpp>		  // Core OpenCV functions
#include <opencv2/highgui.hpp>	  // For pollKey()
#include <opencv2/opencv.hpp>

#include <random>

#include "CaptureInterface.h"
#include "DisplayInterface.h"
#include "FittsConfig.h"
#include "config.h"

// #include <opencv2/objdetect/aruco_detector.hpp> // For AruCo tags
// #include <opencv2/videoio.hpp> // For video capture


// struct Marker {
// 	short		ID				   = 0;
// 	bool		present			   = false;
// 	cv::Point3i error3D			   = cv::Point3i( 0, 0, 0 );
// 	cv::Point2i error2D			   = cv::Point2i( 0, 0 );
// 	float		theta			   = 0.0f;
// 	short		errorMagnitude3D   = 0;
// 	short		errorMagnitude2D   = 0;
// 	short		errorMagnitudeNorm = 0;
// 	float		errorHeading	   = 0.0f;
// };


// Class for the capture interface
class FittsInterface {

public:
	// Constructor
	FittsInterface( CaptureInterface& Capture, DisplayInterface& OutputDisplay ) {
		// Generate random seed
		std::srand( time( NULL ) );
	}

	// Variables
	short		testNumber	   = 1;
	bool		testStarted	   = false;
	bool		testComplete   = false;
	cv::Point2i errorPx		   = cv::Point2i( 0, 0 );
	cv::Point2f errorMm		   = cv::Point2f( 0.0f, 0.0f );
	cv::Point2i markerPosition = cv::Point2i( 0, 0 );

	void StartTest( cv::Mat& targetWindow );
	bool CheckTest( cv::Point3i pts, cv::Mat& targetWindow );
	void EndTest();
	// Function prototypes
	// void GetFrame();
	// void FindTags();

	// // Variables
	// bool					 frameCaptured = false;
	// bool					 markerFound   = false;
	// cv::Mat					 matFrame	   = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	// cv::Mat					 matMarkers	   = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	// cv::Mat					 matGray	   = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC1 );
	// std::vector<Marker>		 Markers;
	// short					 activeMarker  = 0;
	// std::vector<cv::Point2i> activeCorners = { cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ) };

private:
	cv::Mat matAruco01 = cv::imread( "/home/tom/Code/nuring/images/tags/aruco-01-20mm.png" );


	// cv::Mat matAruco02 = cv::imread( "images/tags/aruco-02-20mm.png" );
	// cv::Mat matAruco03 = cv::imread( "images/tags/aruco-03-20mm.png" );
	// cv::Mat matAruco04 = cv::imread( "images/tags/aruco-04-20mm.png" );
	// cv::Mat matAruco05 = cv::imread( "images/tags/aruco-05-20mm.png" );
	// cv::Mat matAruco06 = cv::imread( "images/tags/aruco-06-20mm.png" );
	// cv::Mat matAruco07 = cv::imread( "images/tags/aruco-07-20mm.png" );
	// cv::Mat matAruco08 = cv::imread( "images/tags/aruco-08-20mm.png" );
	// cv::Mat matAruco09 = cv::imread( "images/tags/aruco-09-20mm.png" );
	// cv::Mat matAruco10 = cv::imread( "images/tags/aruco-10-20mm.png" );

	// Functions
};



bool FittsInterface::CheckTest( cv::Point3i pts, cv::Mat& targetWindow ) {

	// Calculate distance in pixel space
	errorPx = cv::Point2i( markerPosition.x - pts.x, markerPosition.y - pts.y );
	errorMm = cv::Point2f( errorPx.x * 0.27f, errorPx.y * 0.27f );


	// Display.DrawFitts()
	// OutputDisplay.DrawFitts( targetWindow, cv::Point2i( pts.x, pts.y ), markerPosition );

	// Draw indicator
	if( pts.z > 0 ) {
		cv::circle( targetWindow, cv::Point2i( pts.x, pts.y ), 36, CONFIG_colRedMd, 2 );
	}

	return true;
	// Start timer
}

void FittsInterface::EndTest() { }



void FittsInterface::StartTest( cv::Mat& targetWindow ) {


	// Clear screen
	targetWindow = CONFIG_colWhite;

	// Calculate marker boundaries
	unsigned short minX = 0 + FITTS_SCREEN_EXCLUSION_ZONE;
	unsigned short maxX = FITTS_SCREEN_WIDTH - FITTS_SCREEN_EXCLUSION_ZONE - 72;
	unsigned short minY = 0 + 72;
	unsigned short maxY = FITTS_SCREEN_HEIGHT - FITTS_SCREEN_EXCLUSION_ZONE - 93;

	// Generate random marker position
	markerPosition.x = minX + ( rand() % ( maxX - minX + 1 ) );
	markerPosition.y = minY + ( rand() % ( maxY - minY + 1 ) );

	// Output position for debugging
	std::cout << "Rand: " << markerPosition.x << " , " << markerPosition.y << "\n";

	// Copy to target window
	matAruco01.copyTo( targetWindow( cv::Rect( markerPosition.x, markerPosition.y, matAruco01.cols, matAruco01.rows ) ) );

	testStarted = true;
}
