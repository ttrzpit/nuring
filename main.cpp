// Test
// Library for managing interrupt signals
#include <csignal>

// Data manager for exclusive read/write
#include "include/SystemDataManager.h"

// Configuration files
#include "include/config.h"

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
#include "include/TasksClass.h"
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
KalmanClass		 Kalman( dataHandle );					  // Kalman filter for target
// KalmanClass		 KalmanFinger( dataHandle );			  // Kalman filter
// KalmanClass		 KalmanAngle( dataHandle );

// Function prototypes
void		SignalHandler( int signum );
void		TaskCalibrate();
void		TaskFitts();
void		TaskFittsAngle();
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
	shared->FLAG_LOGGING_ENABLED = true;
	shared->TASK_USER_ID		 = 000;
	shared->targetMarkerActiveID = 1;

	// Optimization for C/C++ data types
	std::ios_base::sync_with_stdio( false );
	std::cout.setf( std::ios::unitbuf );

	// For debugging
	// std::cout << "OpenCV Build: " << cv::getBuildInformation() << std::endl;
	// cv::utils::logging::setLogLevel( cv::utils::logging::LOG_LEVEL_VERBOSE );

	// Status update
	std::cout << "\nMain:         Program running...\n\n";

	// Start timer for measuring loop frequency
	Timing.StartTimer();

	// Add shortcut panel
	Canvas.ShowShortcuts();

	// Send initial command
	Serial.Send( "E0A2048B2048C2048X" );

	// Initialize kalman filter
	shared->kalmanP = cv::Mat::eye( 6, 6, CV_32F ) * 1.0f;
	// Kalman.Initialize( cv::Point3f( 0.0f, 0.0f, 0.0f ), shared->timingTimestamp );



	// Main loop
	while ( shared->FLAG_MAIN_RUNNING ) {

		// Update timer (for measuring loop frequency)
		Timing.UpdateTimer();

		// Parse any input and use OpenCV WaitKey()
		Input.ParseInput( cv::pollKey() & 0xFF );


		// Capture frame
		Capture.GetFrame();

		// Detect ArUco tags
		if ( shared->TASK_NAME != "FITTS_ANGLE" ) {
			Aruco.FindTags();
		}

		// Task selector
		if ( shared->TASK_NAME == "FITTS" ) {
			TaskFitts();
		} else if ( shared->TASK_NAME == "CALIB" ) {
			TaskCalibrate();
		} else if ( shared->TASK_NAME == "FITTS_ANGLE" ) {
			TaskFittsAngle();
		}


		// Update system state
		UpdateState();

		// Update controller
		Controller.Update();


		// Check touchscreen input
		Touch.GetCursorPosition();

		// Build serial packet
		BuildSerialPacket();

		// Send outgoing serial packet
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

		// Update shutdown flags for clean shutdown
		if ( shared->FLAG_SHUTTING_DOWN ) {
			shared->serialPacket0		= "DX\n";
			shared->FLAG_PACKET_WAITING = true;
			shared->FLAG_MAIN_RUNNING	= false;
		}
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

	// Task hasn't been started yet
	if ( !shared->TASK_RUNNING ) {

		// Initialize prior to starting logging
		if ( !shared->FLAG_LOGGING_STARTED ) {

			// Disable amplifiers for safety
			shared->FLAG_AMPLIFIERS_ENABLED = false;

			shared->targetMarkerActiveID = 8;

			// Customize logging header
			shared->loggingHeader1 = "UserOffsetX";
			shared->loggingHeader2 = "UserOffsetY";
			shared->loggingHeader3 = "UserOffsetZ";

			// Initialize needed items before starting task
			Timing.GetTimestamp();
			Logging.Initialize();

			// Update logging flag
			shared->FLAG_LOGGING_STARTED = true;



		} else {	// Logging started

			// Clear values
			shared->TASK_REP_NUMBER		= 0;
			shared->calibrationOffsetMM = cv::Point3i( 0, 0, 0 );

			// Initialize task
			Calibration.Initialize();

			// Update
			shared->TASK_RUNNING = true;
		}

	} else {	// Task running

		// Run task if not complete
		if ( shared->TASK_REP_NUMBER < 5 ) {

			Calibration.Update();

		} else if ( shared->TASK_REP_NUMBER == 5 ) {	// Task complete

			// End calibration and dave data
			Calibration.End();
			Calibration.Update();

		} else {

			Logging.AddEntry();
			Logging.Save();
			shared->TASK_NAME			 = "";
			shared->FLAG_LOGGING_STARTED = false;
			shared->TASK_RUNNING		 = false;
			shared->calibrationComplete	 = true;
			Calibration.Close();
		}
	}



	// // Initialize logging file and update entries
	// if ( shared->FLAG_LOGGING_ENABLED && !shared->FLAG_LOGGING_STARTED && shared->TASK_RUNNING ) {

	// 	// Customize header
	// 	shared->loggingHeader1 = "UserOffsetX";
	// 	shared->loggingHeader2 = "UserOffsetY";
	// 	shared->loggingHeader3 = "UserOffsetZ";

	// 	// Disable output for safety
	// 	shared->FLAG_AMPLIFIERS_ENABLED = false;
	// 	// shared->FLAG_SERIAL0_ENABLED	= false;
	// 	// shared->FLAG_SERIAL1_ENABLED	= false;

	// 	// Initialize and add initial entry
	// 	Timing.GetTimestamp();
	// 	Logging.Initialize();
	// 	shared->FLAG_LOGGING_STARTED = true;

	// } else if ( shared->FLAG_LOGGING_ENABLED && shared->FLAG_LOGGING_STARTED ) {
	// 	Logging.AddEntry();
	// }

	// if ( !shared->TASK_RUNNING ) {

	// 	shared->calibrationComplete = false;
	// 	shared->calibrationOffsetMM = cv::Point3i( 0, 0, 0 );
	// 	shared->TASK_REP_NUMBER		= 0;
	// 	shared->TASK_RUNNING		= true;
	// 	shared->targetActiveID		= 8;
	// 	Calibration.InitializeCalibration();
	// 	Calibration.StartCalibration();
	// } else {

	// 	// Update calibration
	// 	Calibration.Update();

	// 	// Update
	// 	// shared->loggingVariable1 = std::to_string( shared->calibrationOffsetMM.x );
	// 	// shared->loggingVariable2 = std::to_string( shared->calibrationOffsetMM.y );
	// 	// shared->loggingVariable3 = std::to_string( shared->calibrationOffsetMM.z );
	// 	// Logging.AddEntry();
	// }
}


