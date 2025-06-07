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

// Packet
#include "PacketTypes.h"

// Constants
#define RAD2DEG 57.2958
#define DEG2RAD 0.01745

// Enum for system state
enum class stateEnum { IDLE, DRIVING_PWM, MEASURING_LIMITS, MEASURING_CURRENT };


// Structure for 4-point vector
struct Point4f {
	float abb  = 0.0f;
	float add  = 0.0f;
	float flex = 0.0f;
	float ext  = 0.0f;

	// // Constructor
	// Point4f( float a, float d, float f, float e )
	// 	: abb( a )
	// 	, add( d )
	// 	, flex( f )
	// 	, ext( e ) { }
};



struct AmplifierStruct {

	// Flags
	bool isAmplifierActive = false;

	// Measured balues
	uint8_t	 packetType		  = 0;
	uint8_t	 packetCounter	  = 0;
	uint8_t	 packetDriveState = 0;
	uint16_t measuredPwmA	  = 0;
	uint16_t measuredPwmB	  = 0;
	uint16_t measuredPwmC	  = 0;
	int16_t	 measuredCurrentA = 0;
	int16_t	 measuredCurrentB = 0;
	int16_t	 measuredCurrentC = 0;
	int32_t	 measuredEncoderA = 0;
	int32_t	 measuredEncoderB = 0;
	int32_t	 measuredEncoderC = 0;
};

struct ArUcoStruct {

	// bool isArUcoTagFound = false;
};

struct CaptureStruct {
	bool	isFrameReady = false;
	cv::Mat frameRaw	 = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::Mat frameGray	 = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );

	// OpenCV GPU matrices
	cv::cuda::GpuMat GpuMatFrameRaw			= cv::cuda::GpuMat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::cuda::GpuMat GpuMatFrameGray		= cv::cuda::GpuMat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC1 );
	cv::cuda::GpuMat GpuMatFrameUndistorted = cv::cuda::GpuMat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC1 );
	cv::cuda::GpuMat GpuMatRemap1;
	cv::cuda::GpuMat GpuMatRemap2;

	// OpenCV image matrices
	cv::Mat matFrameUndistorted = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::Mat matRemap1;
	cv::Mat matRemap2;
};

struct ControllerStruct {

	Point4f		gainKp;
	Point4f		gainKi;
	Point4f		gainKd;
	cv::Point3f proportionalTerm		 = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Combined proportional term
	cv::Point3f integralTerm			 = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Combined integral
	cv::Point3f derivativeTerm			 = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Combined derivative term
	cv::Point3f combinedPIDTerms		 = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Combined PID terms
	cv::Point3f combinedPIDTermPrev		 = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Combined PID terms
	cv::Point3i commandedPwmABC			 = cv::Point3i( 0, 0, 0 );			   // Commanded PWM output
	cv::Point3f commandedPercentage		 = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Commanded percentage output
	cv::Point3f commandedPercentageLimit = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Max percentage output
	cv::Point3f commandedCurrentABC		 = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Commanded current output
	cv::Point3f commandedTensionABC		 = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Commanded tension
	cv::Point3f torqueABC				 = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Commanded torque
	float		rampPercentage			 = 0.00f;							   // Counter
	float		rampStartTime			 = 0.0f;							   // [s]
	float		rampDurationTime		 = 1.0f;							   // [s]
	bool		isRampingUp				 = false;							   // Is the motor ramping up?
	bool		isLimitSet				 = false;							   // Are the motor limits set?
	bool		isCalibrated			 = false;							   // Has the finger been calibrated?
	cv::Point3i calibratedOffetMM		 = cv::Point3i( 0, 0, 0 );
};

struct DisplayStruct {

	// Matrix to display overlay
	cv::Mat		matFrameOverlay = cv::Mat( CONFIG_DIS_HEIGHT, CONFIG_DIS_WIDTH, CV_8UC3 );
	std::string statusString	= "";
};

struct KalmanFilterStruct {
	cv::Mat pMatrix;
};

struct LoggingStruct {
	bool isLoggingActivelyRunning = false;	  // Is logging actively running
	bool isLoggingEnabled		  = false;	  // Is logging enabled
};


struct SystemStruct {

	// Flags
	bool isMainRunning	= true;		// Global flag to shut-down safely
	bool isShuttingDown = false;	// Trigger safe shutdown

	// State enum
	stateEnum state = stateEnum::IDLE;
};

struct SerialStruct {

	// Flags
	bool isSerialSendOpen	 = false;
	bool isSerialSending	 = false;
	bool isSerialReceiveOpen = false;
	bool isSerialReceiving	 = false;

	// Plaintext packet
	std::string packetOut = "";
	std::string packetIn  = "";
};

