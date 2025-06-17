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
#include "include/CaptureClass.h"
#include "include/ControllerClass.h"
#include "include/DisplayClass.h"
#include "include/InputClass.h"
#include "include/KalmanClass.h"
#include "include/LoggingClass.h"
#include "include/SerialClass.h"
#include "include/TasksClass.h"
#include "include/TimingClass.h"
#include "include/TouchscreenClass.h"

// New class objects
// New class objects
CaptureClass	 Capture( dataHandle );					  // Camera capture
ArucoClass		 Aruco( dataHandle );					  // Aruco detector
DisplayClass	 Canvas( dataHandle );					  // Display output
InputClass		 Input( dataHandle );					  // Keyboard input
TimingClass		 Timing( dataHandle );					  // Loop timing measurement
TouchscreenClass Touch( dataHandle );					  // Touchscreen position reading
SerialClass		 Serial( dataHandle, 2 );				  // Serial interface
LoggingClass	 Logging( dataHandle );					  // Logging interface
ControllerClass	 Controller( dataHandle );				  // Controller
KalmanClass		 Kalman( dataHandle );					  // Kalman filter
KalmanClass		 KalmanRing( dataHandle );				  // Kalman filter
TasksClass		 Tasks( dataHandle, Timing, Logging );	  // Tasks interface


// Function prototypes
void SignalHandler( int signum );
void UpdateSystem();
void SelectTask();
void UpdateState();
void PrintState();



/**
 * @brief Main program loop
 * 
 * @return int 
 */
int main() {

	// Testing
	shared->Controller.commandedPercentageLimit = cv::Point3f( 1.0f, 1.0f, 1.0f );
	shared->Controller.isLimitSet				= true;
	shared->Calibration.isCalibrated			= false;

	// Settings
	shared->Logging.isEnabled	 = true;
	shared->Task.userID			 = 626;
	shared->Target.isTargetFound = 1;

	shared->System.useRing = false;

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

	// Initialize kalman filter
	shared->KalmanFilter.pMatrix = cv::Mat::eye( 6, 6, CV_32F ) * 1.0f;

	// Set default system state
	shared->System.state = stateEnum::IDLE;

	// Main loop
	while ( shared->System.isMainRunning ) {

		// For debugging
		// PrintState();

		// Update timer (for measuring loop frequency)
		Timing.Update();

		// Parse any input and use OpenCV WaitKey()
		Input.ParseInput( cv::pollKey() & 0xFF );

		// Capture frame
		Capture.GetFrame();

		// Detect ArUco tags
		Aruco.FindTags();

		// Run the appropriate task
		SelectTask();

		// Update system
		UpdateSystem();

		// Update controller
		Controller.Update();
		Controller.UpdateAmplifier();
		Controller.UpdateVibrotactile();

		// Check touchscreen input
		Touch.GetCursorPosition();

		// Update serial messages
		Serial.Update();

		// Update display
		Canvas.Update();

		// Save logging file
		// if ( shared->Task.isComplete ) {
		// Logging.Save();
		// std::cout << "File saved!\n";
		// shared->Task.isComplete = false;
		// }
		//
		// Update shutdown flags for clean shutdown
		if ( shared->System.isShuttingDown ) {
			shared->System.isMainRunning = false;
		}
	}
	cv::destroyAllWindows();
	return 0;
}



/*
 *
 *  
 *
 *
 *
 *
 *
 *
 *  =========================================================================================
 *  ========================================================================================= 
 * 
 *   TTTTTTT   AAAA   SSSSS  KK   KK  SSSSS
 *     T      AA  AA SS      KK  KK  SS
 *     T      AA  AA  SS     KK KK    SS
 *     T     AAAAAAAA  SSSSS  KKKK     SSSSS
 *     T     AA    AA      SS KK KK         SS
 *     T     AA    AA      SS KK  KK        SS
 *     T     AA    AA  SSSSS  KK   KK  SSSSSS
 * 
 *  ========================================================================================= 
 *  ========================================================================================= 
 */



/**
 * @brief State machine for running tasks
 * 
 */
