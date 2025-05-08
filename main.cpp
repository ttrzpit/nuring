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
#include "include/DisplayClass.h"
#include "include/FittsClass.h"
#include "include/InputClass.h"
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


// Function prototypes
void SignalHandler( int signum );
void TaskCalibrate();
void TaskFitts();


/**
 * @brief Main program loop
 * 
 * @return int 
 */
int main() {



	// Settings
	shared->FLAG_LOGGING_ENABLED = true;
	shared->TASK_USER_ID		 = 000;

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

	// Main loop
	while ( shared->FLAG_MAIN_RUNNING ) {

		// Task selector
		if ( shared->TASK_NAME == "FITTS" ) {
			TaskFitts();
		} else if ( shared->TASK_NAME == "CALIBRATE" ) {
			TaskCalibrate();
		}

		// Parse any input and use OpenCV WaitKey()
		Input.ParseInput( cv::pollKey() & 0xFF );

		// Capture frame
		Capture.GetFrame();

		// Detect ArUco tags
		Aruco.FindTags();

		// Check touchscreen input
		Touch.GetCursorPosition();

		// Check for new incoming serial packet
		if ( shared->FLAG_SERIAL1_OPEN ) {
			Serial.CheckForPacket();
		}

		// Send serial commands
		if ( shared->FLAG_SERIAL0_OPEN ) {	  // Check if serial port is open

			// Check if serial is enabled and amplifiers are enabled
			if ( shared->FLAG_SERIAL0_ENABLED ) {

				if ( shared->FLAG_AMPLIFIERS_READY && shared->arucoActiveID != 0 ) {

					if ( shared->FLAG_TAG_FOUND ) {

						// Format properly
						int8_t signX = Serial.Sign( shared->arucoTags[shared->arucoActiveID].error3D.x );
						int8_t signY = Serial.Sign( shared->arucoTags[shared->arucoActiveID].error3D.y );

						shared->serialPacket0 = "Ex" + Serial.PadValues( signX, 1 ) + Serial.PadValues( abs( shared->arucoTags[shared->arucoActiveID].error3D.x * 1.5 ), 3 ) + "y" + Serial.PadValues( signY, 1 ) + Serial.PadValues( abs( shared->arucoTags[shared->arucoActiveID].error3D.y ), 3 ) + "z"
							+ Serial.PadValues( abs( shared->arucoTags[shared->arucoActiveID].error3D.z ), 3 ) + "X\n";
					} else {
						shared->serialPacket0 = "RoX\n";
					}

					// shared->serialPacket = "Ex" + Serial.PadValues( shared->arucoTags[shared->arucoActiveID].error3D.x, 3 ) + "y" + Serial.PadValues( shared->arucoTags[shared->arucoActiveID].error3D.y, 3 ) + "z" + Serial.PadValues( shared->arucoTags[shared->arucoActiveID].error3D.z, 3 ) + "X\n";

					// shared->serialPacket = "EM" + Serial.PadValues( shared->arucoTags[shared->arucoActiveID].errorMagnitudeNorm2D, 3 ) + "H" + Serial.PadValues( shared->arucoTags[shared->arucoActiveID].errorTheta * RAD2DEG, 3 ) + "m"
					// 	+ Serial.PadValues( shared->arucoTags[shared->arucoActiveID].velMagnitude, 3 ) + "h" + Serial.PadValues( shared->arucoTags[shared->arucoActiveID].velHeading * RAD2DEG, 3 ) + "X\n";
					shared->FLAG_PACKET_WAITING = true;
				} else {
					shared->serialPacket0		= "DX\n";
					shared->FLAG_PACKET_WAITING = true;
				}

			} else {

				shared->serialPacket0		= "DX\n";
				shared->FLAG_PACKET_WAITING = true;
			}

			Serial.Monitor();
		} else {
			shared->serialPacket0 = "DX\n";
			// shared->FLAG_PACKET_WAITING = true;
		}



		// Update display
		Canvas.Update();

		// Update timer (for measuring loop frequency)
		Timing.UpdateTimer();


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

	} else if ( shared->FLAG_LOGGING_ENABLED && shared->FLAG_LOGGING_STARTED ) {

		shared->loggingVariable1 = std::to_string( shared->calibrationOffsetMM.x );
		shared->loggingVariable1 = std::to_string( shared->calibrationOffsetMM.y );
		shared->loggingVariable1 = std::to_string( shared->calibrationOffsetMM.z );
		Logging.AddEntry();
	}

	if ( !shared->TASK_RUNNING ) {
		shared->calibrationComplete = false;
		shared->calibrationOffsetMM = cv::Point3i( 0, 0, 0 );
		shared->TASK_REP_NUMBER		= 0;
		shared->TASK_RUNNING		= true;
		shared->arucoActiveID		= 8;
		Calibration.InitializeCalibration();
		Calibration.StartCalibration();
	} else {
		Calibration.Update();
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

		// Initialize and add initial entry
		Logging.Initialize();
		Logging.AddEntry();
		shared->FLAG_LOGGING_STARTED = true;

	} else if ( shared->FLAG_LOGGING_ENABLED && shared->FLAG_LOGGING_STARTED ) {

		shared->loggingVariable1 = std::to_string( shared->arucoTags[shared->arucoActiveID].present );
		shared->loggingVariable2 = std::to_string( shared->touchDetected );
		Logging.AddEntry();
	}

	// Run task
	if ( !shared->TASK_RUNNING ) {
		shared->TASK_RUNNING  = true;
		shared->TASK_COMPLETE = false;
		shared->arucoActiveID = 1;
		Fitts.StartTest( 'y' );
	} else {
		Fitts.Update();
	}
}