struct TelemetryStruct {
	int						 activeID			   = 1;	   // ID of target currently being tracked
	bool					 isTargetReset		   = false;
	bool					 isTargetFound		   = false;
	cv::Point2i				 screenPositionPX	   = cv::Point2i( 0, 0 );																		// [px] Position of target in screen space
	cv::Point3f				 positionUnfilteredMM  = cv::Point3f( 0.0f, 0.0f, 0.0f );															// [mm] Raw (unfiltered) position of tag relative to camera
	cv::Point3f				 positionFilteredNewMM = cv::Point3f( 0.0f, 0.0f, 0.0f );															// [mm] New 3D filtered position
	cv::Point3f				 positionFilteredOldMM = cv::Point3f( 0.0f, 0.0f, 0.0f );															// [mm] Old 3D filtered position
	cv::Point3f				 velocityFilteredNewMM = cv::Point3f( 0.0f, 0.0f, 0.0f );															// [mm] New 3D filtered position
	cv::Point3f				 velocityFilteredOldMM = cv::Point3f( 0.0f, 0.0f, 0.0f );															// [mm] New 3D filtered position
	std::vector<cv::Point2i> cornersPX			   = { cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ) };	// Target corners in pixel space
	cv::Point3f				 positionIntegratedMM  = cv::Point3f( 0.0f, 0.0f, 0.0f );
};

struct TaskStruct {
	std::string name			 = "";		 // String name of task
	float		runningTime		 = 0.0f;	 // Task running time
	int			userID			 = 000;		 // Participant ID
	int			repetitionNumber = 0;		 // Task rep number
	char		command			 = 0;		 // Command
	bool		isRunning		 = false;	 // Is the task running
	bool		isComplete		 = false;	 // Trip when task is complete
};


struct TeensyStruct {
	bool		isTeensyResponding	   = false;
	bool		isAmplifierResponding  = false;
	cv::Point3i measuredAmplfierOutput = cv::Point3i( 0, 0, 0 );
};

struct TimingStruct {

	short measuredFrequency	 = 45;		// Default timing frequency
	float elapsedRunningTime = 0.0f;	// Elapsed time in seconds
	float taskTimer			 = 0.0f;	// Elapsed time for current task
	float timestepDT		 = 0.0f;
};

struct TouchscreenStruct {

	cv::Point3i positionTouched = cv::Point3i( 0, 0, 0 );
	bool		isTouched		= false;
};


// Shared system variable container
struct ManagedData {


	// Structs
	AmplifierStruct	   Amplifier;
	ArUcoStruct		   Aruco;
	CaptureStruct	   Capture;
	ControllerStruct   Controller;
	DisplayStruct	   Display;
	KalmanFilterStruct KalmanFilter;
	LoggingStruct	   Logging;
	SystemStruct	   System;
	SerialStruct	   Serial;
	TelemetryStruct	   Telemetry;
	TaskStruct		   Task;
	TeensyStruct	   Teensy;
	TimingStruct	   Timing;
	TouchscreenStruct  Touchscreen;



	// Teensy variables


	// Display variables


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



	// int	 lostCount					   = 0;
	// Target marker
	// bool					 FLAG_TARGET_MARKER_SWITCHED	 = false;	// Updated in ArucoClass
	// int						 targetMarkerActiveID			 = 1;		// Target.activeID
	// cv::Point2i				 targetMarkerScreenPosition		 = cv::Point2i( 0, 0 );		// Target.screenPositionPX
	// cv::Point3f				 targetMarkerPosition3dRaw		 = cv::Point3f( 0.0f, 0.0f, 0.0f );		// Target.rawPositionMM													  // Target.rawPositionMM	// [mm] Raw (unfiltered) position of tag relative to camera
	// cv::Point3f				 targetMarkerPosition3dOld		 = cv::Point3f( 0.0f, 0.0f, 0.0f );		// Target.positionOldMM													  // Target.PositionOldMM												  // [mm] Old position of tag relative to camera
	// cv::Point3f				 targetMarkerPosition3dNew		 = cv::Point3f( 0.0f, 0.0f, 0.0f );		// Target.positionFilteredNewMM													  // Target.filteredPositionMM
	// cv::Point3f				 targetMarkerPosition3dPredicted = cv::Point3f( 0.0f, 0.0f, 0.0f );		// Deleted
	// cv::Point3f				 targetMarkerVelocity3dOld		 = cv::Point3f( 0.0f, 0.0f, 0.0f );			// Target.velocityFilteredOldMM												  // Target.velocityFilteredOldMM													  // Target.filteredPositionMM
	// cv::Point3f				 targetMarkerVelocity3dNew		 = cv::Point3f( 0.0f, 0.0f, 0.0f );				// Target.velocityFilteredNewMM											  // Target.velocityFilteredNewMM
	// cv::Point3f				 targetMarkerIntegralError		 = cv::Point3f( 0.0f, 0.0f, 0.0f );															  // Controller.accumulatedIntegralError
	// cv::Point2f				 targetMarkerAngleOld			 = cv::Point2f( 0.0f, 0.0f );	// Deleted
	// cv::Point2f				 targetMarkerAngleNew			 = cv::Point2f( 0.0f, 0.0f );	// Deleted																  // [rad] New angle
	// cv::Point2f				 targetMarkerAnglularVelocityOld = cv::Point2f( 0.0f, 0.0f );																  // [rad/s] Old angular velocity
	// cv::Point2f				 targetMarkerAnglularVelocityNew = cv::Point2f( 0.0f, 0.0f );																  // [rad/s] New angular velocity
	// std::vector<cv::Point2i> targetMarkerCorners = { cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ) };	  // Target.cornersPX
	// cv::Vec3d targetMarkerRotationVector, targetMarkerTranslationVector;	// Target.rotationVector // Rotation and translation vectors for active marker