void SelectTask() {

	switch ( shared->Task.state ) {

		case taskEnum::IDLE: {

			// Do nothing
			break;
		}

		case taskEnum::FITTS: {

			/** Select how to generate marker
			 * 		x = random x position
			 * 		y = random y position
			 * 		z = random xy position
			 * 		v = constant velocity
			 */
			shared->Task.command = 'c';

			// Update name and start
			shared->Task.name = "FITTS";
			Tasks.Fitts();


			// // Initialize logging
			// if ( shared->Logging.isLoggingEnabled && !shared->Logging.isLoggingActivelyRunning && shared->Task.isRunning ) {

			// 	// Set logging headers
			// 	shared->loggingHeader1 = "TouchDetected";
			// 	shared->loggingHeader2 = "OutgoingPacket";
			// 	shared->loggingHeader3 = "IncomingPacket";
			// 	shared->loggingHeader4 = "Serial0Enabled,Serial0Open,Serial1Enabled,Serial1Open";
			// 	shared->loggingHeader5 = "kpx,kpy,kix,kiy,kdx,kdy";

			// 	// Start timer and logging
			// 	Timing.TaskTimerStart();
			// 	Logging.Initialize();

			// 	shared->Logging.isLoggingActivelyRunning = true;

			// }
			// // Save logging info each loop
			// else {

			// 	shared->loggingVariable1 = std::to_string( shared->Touchscreen.isTouched );
			// 	shared->loggingVariable2 = shared->Serial.packetOut.substr( 0, shared->Serial.packetOut.length() - 1 );
			// 	shared->loggingVariable3 = shared->Serial.packetIn.substr( 0, shared->Serial.packetIn.length() - 1 );
			// 	shared->loggingVariable4 = std::to_string( shared->Serial.isSerialSending ) + "," + std::to_string( shared->Serial.isSerialSendOpen ) + "," + std::to_string( shared->Serial.isSerialReceiving ) + "," + std::to_string( shared->Serial.isSerialReceiveOpen );
			// 	shared->loggingVariable5 = shared->FormatDecimal( shared->controllerKp.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKp.y, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKi.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKi.y, 1, 1 ) + ","
			// 		+ shared->FormatDecimal( shared->controllerKd.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKd.y, 1, 1 );
			// 	Logging.AddEntry();
			// }
			// TaskFitts();	// Fallback to just this
			break;
		}

		case taskEnum::CALIBRATE: {

			shared->Task.name = "CALIBRATE";
			Tasks.Calibration();

			break;
		}
	}
}


/**
 * @brief Update system state using kalman filter
 * 
 */
void UpdateSystem() {

	if ( shared->Target.isTargetFound && shared->Capture.isFrameReady ) {

		if ( cv::norm( cv::Point2f( shared->Target.positionUnfilteredMM.x, shared->Target.positionUnfilteredMM.y ) - cv::Point2f( shared->Target.positionFilteredOldMM.x, shared->Target.positionFilteredOldMM.y ) ) > 100.0f ) {
			shared->Target.isTargetReset = true;
		}


		// Save old data
		shared->Target.positionFilteredOldMM = shared->Target.positionFilteredNewMM;
		shared->Target.velocityFilteredOldMM = shared->Target.velocityFilteredNewMM;

		// Update kalman filter
		Kalman.Update( shared->Target.positionUnfilteredMM, shared->Timing.elapsedRunningTime );

		// Get updated state values
		shared->Target.positionFilteredNewMM = Kalman.GetPosition();
		shared->Target.velocityFilteredNewMM = Kalman.GetVelocity();
		shared->Target.positionIntegratedMM	 = Kalman.GetIntegralError();

		// Update marker if finger calibrated
		if ( shared->Calibration.isCalibrated ) {

			// Offset based on calibrated touch
			shared->Target.positionFilteredNewMM.x = shared->Target.positionFilteredNewMM.x + shared->Calibration.calibratedOffetMM.x;
			shared->Target.positionFilteredNewMM.y = shared->Target.positionFilteredNewMM.y + CONFIG_TARGET_OFFSET_Y_MM + shared->calibrationOffsetMM.y;
		}

		if ( shared->System.useRing ) {

			std::cout << "Ring: " << shared->Ring.ringPositionMm << "   Finger: " << shared->Ring.fingerPositionMm << "   Delta: " << shared->Target.positionFilteredNewMM - shared->Ring.fingerPositionMm << "\n";
			shared->Target.positionFilteredNewMM = shared->Target.positionFilteredNewMM - shared->Ring.fingerPositionMm - cv::Point3f( 0.0f, 25.0f, 0.0f );
		}

		// std::cout << "dE = " << shared->targetMarkerPosition3dOld.x - shared->targetMarkerPosition3dNew.x << "dE/dt = " << ( shared->targetMarkerPosition3dOld.x - shared->targetMarkerPosition3dNew.x ) / shared->kalmanDt << "\n" ;

		// Get covariance from Kalman filter
		// shared->kalmanP = Kalman.GetCovariance();
	}
}



