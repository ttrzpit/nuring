// Library for managing interrupt signals
#include <csignal>

// Data manager for exclusive read/write
#include "include/SystemDataManager.h"

// System data manager object handle
SystemDataManager dataHandle;

// Handle to shared data
auto shared = dataHandle.getData();

// Custom class objects
#include "include/ArucoClass.h"
#include "include/CalibrationClass.h"
#include "include/CaptureClass.h"
#include "include/ControllerClass.h"
#include "include/DisplayClass.h"
#include "include/FittsClass.h"
#include "include/InputClass.h"
#include "include/KalmanClass.h"
#include "include/LoggingClass.h"
#include "include/SerialClass.h"
#include "include/TimingClass.h"
#include "include/TouchscreenClass.h"

// New class objects
CaptureClass	 Capture( dataHandle );					  // Camera capture
ArucoClass		 Aruco( dataHandle );					  // Aruco detector
DisplayClass	 Canvas( dataHandle );					  // Display output
InputClass		 Input( dataHandle );					  // Keyboard input
TimingClass		 Timing( dataHandle );					  // Loop timing measurement
TouchscreenClass Touch( dataHandle );					  // Touchscreen position reading
SerialClass		 Serial( dataHandle, 2 );				  // Serial interface
LoggingClass	 Logging( dataHandle );					  // Logging interface
FittsClass		 Fitts( dataHandle, Timing, Logging );	  // For fitts-law testing
CalibrationClass Calibration( dataHandle );				  // For calibration of user to touchscreen
ControllerClass	 Controller( dataHandle );				  // Controller
KalmanClass		 Kalman( dataHandle );					  // Kalman filter


// Function prototypes
void		SignalHandler( int signum );
void		TaskCalibrate();
void		TaskFitts();
std::string BuildPacketAngularError();
void		UpdateState();
void		SendSerialPacket();
void		BuildSerialPacket();
/**
 * @brief Main program loop
 * 
 * @return int 
 */
int main() {

	// Settings
	shared->FLAG_LOGGING_ENABLED = false;
	shared->TASK_USER_ID		 = 000;
	shared->targetActiveID		 = 1;

	// Optimization for C/C++ data types
	std::ios_base::sync_with_stdio( false );
	std::cout.setf( std::ios::unitbuf );

	// For debugging
	// std::cout << "OpenCV Build: " << cv::getBuildInformation() << std::endl;
	// cv::utils::logging::setLogLevel( cv::utils::logging::LOG_LEVEL_VERBOSE );

	// Status update
	std::cout << "\nMain:         Program running...\n\n";

	// Initialize controller
	// Controller.Initialize();

	// Start timer for measuring loop frequency
	Timing.StartTimer();

	// Add shortcut panel
	Canvas.ShowShortcuts();

	// Send initial command
	Serial.Send( "E0A2048B2048C2048X" );

	// Initialize kalman filter
	Kalman.Initialize( cv::Point3f( 0.0f, 0.0f, 0.0f ), shared->timingTimestamp );

	// Main loop
	while ( shared->FLAG_MAIN_RUNNING ) {

		// Update timer (for measuring loop frequency)
		Timing.UpdateTimer();

		// Capture frame
		Capture.GetFrame();

		// Detect ArUco tags
		Aruco.FindTags();

		// Update system state
		if ( shared->FLAG_TARGET_FOUND ) {
			UpdateState();
		}

		// Task selector
		if ( shared->TASK_NAME == "FITTS" ) {
			TaskFitts();
		} else if ( shared->TASK_NAME == "CALIBRATE" ) {
			TaskCalibrate();
		}

		// Update controller
		Controller.Update();

		// Parse any input and use OpenCV WaitKey()
		Input.ParseInput( cv::pollKey() & 0xFF );

		// Check touchscreen input
		Touch.GetCursorPosition();

		// Build serial packet
		BuildSerialPacket();

		// Send serial packet
		if ( shared->FLAG_SERIAL0_OPEN && shared->FLAG_SERIAL0_ENABLED ) {
			Serial.Send( shared->serialPacket0 );
			// std::cout << shared->serialPacket0 << "\n";
		}

		// Check for new incoming serial packet
		if ( shared->FLAG_SERIAL1_OPEN && shared->FLAG_SERIAL1_ENABLED ) {
			Serial.CheckForPacket();
		}

		// Update display
		Canvas.Update();

		// Save logging file
		if ( shared->TASK_COMPLETE ) {
			Logging.Save();
			std::cout << "File saved!\n";
			shared->TASK_COMPLETE = false;
		}

		if ( shared->FLAG_SHUTTING_DOWN ) {
			shared->serialPacket0		= "DX\n";
			shared->FLAG_PACKET_WAITING = true;
			shared->FLAG_MAIN_RUNNING	= false;
		}

		// std::cout << shared->touchPosition.x << " , " << shared->touchPosition.y << " | " << shared->touchPosition.z << "\n";
	}
	cv::destroyAllWindows();
	return 0;
}



/**
 * @brief Shutdown safely if error occurs
 */
