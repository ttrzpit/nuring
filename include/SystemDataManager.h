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

// Shared system variable container
struct ManagedData {

	// System flags
	bool FLAG_AMPLIFIERS_ENABLED	   = false;
	bool FLAG_CONTROLLER_ACTIVE		   = false;
	bool FLAG_FRAME_READY			   = false;
	bool FLAG_LOGGING_ENABLED		   = false;
	bool FLAG_LOGGING_STARTED		   = false;
	bool FLAG_MAIN_RUNNING			   = true;
	bool FLAG_PACKET_WAITING		   = false;
	bool FLAG_SERIAL0_OPEN			   = false;
	bool FLAG_SERIAL0_ENABLED		   = false;
	bool FLAG_SERIAL1_OPEN			   = false;
	bool FLAG_SERIAL1_ENABLED		   = false;
	bool FLAG_SHUTTING_DOWN			   = false;
	bool FLAG_TARGET_MARKER_FOUND	   = false;
	bool FLAG_TEENSY_SERIAL_RESPONDING = false;
	bool FLAG_TEENSY_AMPLIFIER_ENABLED = false;
	bool FLAG_USE_FINGER_MARKER		   = false;
	bool FLAG_TARGET_RESET			   = false;
	bool FLAG_USE_FIXED_MARKER		   = false;
	// int	 lostCount					   = 0;
	// Target marker
	// bool					 FLAG_TARGET_MARKER_SWITCHED	 = false;																					  // Updated in ArucoClass
	int						 targetMarkerActiveID			 = 1;																						  // ID of target currently being tracked
	cv::Point2i				 targetMarkerScreenPosition		 = cv::Point2i( 0, 0 );																		  // Position of target in screen space, ArucoClass
	cv::Point3f				 targetMarkerPosition3dRaw		 = cv::Point3f( 0.0f, 0.0f, 0.0f );															  // [mm] Raw (unfiltered) position of tag relative to camera
	cv::Point3f				 targetMarkerPosition3dOld		 = cv::Point3f( 0.0f, 0.0f, 0.0f );															  // [mm] Old position of tag relative to camera
	cv::Point3f				 targetMarkerPosition3dNew		 = cv::Point3f( 0.0f, 0.0f, 0.0f );															  // [mm] New position of tag relative to camera
	cv::Point3f				 targetMarkerPosition3dPredicted = cv::Point3f( 0.0f, 0.0f, 0.0f );															  // [mm] New position of tag relative to camera
	cv::Point3f				 targetMarkerVelocity3dOld		 = cv::Point3f( 0.0f, 0.0f, 0.0f );															  // [mm] Old position of tag relative to camera
	cv::Point3f				 targetMarkerVelocity3dNew		 = cv::Point3f( 0.0f, 0.0f, 0.0f );															  // [mm] New position of tag relative to camera
	cv::Point3f				 targetMarkerIntegralError		 = cv::Point3f( 0.0f, 0.0f, 0.0f );															  // [mm] Integral error
	cv::Point2f				 targetMarkerAngleOld			 = cv::Point2f( 0.0f, 0.0f );																  // [rad] Old angle
	cv::Point2f				 targetMarkerAngleNew			 = cv::Point2f( 0.0f, 0.0f );																  // [rad] New angle
	cv::Point2f				 targetMarkerAnglularVelocityOld = cv::Point2f( 0.0f, 0.0f );																  // [rad/s] Old angular velocity
	cv::Point2f				 targetMarkerAnglularVelocityNew = cv::Point2f( 0.0f, 0.0f );																  // [rad/s] New angular velocity
	std::vector<cv::Point2i> targetMarkerCorners			 = { cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ) };	  // Target corners in pixel space
	cv::Vec3d				 targetMarkerRotationVector, targetMarkerTranslationVector;																	  // Rotation and translation vectors for active marker

	// Controller variables
	cv::Point3f controllerKp			  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Proportional gain
	cv::Point3f controllerKd			  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Derivative gain
	cv::Point3f controllerKi			  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Integral gain
	cv::Point3i controllerPWM			  = cv::Point3i( 0, 0, 0 );				// Motor output PWM value
	cv::Point3f controllerPercentage	  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Motor output percentage
	cv::Point3f controllerProportinalTerm = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Placeholder for proportional term
	cv::Point3f controllerDerivativeTerm  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Placeholder for derivative term
	cv::Point3f controllerIntegralTerm	  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Placeholder for integral term
	cv::Point3f controllerTotalTerm		  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Placeholder for total term
	cv::Point3f controllerTorqueABC		  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Torque
	cv::Point3f controllerCurrent		  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Current
	cv::Point3f controllerTension		  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Tension (in PWM units)
	float		controllerRampLimit		  = 0.05f;								// Maximum change per update

	// Kalman filter
	float	kalmanProcessNoiseCovarianceQ = 0.00001f;	 // 0.01 Higher Q = more trust in model, faster response, less lag
	float	kalmanMeasurementNoiseR		  = 1.0e-2f;	 // 0.5f;		// 10.0 [mm^2] Higher R means less trust in model, smoother but more lag
	float	kalmanTimeStepDt			  = 0.01f;		 // 0.01 Minimum time step
	cv::Mat kalmanP;									 // Covariance


	// Finger marker
	bool					 FLAG_FINGER_MARKER_FOUND  = false;
	cv::Point3i				 fingerMarkerPosition3DRaw = cv::Point3i( 0, 0, 0 );
	cv::Point3i				 fingerMarkerPosition3DNew = cv::Point3i( 0, 0, 0 );
	std::vector<cv::Point2i> fingerMarkerCorners	   = { cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ) };	// Corners of finger tag
	cv::Vec3d				 fingerMarkerRotationVector, fingerMarkerTranslationVector;																// Rotation and translation vectors for active marker
	cv::Point2i				 fingerMarkerScreenPosition = cv::Point2i( 0, 0 );																		// Position of target in screen space, ArucoClass
	cv::Point2f				 fingerMarkerAngleNew		= cv::Point2f( 0.0f, 0.0f );																// [rad] New angle

	// Tasks
	std::string TASK_NAME		= "";		// String name of task
	uint8_t		TASK_USER_ID	= 000;		// Participant ID
	int			TASK_REP_NUMBER = 0;		// Task rep number
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
	cv::Point3i teensyMeasuredAmplfierOutput = cv::Point3i( 0, 0, 0 );

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
	std::string loggingTimestamp = "";

	// Task variables
	cv::Point3i fittsErrorPx		= cv::Point3i( 0, 0, 0 );
	cv::Point3i fittsErrorMm		= cv::Point3i( 0, 0, 0 );
	float		fittsCompletionTime = 0.0f;
	short		fittsTestNumber		= 0;
	cv::Point2i fittsMarkerPosition = cv::Point2i( 0, 0 );
	char		fittsActiveAxis		= 'z';

	// 3D Visualization
	bool vizClear	= false;
	bool vizEnabled = false;
	bool vizLoaded	= false;

	// 2D Angle Visualization
	bool  angleEnabled	= false;
	bool  angleLoaded	= false;
	float angleTheta	= 0.0f;
	float angleDesired	= 0.0f;
	float angleFiltered = 0.0f;
	float angleVelocity = 0.0f;

	// Serial triggers
	std::string serialTrigger = "drive";

	// Calibration
	bool		calibrationComplete = false;
	cv::Point3i calibrationOffsetMM = cv::Point3i( 0, 0, 0 );
	cv::Point3i calibrationOffsetPX = cv::Point3i( 0, 0, 0 );
	cv::Point2i calibrationScreenPX = cv::Point2i( 0, 0 );
	bool		calibrationLoaded	= false;



	// Helper functions
	float		GetNorm2D( cv::Point2f pt1 );					   // Calculate magnitude of 2D vector
	float		GetNorm3D( cv::Point3f pt1 );					   // Calculate magnitude of 3D vector
	float		GetDist2D( cv::Point2f pt1, cv::Point2f pt2 );	   // Calculate magnitude of 2D distance between 2 points
	float		GetDist3D( cv::Point3f pt1, cv::Point3f pt2 );	   // Calculate magnitude of 2D distance between 2 points
	cv::Point2f GetDelta2D( cv::Point2f pt1, cv::Point2f pt2 );	   // Calculate the x,y delta beteen two points
	cv::Point3f GetDelta3D( cv::Point3f pt1, cv::Point3f pt2 );	   // Calculate the x,y,z delta beteen two points
	std::string FormatDecimal( float x, uint8_t p, uint8_t d );	   // Formats a float to xx.xxx
	std::string PadValues( int val, int nZeroes );				   // Return a padded string
	// int			MapInt( int x, int inMin, int inMax, int outMin, int outMax );					// Return a remapped integer
	float MapFloat( float val, float inMin, float inMax, float outMin, float outMax );	  // Return a remapped float
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