/**
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */

void PrintState() {

	// enum class stateEnum { IDLE, DRIVING_PWM, MEASURING_LIMITS, MEASURING_CURRENT, ZERO_ENCODER };

	switch ( shared->System.state ) {

		case stateEnum::IDLE: {
			std::cout << "State: IDLE\n";
			break;
		}

		case stateEnum::DRIVING_PWM: {
			std::cout << "State: DRIVING_PWM\n";
			break;
		}

		case stateEnum::MEASURING_LIMITS: {
			std::cout << "State: MEASURING_LIMITS\n";
			break;
		}


		case stateEnum::ZERO_ENCODER: {
			std::cout << "State: ZERO_ENCODER\n";
			break;
		}
	}
}


/**
 * @brief Shutdown safely if error occurs
 */
void SignalHandler( int signum ) {
	std::cout << "\nMain:         Interrupt signal (" << signum << ") received.\n";
	shared->System.isMainRunning = false;
	Touch.Close();
	exit( signum );
}


// /**
//  * @brief Start fitts law testing
//  */
// void TaskFitts() {

// 	// Initialize logging file and update entries
// 	if ( shared->Logging.isEnabled && !shared->Logging.isRunning && shared->Task.isRunning ) {

// 		// Customize header
// 		shared->loggingHeader1 = "TouchDetected";
// 		shared->loggingHeader2 = "OutgoingPacket";
// 		shared->loggingHeader3 = "IncomingPacket";
// 		shared->loggingHeader4 = "Serial0Enabled,Serial0Open,Serial1Enabled,Serial1Open";
// 		shared->loggingHeader5 = "kpx,kpy,kix,kiy,kdx,kdy";

// 		// Initialize and add initial entry
// 		// Start timer for measuring loop frequency
// 		Timing.TaskTimerStart();
// 		Logging.Initialize();
// 		// Timing.StartTimer();
// 		// Logging.AddEntry();
// 		shared->Logging.isRunning = true;

// 	} else if ( shared->Logging.isEnabled && shared->Logging.isRunning ) {

// 		shared->loggingVariable1 = std::to_string( shared->Touchscreen.isTouched );
// 		shared->loggingVariable2 = shared->Serial.packetOut.substr( 0, shared->Serial.packetOut.length() - 1 );
// 		shared->loggingVariable3 = shared->Serial.packetIn.substr( 0, shared->Serial.packetIn.length() - 1 );
// 		shared->loggingVariable4 = std::to_string( shared->Serial.isSerialSending ) + "," + std::to_string( shared->Serial.isSerialSendOpen ) + "," + std::to_string( shared->Serial.isSerialReceiving ) + "," + std::to_string( shared->Serial.isSerialReceiveOpen );
// 		shared->loggingVariable5 = shared->FormatDecimal( shared->controllerKp.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKp.y, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKi.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKi.y, 1, 1 ) + ","
// 			+ shared->FormatDecimal( shared->controllerKd.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKd.y, 1, 1 );
// 		Logging.AddEntry();
// 	}

// 	// std::cout << "0: " << shared->serialPacket0.substr( 0, shared->serialPacket0.length() ) << "  1: " << shared->serialPacket1.substr( 0, shared->serialPacket1.length() ) << "\n";

// 	// Run task
// 	if ( !shared->Task.isRunning ) {

// 		shared->Task.isRunning = true;
// 		// shared->Task.isComplete			= false;
// 		shared->Telemetry.isTargetFound = 1;
// 		Fitts.Initialize();
// 		Fitts.StartTest( 'z' );	   // x , y , z, t , v, f
// 	} else {
// 		Timing.UpdateTaskTime();
// 		Fitts.Update();
// 	}
// }