/**
 * @brief Start fitts law testing
 */
void TaskFitts() {

	// Initialize logging file and update entries
	if ( shared->FLAG_LOGGING_ENABLED && !shared->FLAG_LOGGING_STARTED && shared->TASK_RUNNING ) {

		// Customize header
		shared->loggingHeader1 = "TouchDetected";
		shared->loggingHeader2 = "OutgoingPacket";
		shared->loggingHeader3 = "IncomingPacket";
		shared->loggingHeader4 = "Serial0Enabled,Serial0Open,Serial1Enabled,Serial1Open";
		shared->loggingHeader5 = "kpx,kpy,kix,kiy,kdx,kdy";

		// Initialize and add initial entry
		// Start timer for measuring loop frequency
		Timing.GetTimestamp();
		Logging.Initialize();
		Timing.StartTimer();
		// Logging.AddEntry();
		shared->FLAG_LOGGING_STARTED = true;

	} else if ( shared->FLAG_LOGGING_ENABLED && shared->FLAG_LOGGING_STARTED ) {

		shared->loggingVariable1 = std::to_string( shared->touchDetected );
		shared->loggingVariable2 = shared->serialPacket0.substr( 0, shared->serialPacket0.length() - 1 );
		shared->loggingVariable3 = shared->serialPacket1.substr( 0, shared->serialPacket1.length() - 1 );
		shared->loggingVariable4 = std::to_string( shared->FLAG_SERIAL0_ENABLED ) + "," + std::to_string( shared->FLAG_SERIAL0_OPEN ) + "," + std::to_string( shared->FLAG_SERIAL1_ENABLED ) + "," + std::to_string( shared->FLAG_SERIAL1_OPEN );
		shared->loggingVariable5 = shared->FormatDecimal( shared->controllerKp.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKp.y, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKi.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKi.y, 1, 1 ) + ","
			+ shared->FormatDecimal( shared->controllerKd.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKd.y, 1, 1 );
		Logging.AddEntry();
	}

	// std::cout << "0: " << shared->serialPacket0.substr( 0, shared->serialPacket0.length() ) << "  1: " << shared->serialPacket1.substr( 0, shared->serialPacket1.length() ) << "\n";

	// Run task
	if ( !shared->TASK_RUNNING ) {

		shared->TASK_RUNNING		 = true;
		shared->TASK_COMPLETE		 = false;
		shared->targetMarkerActiveID = 1;
		Fitts.Initialize();
		Fitts.StartTest( 'z' );	   // x , y , z, t , v, f
	} else {

		Fitts.Update();
	}
}



