#pragma once

// Necessary libraries
#include <iostream>
#include <memory>

// OpenCV libraries
#include <ArucoClass.h>
#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/cudaimgproc.hpp>

// Runtime configuration
#include "config.h"

// Constants
#define RAD2DEG 57.2958
#define DEG2RAD 0.01745


// Container for marker information
struct Tags {
	short		ID					 = 0;
	bool		present				 = false;
	cv::Point3f error3D				 = cv::Point3i( 0, 0, 0 );
	cv::Point3f errorPrev3D			 = cv::Point3i( 0, 0, 0 );
	cv::Point3f errorVel3D			 = cv::Point3i( 0, 0, 0 );
	cv::Point2f error2D				 = cv::Point2i( 0, 0 );
	float		theta				 = 0.0f;
	short		errorMagnitude3D	 = 0;
	short		errorMagnitude2D	 = 0;
	short		errorMagnitudeNorm2D = 0;
	float		errorTheta			 = 0.0f;
	float		velMagnitude		 = 0.0f;
	float		velHeading			 = 0.0f;
	short		area				 = 0;
	cv::Vec3d	rotationVector		 = cv::Vec3d( 0, 0, 0 );
	cv::Vec3d	translationVector	 = cv::Vec3d( 0, 0, 0 );

	KalmanFilter3D kf;
};

// Shared system variable container
struct ManagedData {

	// System flags
	bool FLAG_MAIN_RUNNING	   = true;
	bool FLAG_FRAME_READY	   = false;
	bool FLAG_TAG_FOUND		   = false;
	bool FLAG_AMPLIFIERS_READY = false;
	bool FLAG_PACKET_WAITING   = false;
	bool FLAG_LOGGING_ENABLED  = false;
	bool FLAG_LOGGING_STARTED  = false;
	bool FLAG_SERIAL0_OPEN	   = false;
	bool FLAG_SERIAL1_OPEN	   = false;
	bool FLAG_SERIAL0_ENABLED  = false;
	bool FLAG_SERIAL1_ENABLED  = false;
	bool FLAG_SHUTTING_DOWN	   = false;

	// Tasks
	std::string TASK_NAME		= "";		// String name of task
	uint8_t		TASK_USER_ID	= 000;		// Participant ID
	char		TASK_REP_NUMBER = 0;		// Task rep number
	char		TASK_COMMAND	= 0;		// Command
	bool		TASK_RUNNING	= false;	// Is the task running
	bool		TASK_COMPLETE	= false;	// Trip when task is complete

	// OpenCV image matrices
	cv::Mat matFrameRaw			= cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::Mat matFrameGray		= cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::Mat matFrameUndistorted = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::Mat matFrameOverlay		= cv::Mat( CONFIG_DIS_HEIGHT, CONFIG_DIS_WIDTH, CV_8UC3 );
	cv::Mat matRemap1;
	cv::Mat matRemap2;

	// OpenCV GPU matrices
	cv::cuda::GpuMat GpuMatFrameRaw			= cv::cuda::GpuMat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::cuda::GpuMat GpuMatFrameGray		= cv::cuda::GpuMat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC1 );
	cv::cuda::GpuMat GpuMatFrameUndistorted = cv::cuda::GpuMat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC1 );
	cv::cuda::GpuMat GpuMatRemap1;
	cv::cuda::GpuMat GpuMatRemap2;

	// ArUco detector
	cv::aruco::Dictionary				  arucoDictionary;
	cv::aruco::DetectorParameters		  arucoDetectorParams;
	cv::aruco::ArucoDetector			  arucoDetector;
	std::vector<Tags>					  arucoTags;
	std::vector<int>					  arucoIDs;
	std::vector<std::vector<cv::Point2f>> arucoCorners, arucoRejects;
	std::vector<cv::Vec3d>				  arucoRotationVector, arucoTranslationVector;
	cv::Mat								  arucoPoints { 4, 1, CV_32FC3 };
	short								  arucoActiveID		 = 0;
	std::vector<cv::Point2i>			  arucoActiveCorners = { cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ) };
	short								  arucoMinArea		 = 150;
	short								  arucoMaxArea		 = 30000;

	// Timing variables
	short timingFrequency = 45;
	float timingTimestamp = 0.0f;
	float timingTimestep  = 0.0f;

	// Touchscreen variables
	cv::Point3i touchPosition	   = cv::Point3i( 0, 0, 0 );
	bool		touchDetected = false;

	// Serial variables
	std::string serialPacket0	= "";
	std::string serialPacket1	= "";
	uint8_t		serialPortsOpen = 0;

	// Display variables
	std::string displayString = "";

	// Logging variables
	std::string loggingFilename	 = "NULL";
	std::string loggingHeader1	 = "Heading1";
	std::string loggingHeader2	 = "Heading2";
	std::string loggingHeader3	 = "Heading3";
	std::string loggingHeader4	 = "Heading4";
	std::string loggingHeader5	 = "Heading5";
	std::string loggingVariable1 = "0";
	std::string loggingVariable2 = "0";
	std::string loggingVariable3 = "0";
	std::string loggingVariable4 = "0";
	std::string loggingVariable5 = "0";

	// Controller variables
	float	controllerK		= 0.010f;	 // N/mm
	float	controllerB		= 0.000f;	 // N*s/mm
	float	controllerFx	= 0.00f;	 // N
	float	controllerFy	= 0.00f;	 // N
	int16_t controllerCompZ = 0;		 // mm

	// Task variables
	cv::Point3i fittsErrorPx		= cv::Point3i( 0, 0, 0 );
	cv::Point3i fittsErrorMm		= cv::Point3i( 0, 0, 0 );
	float		fittsCompletionTime = 0.0f;
	short		fittsTestNumber		= 0;
	// bool		fittsTestStarted	= false;
	cv::Point2i fittsMarkerPosition = cv::Point2i( 0, 0 );
	char		fittsActiveAxis		= 'z';

	// 3D Visualization
	bool vizClear	= false;
	bool vizEnabled = false;
	bool vizLoaded	= false;

	// 2D Angle Visualization
	bool  angleEnabled = false;
	bool  angleLoaded  = false;
	float angleTheta   = 0.0f;

	// Calibration
	bool		calibrationComplete = false;
	cv::Point3i calibrationOffsetMM = cv::Point3i( 0, 0, 0 );
	bool		calibrationLoaded	= false;
};

class SystemDataManager {
public:
	// Class
	SystemDataManager();

	// Functions
	std::shared_ptr<ManagedData> getData();

private:
	std::shared_ptr<ManagedData> data;
};