	// Controller variables
	cv::Point3f controllerKp = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Proportional gain
	cv::Point3f controllerKd = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Derivative gain
	cv::Point3f controllerKi = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Integral gain
	// cv::Point3i controllerPWM			  = cv::Point3i( 0, 0, 0 );		// Controller.commandedPwm				// Motor output PWM value
	// cv::Point3f controllerPercentage	  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Controller.commandedPercentage	// Motor output percentage
	cv::Point3f controllerProportinalTerm = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Placeholder for proportional term
	cv::Point3f controllerDerivativeTerm  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Placeholder for derivative term
	cv::Point3f controllerIntegralTerm	  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Placeholder for integral term
	cv::Point3f controllerTotalTerm		  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Placeholder for total term
	// cv::Point3f controllerTorqueABC		  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Controller.torqueABC
	// cv::Point3f controllerCurrent		  = cv::Point3f( 0.0f, 0.0f, 0.0f );	// Controller.currentABC
	// cv::Point3f controllerTension		= cv::Point3f( 0.0f, 0.0f, 0.0f );	  // Controller.commandedTensionABC 	Tension (in PWM units)
	// float controllerRampFactor	  = 0.0f;	  // Controller.rampPercentage
	// bool  controllerIsRamping	  = false;	  // Controller.isRampingUp
	// float controllerRampStartTime = 0.0f;	 // Controller.rampStartTime 	Ramp start time
	// float controllerRampDuration = 1.0f;	// // Controller.rampDurationTime 	How long to ramp up for



	// Kalman filter
	// cv::Mat kalmanP;									 // KalmanFilter.pMatrix
	// float	kalmanDt = 0.1f;


	// Finger marker
	// bool					 FLAG_FINGER_MARKER_FOUND  = false;
	// cv::Point3i				 fingerMarkerPosition3DRaw = cv::Point3i( 0, 0, 0 );
	// cv::Point3i				 fingerMarkerPosition3DNew = cv::Point3i( 0, 0, 0 );
	// std::vector<cv::Point2i> fingerMarkerCorners	   = { cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ) };	// Corners of finger tag
	// cv::Vec3d				 fingerMarkerRotationVector, fingerMarkerTranslationVector;																// Rotation and translation vectors for active marker
	// cv::Point2i				 fingerMarkerScreenPosition = cv::Point2i( 0, 0 );																		// Position of target in screen space, ArucoClass
	// cv::Point2f				 fingerMarkerAngleNew		= cv::Point2f( 0.0f, 0.0f );																// [rad] New angle

	// Tasks
	// std::string TASK_NAME		= "";		// Task.name
	// uint8_t TASK_USER_ID	= 000;		// Task.userID
	// int	 TASK_REP_NUMBER = 0;		 // Task.repetitionNumber	 Task rep number
	// char TASK_COMMAND	 = 0;		 // Task.command
	// bool TASK_RUNNING  = false;	   // Task.isRunning
	// bool TASK_COMPLETE = false;	   // Task.isComplete Trip when task is complete

	/** OLD VARIABLES */

	// System flags
	// bool FLAG_AMPLIFIERS_ENABLED	   = false;		// Amplifier.isAmplifierActive
	// bool FLAG_FRAME_READY			   = false;		// Camera.isFrameReady
	// bool FLAG_LOGGING_ENABLED		   = false;		// Logging.isLoggingEnabled
	// bool FLAG_LOGGING_STARTED		   = false;		// Logging.isLoggingActivelyRunning
	// bool FLAG_MAIN_RUNNING			   = true;		// Main.isMainRunning
	// bool FLAG_PACKET_WAITING		   	   = false;		// Deleted
	// bool FLAG_SERIAL0_OPEN			   = false;		// Serial.isSerialSendOpen
	// bool FLAG_SERIAL0_ENABLED = false;	  // Serial.isSerialSending
	// bool FLAG_SERIAL1_OPEN			   = false;	   // Serial.isSerialReceiveOpen
	// bool FLAG_SERIAL1_ENABLED		   = false;	   // Serial.isSerialReceiving
	// bool FLAG_SHUTTING_DOWN			   = false;	   // Main.isShuttingDown
	// bool FLAG_TARGET_MARKER_FOUND	   = false;		// Target.isTargetFound
	// bool FLAG_TEENSY_SERIAL_RESPONDING = false;		// Teensy.isTeensyResponding
	// bool FLAG_TEENSY_AMPLIFIER_ENABLED = false;		// Teensy.isAmplifierResponding
	bool FLAG_USE_FINGER_MARKER = false;
	// bool FLAG_TARGET_RESET		= false;		// Target.isTargetReset
	// bool FLAG_USE_FIXED_MARKER	= false;		// Deleted
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