void SignalHandler( int signum ) {
	std::cout << "\nMain:         Interrupt signal (" << signum << ") received.\n";
	shared->FLAG_MAIN_RUNNING = false;
	Touch.Close();
	exit( signum );
}

/**
 * @brief Run calibration routine
 */
void TaskCalibrate() {


	// Initialize logging file and update entries
	if ( shared->FLAG_LOGGING_ENABLED && !shared->FLAG_LOGGING_STARTED && shared->TASK_RUNNING ) {

		// Customize header
		shared->loggingHeader1 = "UserOffsetX";
		shared->loggingHeader2 = "UserOffsetY";
		shared->loggingHeader3 = "UserOffsetZ";

		// Initialize and add initial entry
		Logging.Initialize();
		Logging.AddEntry();
		shared->FLAG_LOGGING_STARTED = true;
	}

	if ( !shared->TASK_RUNNING ) {
		shared->calibrationComplete = false;
		shared->calibrationOffsetMM = cv::Point3i( 0, 0, 0 );
		shared->TASK_REP_NUMBER		= 0;
		shared->TASK_RUNNING		= true;
		shared->targetActiveID		= 8;
		Calibration.InitializeCalibration();
		Calibration.StartCalibration();
	} else {

		// Update calibration
		Calibration.Update();

		// Update
		shared->loggingVariable1 = std::to_string( shared->calibrationOffsetMM.x );
		shared->loggingVariable2 = std::to_string( shared->calibrationOffsetMM.y );
		shared->loggingVariable3 = std::to_string( shared->calibrationOffsetMM.z );
		Logging.AddEntry();
	}
}


/**
 * @brief Start fitts law testing
 */
void TaskFitts() {

	// Initialize logging file and update entries
	if ( shared->FLAG_LOGGING_ENABLED && !shared->FLAG_LOGGING_STARTED && shared->TASK_RUNNING ) {

		// Customize header
		shared->loggingHeader1 = "MarkerPresent";
		shared->loggingHeader2 = "Touched";
		shared->loggingHeader3 = "Encoder";

		// Initialize and add initial entry
		Logging.Initialize();
		Logging.AddEntry();
		shared->FLAG_LOGGING_STARTED = true;

	} else if ( shared->FLAG_LOGGING_ENABLED && shared->FLAG_LOGGING_STARTED ) {

		shared->loggingVariable1 = std::to_string( shared->FLAG_TARGET_FOUND );
		shared->loggingVariable2 = std::to_string( shared->touchDetected );
		shared->loggingVariable3 = shared->serialPacket1;
		Logging.AddEntry();
	}

	// Run task
	if ( !shared->TASK_RUNNING ) {
		shared->TASK_RUNNING   = true;
		shared->TASK_COMPLETE  = false;
		shared->targetActiveID = 1;
		Fitts.StartTest( 'x' );
	} else {
		Fitts.Update();
	}
}


std::string BuildPacketAngularError() {

	// Calculate angular error
	// int8_t angX = std::clamp( int( RAD2DEG * atan2( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.z ) ), -45, 45 );
	// int8_t angY = std::clamp( int( RAD2DEG * atan2( shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z ) ), -45, 45 );

	// // Get boolean for sign
	// int8_t signX = Serial.Sign( angX );
	// int8_t signY = Serial.Sign( angY );

	// // Build string
	// std::string output = "Ax" + Serial.PadValues( signX, 1 ) + Serial.PadValues( abs( angX ), 2 ) + "y" + Serial.PadValues( signY, 1 ) + Serial.PadValues( abs( angY ), 2 ) + "X";

	// return output;

	// // shared->serialPacket0 = "Ex" + std::to_string( int( RAD2DEG * atan2( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.z ) ) ) + "y"
	// // 	+ std::to_string( int( RAD2DEG * atan2( shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z ) ) ) + "X\n";

	return "";
}


/**
 * @brief Update system state using kalman filter
 * 
 */
void UpdateState() {

	// Save old data
	shared->targetPosition3dOld		  = shared->targetPosition3dNew;
	shared->targetVelocity3dOld		  = shared->targetVelocity3dNew;
	shared->targetAngleOld			  = shared->targetAngleNew;
	shared->targetAnglularVelocityOld = shared->targetAnglularVelocityNew;

	// Update kalman filter
	Kalman.Update( shared->targetPosition3dRaw, shared->timingTimestamp );

	// // Get updated state values
	shared->targetPosition3dNew		  = Kalman.GetPosition();
	shared->targetVelocity3dNew		  = Kalman.GetVelocity();
	shared->targetAngleNew			  = Kalman.GetAngle();
	shared->targetAnglularVelocityNew = Kalman.GetAnglularVelocity();
}



void BuildSerialPacket() {

	// Build string
	shared->serialPacket0 = std::string( "E" ) + ( shared->FLAG_AMPLIFIERS_ENABLED ? "1" : "0" ) + "A" + Serial.PadValues( shared->controllerPWM.x, 4 ) + "B" + Serial.PadValues( shared->controllerPWM.y, 4 ) + "C" + Serial.PadValues( shared->controllerPWM.z, 4 ) + "X";
}