#pragma once

// Necessary libraries
#include <iostream>
#include <memory>

// OpenCV libraries
#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/cudaimgproc.hpp>

// Runtime configuration
#include "config.h"

// Container for marker information
struct Tags {
	short		ID					 = 0;
	bool		present				 = false;
	cv::Point3i error3D				 = cv::Point3i( 0, 0, 0 );
	cv::Point2i error2D				 = cv::Point2i( 0, 0 );
	float		theta				 = 0.0f;
	short		errorMagnitude3D	 = 0;
	short		errorMagnitude2D	 = 0;
	short		errorMagnitudeNorm2D = 0;
	float		errorTheta			 = 0.0f;
	short		area				 = 0;
};

// Shared system variable container
struct ManagedData {

	// System flags
	bool FLAG_MAIN_RUNNING	   = true;
	bool FLAG_FRAME_READY	   = false;
	bool FLAG_TAG_FOUND		   = false;
	bool FLAG_SERIAL_OPEN	   = false;
	bool FLAG_AMPLIFIERS_READY = false;
	bool FLAG_PACKET_WAITING   = false;
	bool FLAG_LOGGING_ON	   = false;

	// Testing
	std::string TASK_NAME	 = "";
	char		TASK_NUMBER	 = 0;
	char		TASK_COMMAND = 0;
	short		TASK_USER_ID = 100;

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
	short timingFrequency = 0;
	float timingTimestamp = 0.0f;

	// Touchscreen variables
	cv::Point3i touchPosition = cv::Point3i( 0, 0, 0 );

	// Serial variables
	std::string serialPacket = "";

	// Display variables
	std::string displayString = "";

	// Logging variables
	std::string loggingFilename = "";
	std::string loggingCustom1	= "";
	std::string loggingCustom2	= "";
	std::string loggingCustom3	= "";
	std::string loggingCustom4	= "";
	std::string loggingCustom5	= "";

	// Task variables
	cv::Point3i fittsErrorPx		= cv::Point3i( 0, 0, 0 );
	cv::Point3i fittsErrorMm		= cv::Point3i( 0, 0, 0 );
	float		fittsCompletionTime = 0.0f;
	short		fittsTestNumber		= 0;
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