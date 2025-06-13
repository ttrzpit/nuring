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

// Function prototypes
void		SignalHandler( int signum );
void		TaskCalibrate();
void		TaskFitts();
void		TaskFittsAngle();
std::string BuildPacketAngularError();
void		UpdateSystem();
void		SendSerialPacket();
void		BuildSerialPacket();
void		UpdateSerial();
void		UpdateState();
void		PrintState();



/**
 * @brief Main program loop
 * 
 * @return int 
 */
int main() {

	// Testing
	shared->Controller.commandedPercentageLimit = cv::Point3f( 1.0f, 1.0f, 1.0f );
	shared->Controller.isLimitSet				= true;
	shared->Controller.gainKi					= { 0.0f, 0.0f, 0.0f, 0.0f };
	shared->Controller.isCalibrated				= false;
	// shared->Controller.calibratedOffetMM		= cv::Point3i( -50.0f, -75.0f, 140.0f );



	// Settings
	shared->Logging.isLoggingEnabled = true;
	shared->Task.userID				 = 000;
	shared->Telemetry.isTargetFound	 = 1;

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
		Timing.UpdateTimer();


		// Parse any input and use OpenCV WaitKey()
		Input.ParseInput( cv::pollKey() & 0xFF );

		// Capture frame
		Capture.GetFrame();


		// UpdateState();

		// Detect ArUco tags
		if ( shared->Task.name != "FITTS_ANGLE" ) {
			Aruco.FindTags();
		}



		// Task selector
		if ( shared->Task.name == "FITTS" ) {
			TaskFitts();
		} else if ( shared->Task.name == "CALIB" ) {
			TaskCalibrate();
		} else if ( shared->Task.name == "FITTS_ANGLE" ) {
			TaskFittsAngle();
		}

		// Update system
		UpdateSystem();

		// Update controller
		Controller.Update();
		Controller.UpdateAmplifier();

		// Check touchscreen input
		Touch.GetCursorPosition();

		// Update serial messages
		Serial.Update();

		// Update display
		Canvas.Update();

		// Save logging file
		if ( shared->Task.isComplete ) {
			Logging.Save();
			std::cout << "File saved!\n";
			shared->Task.isComplete = false;
		}

		// Update shutdown flags for clean shutdown
		if ( shared->System.isShuttingDown ) {
			shared->System.isMainRunning = false;
		}
	}
	cv::destroyAllWindows();
	return 0;
}



/**
 * @brief Update system state using kalman filter
 * 
 */
