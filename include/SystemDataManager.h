#pragma once

// Necessary libraries
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

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
// struct Tags {
// 	short		ID					 = 0;
// 	bool		present				 = false;
// 	cv::Point3f error3D				 = cv::Point3i( 0, 0, 0 );
// 	cv::Point3f errorPrev3D			 = cv::Point3i( 0, 0, 0 );
// 	cv::Point3f errorVel3D			 = cv::Point3i( 0, 0, 0 );
// 	cv::Point2f error2D				 = cv::Point2i( 0, 0 );
// 	float		theta				 = 0.0f;
// 	short		errorMagnitude3D	 = 0;
// 	short		errorMagnitude2D	 = 0;
// 	short		errorMagnitudeNorm2D = 0;
// 	float		errorTheta			 = 0.0f;
// 	float		velMagnitude		 = 0.0f;
// 	float		velHeading			 = 0.0f;
// 	short		area				 = 0;
// 	cv::Vec3d	rotationVector		 = cv::Vec3d( 0, 0, 0 );
// 	cv::Vec3d	translationVector	 = cv::Vec3d( 0, 0, 0 );
// 	cv::Point2f thetaMeasured		 = cv::Point2f( 0.0f, 0.0f );
// 	cv::Point2f thetaDotMeasured	 = cv::Point2f( 0.0f, 0.0f );

// 	KalmanFilter3D kf;
// };

// Shared system variable container
struct ManagedData {

	// System flags
	bool FLAG_MAIN_RUNNING		 = true;
	bool FLAG_FRAME_READY		 = false;
	bool FLAG_TAG_FOUND			 = false;
	bool FLAG_AMPLIFIERS_ENABLED = false;
	bool FLAG_PACKET_WAITING	 = false;
	bool FLAG_LOGGING_ENABLED	 = false;
	bool FLAG_LOGGING_STARTED	 = false;
	bool FLAG_SERIAL0_OPEN		 = false;
	bool FLAG_SERIAL1_OPEN		 = false;
	bool FLAG_SERIAL0_ENABLED	 = false;
	bool FLAG_SERIAL1_ENABLED	 = false;
	bool FLAG_SHUTTING_DOWN		 = false;