/**
 * @brief Start fitts law testing
 */
void TaskFittsAngle() {

	// Initialize logging file and update entries
	if ( shared->FLAG_LOGGING_ENABLED && !shared->FLAG_LOGGING_STARTED && shared->TASK_RUNNING ) {

		// Customize header
		shared->loggingHeader1 = "ThetaDesired";
		shared->loggingHeader2 = "ThetaMeasured";

		// Initialize and add initial entry
		Timing.GetTimestamp();
		Logging.Initialize();
		Logging.AddEntry();
		shared->FLAG_LOGGING_STARTED = true;

	} else if ( shared->FLAG_LOGGING_ENABLED && shared->FLAG_LOGGING_STARTED ) {

		shared->loggingVariable1 = std::to_string( shared->angleDesired );
		shared->loggingVariable2 = std::to_string( shared->angleTheta );
		Logging.AddEntry();
	}

	// Run task
	if ( !shared->TASK_RUNNING ) {

		shared->TASK_RUNNING		 = true;
		shared->TASK_COMPLETE		 = false;
		shared->targetMarkerActiveID = 1;
		shared->angleEnabled		 = !shared->angleEnabled;
		Fitts.StartAngleTest();
	} else {

		Canvas.UpdateAngle();
		Fitts.UpdateAngle();
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

	if ( shared->FLAG_TARGET_MARKER_FOUND && shared->FLAG_FRAME_READY ) {


		if ( cv::norm( cv::Point2f( shared->targetMarkerPosition3dRaw.x, shared->targetMarkerPosition3dRaw.y ) - cv::Point2f( shared->targetMarkerPosition3dOld.x, shared->targetMarkerPosition3dOld.y ) ) > 100.0f ) {
			shared->FLAG_TARGET_RESET = true;
			// shared->lostCount++;
		}


		// Save old data
		shared->targetMarkerPosition3dOld		= shared->targetMarkerPosition3dNew;
		shared->targetMarkerVelocity3dOld		= shared->targetMarkerVelocity3dNew;
		shared->targetMarkerAngleOld.x			= shared->angleTheta;
		shared->targetMarkerAnglularVelocityOld = shared->targetMarkerAnglularVelocityNew;

		// Update kalman filter
		if ( shared->calibrationComplete ) {
			Kalman.Update( shared->targetMarkerPosition3dRaw + cv::Point3f( shared->calibrationOffsetMM ), shared->timingTimestamp );
		} else {
			Kalman.Update( shared->targetMarkerPosition3dRaw, shared->timingTimestamp );
		}

		// Get updated state values
		shared->targetMarkerPosition3dNew		= Kalman.GetPosition();
		shared->targetMarkerVelocity3dNew		= Kalman.GetVelocity();
		shared->targetMarkerAngleNew			= Kalman.GetAngle();
		shared->targetMarkerAnglularVelocityNew = Kalman.GetAnglularVelocity();
		shared->targetMarkerIntegralError		= Kalman.GetIntegralError();

		// Get covariance from Kalman filter
		// shared->kalmanP = Kalman.GetCovariance();
	}
}



void BuildSerialPacket() {

	if ( shared->serialTrigger == "drive" ) {
		// Build string32
		shared->serialPacket0 = std::string( "E" ) + ( shared->FLAG_AMPLIFIERS_ENABLED ? "1" : "0" ) + "A" + shared->PadValues( shared->controllerPWM.x, 4 ) + "B" + shared->PadValues( shared->controllerPWM.y, 4 ) + "C" + shared->PadValues( shared->controllerPWM.z, 4 ) + "X";
	} else if ( shared->serialTrigger == "reset" ) {
		shared->serialPacket0 = "E0RX";
		shared->serialTrigger = "drive";
	}
}