void UpdateSystem() {

	if ( shared->Telemetry.isTargetFound && shared->Capture.isFrameReady ) {

		if ( cv::norm( cv::Point2f( shared->Telemetry.positionUnfilteredMM.x, shared->Telemetry.positionUnfilteredMM.y ) - cv::Point2f( shared->Telemetry.positionFilteredOldMM.x, shared->Telemetry.positionFilteredOldMM.y ) ) > 100.0f ) {
			shared->Telemetry.isTargetReset = true;
		}


		// Save old data
		shared->Telemetry.positionFilteredOldMM = shared->Telemetry.positionFilteredNewMM;
		shared->Telemetry.velocityFilteredOldMM = shared->Telemetry.velocityFilteredNewMM;

		// Update kalman filter
		Kalman.Update( shared->Telemetry.positionUnfilteredMM, shared->Timing.elapsedRunningTime );

		// Get updated state values
		shared->Telemetry.positionFilteredNewMM = Kalman.GetPosition();
		shared->Telemetry.velocityFilteredNewMM = Kalman.GetVelocity();
		shared->Telemetry.positionIntegratedMM	= Kalman.GetIntegralError();

		// Update marker if finger calibrated
		if ( shared->Controller.isCalibrated ) {
			shared->Telemetry.positionFilteredNewMM = shared->Telemetry.positionFilteredNewMM - cv::Point3f( shared->Controller.calibratedOffetMM );
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

		case stateEnum::MEASURING_CURRENT: {
			std::cout << "State: MEASURING_CURRENT\n";
			break;
		}

		case stateEnum::ZERO_ENCODER: {
			std::cout << "State: ZERO_ENCODER\n";
			break;
		}
	}
}

// void UpdateState() {

// 	switch ( shared->System.state ) {

// 		case stateEnum::IDLE:{

// 		}
// 	}
// }



/**
 * @brief Shutdown safely if error occurs
 */
void SignalHandler( int signum ) {
	std::cout << "\nMain:         Interrupt signal (" << signum << ") received.\n";
	shared->System.isMainRunning = false;
	Touch.Close();
	exit( signum );
}

/**
 * @brief Run calibration routine
 */
void TaskCalibrate() {

	// Task hasn't been started yet
	if ( !shared->Task.isRunning ) {

		// Initialize prior to starting logging
		if ( !shared->Logging.isLoggingActivelyRunning ) {

			// Disable amplifiers for safety
			shared->Amplifier.isAmplifierActive = false;

			shared->Telemetry.isTargetFound = 8;

			// Customize logging header
			shared->loggingHeader1 = "UserOffsetX";
			shared->loggingHeader2 = "UserOffsetY";
			shared->loggingHeader3 = "UserOffsetZ";

			// Initialize needed items before starting task
			Timing.GetTimestamp();
			Logging.Initialize();

			// Update logging flag
			shared->Logging.isLoggingActivelyRunning = true;



		} else {	// Logging started

			// Clear values
			shared->Task.repetitionNumber = 0;
			shared->calibrationOffsetMM	  = cv::Point3i( 0, 0, 0 );

			// Initialize task
			Calibration.Initialize();

			// Update
			shared->Task.isRunning = true;
		}

	} else {	// Task running

		// Run task if not complete
		if ( shared->Task.repetitionNumber < 5 ) {

			Calibration.Update();

		} else if ( shared->Task.repetitionNumber == 5 ) {	  // Task complete

			// End calibration and dave data
			Calibration.End();
			Calibration.Update();

		} else {

			Logging.AddEntry();
			Logging.Save();
			shared->Task.name						 = "";
			shared->Logging.isLoggingActivelyRunning = false;
			shared->Task.isRunning					 = false;
			shared->calibrationComplete				 = true;
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
	if ( shared->Logging.isLoggingEnabled && !shared->Logging.isLoggingActivelyRunning && shared->Task.isRunning ) {

		// Customize header
		shared->loggingHeader1 = "TouchDetected";
		shared->loggingHeader2 = "OutgoingPacket";
		shared->loggingHeader3 = "IncomingPacket";
		shared->loggingHeader4 = "Serial0Enabled,Serial0Open,Serial1Enabled,Serial1Open";
		shared->loggingHeader5 = "kpx,kpy,kix,kiy,kdx,kdy";

		// Initialize and add initial entry
		// Start timer for measuring loop frequency
		Timing.TaskTimerStart();
		Logging.Initialize();
		// Timing.StartTimer();
		// Logging.AddEntry();
		shared->Logging.isLoggingActivelyRunning = true;

	} else if ( shared->Logging.isLoggingEnabled && shared->Logging.isLoggingActivelyRunning ) {

		shared->loggingVariable1 = std::to_string( shared->Touchscreen.isTouched );
		shared->loggingVariable2 = shared->Serial.packetOut.substr( 0, shared->Serial.packetOut.length() - 1 );
		shared->loggingVariable3 = shared->Serial.packetIn.substr( 0, shared->Serial.packetIn.length() - 1 );
		shared->loggingVariable4 = std::to_string( shared->Serial.isSerialSending ) + "," + std::to_string( shared->Serial.isSerialSendOpen ) + "," + std::to_string( shared->Serial.isSerialReceiving ) + "," + std::to_string( shared->Serial.isSerialReceiveOpen );
		shared->loggingVariable5 = shared->FormatDecimal( shared->controllerKp.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKp.y, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKi.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKi.y, 1, 1 ) + ","
			+ shared->FormatDecimal( shared->controllerKd.x, 1, 1 ) + "," + shared->FormatDecimal( shared->controllerKd.y, 1, 1 );
		Logging.AddEntry();
	}

	// std::cout << "0: " << shared->serialPacket0.substr( 0, shared->serialPacket0.length() ) << "  1: " << shared->serialPacket1.substr( 0, shared->serialPacket1.length() ) << "\n";

	// Run task
	if ( !shared->Task.isRunning ) {

		shared->Task.isRunning			= true;
		shared->Task.isComplete			= false;
		shared->Telemetry.isTargetFound = 1;
		Fitts.Initialize();
		Fitts.StartTest( 'z' );	   // x , y , z, t , v, f
	} else {
		Timing.UpdateTaskTime();
		Fitts.Update();
	}
}



/**
 * @brief Start fitts law testing
 */
void TaskFittsAngle() {

	// Initialize logging file and update entries
	if ( shared->Logging.isLoggingEnabled && !shared->Logging.isLoggingActivelyRunning && shared->Task.isRunning ) {

		// Customize header
		shared->loggingHeader1 = "ThetaDesired";
		shared->loggingHeader2 = "ThetaMeasured";

		// Initialize and add initial entry
		Timing.GetTimestamp();
		Logging.Initialize();
		Logging.AddEntry();
		shared->Logging.isLoggingActivelyRunning = true;

	} else if ( shared->Logging.isLoggingEnabled && shared->Logging.isLoggingActivelyRunning ) {

		shared->loggingVariable1 = std::to_string( shared->angleDesired );
		shared->loggingVariable2 = std::to_string( shared->angleTheta );
		Logging.AddEntry();
	}

	// Run task
	if ( !shared->Task.isRunning ) {

		shared->Task.isRunning			= true;
		shared->Task.isComplete			= false;
		shared->Telemetry.isTargetFound = 1;
		shared->angleEnabled			= !shared->angleEnabled;
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



// void BuildSerialPacket() {

// 	if ( shared->serialTrigger == "drive" ) {
// 		// Build string32
// 		shared->Serial.packetOut
// 			= std::string( "E" ) + ( shared->Amplifier.isAmplifierActive ? "1" : "0" ) + "A" + shared->PadValues( shared->Controller.commandedPwmABC.x, 4 ) + "B" + shared->PadValues( shared->Controller.commandedPwmABC.y, 4 ) + "C" + shared->PadValues( shared->Controller.commandedPwmABC.z, 4 ) + "X";
// 	} else if ( shared->serialTrigger == "reset" ) {
// 		shared->Serial.packetOut = "E0RX";
// 		shared->serialTrigger	 = "drive";
// 	}
// }



// void UpdateSerial() {

// 		// Make sure serial comms are open and running
// 		if ( shared->Serial.isSerialSendOpen && shared->Serial.isSerialSending ) {

// 			// Send appropriate packet
// 			if ( shared->System.state == "RUNNING" ) {	// Send drive commands

// 				// Send drive packet
// 				Serial.SendDrivePacket();

// 			} else if ( shared->System.state == "MEASURE_LIMITS" ) { 	// Send encoder measurement commands

// 				// Send limits packet
// 				Serial.SendLimitsPacket() ;

// 			} else if ( shared->System.state == "WAITING" ) {
// 				Serial.SendWaitingPacket() ;
// 			}
// 		}

// 		// Check for new incoming serial packet
// 		if ( shared->Serial.isSerialReceiveOpen && shared->Serial.isSerialReceiving ) {
// 			Serial.CheckForPacket();
// 		}
// }