	// Target information
	bool					 FLAG_TARGET_FOUND		   = false;
	bool					 FLAG_TARGET_SWITCHED	   = false;																						// Updated in ArucoClass
	int						 targetActiveID			   = 1;																							// ID of target currently being tracked
	cv::Point2i				 targetScreenPosition	   = cv::Point2i( 0, 0 );																		// Position of target in screen space, ArucoClass
	float					 targetScreenMagnitude	   = 0.0f;																						// Magnitude of screen error, ArucoClass
	cv::Point3f				 targetPosition3dRaw	   = cv::Point3f( 0.0f, 0.0f, 0.0f );															// [mm] Raw (unfiltered) position of tag relative to camera
	cv::Point3f				 targetPosition3dOld	   = cv::Point3f( 0.0f, 0.0f, 0.0f );															// [mm] Old position of tag relative to camera
	cv::Point3f				 targetPosition3dNew	   = cv::Point3f( 0.0f, 0.0f, 0.0f );															// [mm] New position of tag relative to camera
	cv::Point3f				 targetVelocity3dOld	   = cv::Point3f( 0.0f, 0.0f, 0.0f );															// [mm] Old position of tag relative to camera
	cv::Point3f				 targetVelocity3dNew	   = cv::Point3f( 0.0f, 0.0f, 0.0f );															// [mm] New position of tag relative to camera
	cv::Point2f				 targetAngleOld			   = cv::Point2f( 0.0f, 0.0f );																	// [rad] Old angle
	cv::Point2f				 targetAngleNew			   = cv::Point2f( 0.0f, 0.0f );																	// [rad] New angle
	cv::Point2f				 targetAnglularVelocityOld = cv::Point2f( 0.0f, 0.0f );																	// [rad/s] Old angular velocity
	cv::Point2f				 targetAnglularVelocityNew = cv::Point2f( 0.0f, 0.0f );																	// [rad/s] New angular velocity
	std::vector<cv::Point2i> targetCorners			   = { cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ) };	// Target corners in pixel space
	cv::Vec3d				 targetRotationVector, targetTranslationVector;																			// Rotation and translation vectors for active marker


	// cv::Point3f targetPositionError3dPrev = cv::Point3f( 0.0f, 0.0f, 0.0f );	// [mm] Previous position of tag relative to camera
	// cv::Point2i targetPositionError2d	  = cv::Point2i( 0, 0 );				// [px] Position of tag relative to camera
	// cv::Point2f targetThetaErrorNew		  = cv::Point2f( 0.0f, 0.0f );			// [rad] New angle of error relative to camera principal axis
	// cv::Point2f targetThetaErrorPrev	  = cv::Point2f( 0.0f, 0.0f );			// [rad] Previous angle of error relative to camera principal axis
	// cv::Point2f targetThetaDotErrorNew	  = cv::Point2f( 0.0f, 0.0f );			// [rad] New angle of error relative to camera principal axis
	// cv::Point2f targetThetaDotErrorPrev	  = cv::Point2f( 0.0f, 0.0f );			// [rad] Previous angle of error relative to camera principal axis

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


	short arucoMinArea = 150;
	short arucoMaxArea = 30000;

	// Timing variables
	short timingFrequency = 45;
	float timingTimestamp = 0.0f;
	float timingTimestep  = 0.0f;

	// Touchscreen variables
	cv::Point3i touchPosition = cv::Point3i( 0, 0, 0 );
	bool		touchDetected = false;

	// Serial variables
	std::string serialPacket0	= "";
	std::string serialPacket1	= "";
	uint8_t		serialPortsOpen = 0;



	// Teensy variables
	bool		teensySerialResponding = false;
	bool		teensyAmplifierEnabled = false;
	cv::Point3i teensyABC			   = cv::Point3i( 0, 0, 0 );


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
	bool		FLAG_CONTROLLER_ACTIVE	 = false;
	cv::Point2f controllerKp			 = cv::Point2f( 10.0f, 10.0f );
	cv::Point2f controllerKd			 = cv::Point2f( 0.0f, .0f );
	cv::Point3i controllerPWM			 = cv::Point3i( 0, 0, 0 );
	cv::Point3f controllerPercentage	 = cv::Point3f( 0.0f, 0.0f, 0.0f );
	cv::Point2f controllerXYProportional = cv::Point2f( 0.0f, 0.0f );
	cv::Point2f controllerXYDerivative	 = cv::Point2f( 0.0f, 0.0f );
	cv::Point2f controllerXYTotal		 = cv::Point2f( 0.0f, 0.0f );
	cv::Point3f controllerTorqueABC		 = cv::Point3f( 0.0f, 0.0f, 0.0f );
	cv::Point3f controllerCurrent		 = cv::Point3f( 0.0f, 0.0f, 0.0f );
	cv::Point3f controllerTension		 = cv::Point3f( 0.0f, 0.0f, 0.0f );

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

	// Kalman filter
	float kalmanProcessNoiseCovarianceQ = 0.1f;		// 0.01 Higher Q = more trust in model, faster response, less lag
	float kalmanMeasurementNoiseR		= 0.5f;		// 10.0 [mm^2] Higher R means less trust in model, smoother but more lag
	float kalmanTimeStepDt				= 0.05f;	// 0.01 Minimum time step

	// Helper functions
	float		GetNorm2D( cv::Point2f pt1 );					   // Calculate magnitude of 2D vector
	float		GetNorm3D( cv::Point3f pt1 );					   // Calculate magnitude of 3D vector
	float		GetDist2D( cv::Point2f pt1, cv::Point2f pt2 );	   // Calculate magnitude of 2D distance between 2 points
	float		GetDist3D( cv::Point3f pt1, cv::Point3f pt2 );	   // Calculate magnitude of 2D distance between 2 points
	cv::Point2f GetDelta2D( cv::Point2f pt1, cv::Point2f pt2 );	   // Calculate the x,y delta beteen two points
	cv::Point3f GetDelta3D( cv::Point3f pt1, cv::Point3f pt2 );	   // Calculate the x,y,z delta beteen two points
	std::string FormatDecimal( float x, uint8_t d );						   // Formats a float to xx.xxx
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