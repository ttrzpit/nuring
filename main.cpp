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
#include "include/CaptureClass.h"
#include "include/DisplayClass.h"
#include "include/FittsClass.h"
#include "include/InputClass.h"
#include "include/LoggingClass.h"
#include "include/SerialClass.h"
#include "include/TimingClass.h"
#include "include/TouchscreenClass.h"


// New class objects
CaptureClass	 Capture( dataHandle );	   // Camera capture
ArucoClass		 Aruco( dataHandle );	   // Aruco detector
DisplayClass	 Canvas( dataHandle );	   // Display output
InputClass		 Input( dataHandle );	   // Keyboard input
TimingClass		 Timing( dataHandle );	   // Loop timing measurement
TouchscreenClass Touch( dataHandle );	   // Touchscreen position reading
SerialClass		 Serial( dataHandle );	   // Serial interface
LoggingClass	 Logging( dataHandle );	   // Logging interface
FittsClass		 Fitts( dataHandle, Timing, Logging );



// Function prototypes
void SignalHandler( int signum );



/**
 * @brief Main program
 */
int main() {

	// Optimization for C/C++ data types
	std::ios_base::sync_with_stdio( false );
	std::cout.setf( std::ios::unitbuf );



	// For debugging
	std::cout << "OpenCV Build: " << cv::getBuildInformation() << std::endl;
	// cv::utils::logging::setLogLevel( cv::utils::logging::LOG_LEVEL_VERBOSE );



	// Status update
	std::cout << "\nMain:         Program running...\n\n";



	// Start timer for measuring loop frequency
	Timing.StartTimer();

	// Add shortcut panel
	Canvas.ShowShortcuts();
	Canvas.ShowVisualizer();

	std::cout << cv::getBuildInformation() << std::endl;
	// Main loop
	while ( shared->FLAG_MAIN_RUNNING ) {



		// Trial selector
		if ( shared->TASK_NUMBER == 0 ) {
			// Nothing
		} else if ( shared->TASK_NUMBER == 1 ) {
			// Nothing
		} else if ( shared->TASK_NUMBER == 2 ) {
			shared->TASK_NAME = "Fitts";
			Fitts.Update();
		} else {
		}

		// Parse any input and use OpenCV WaitKey()
		Input.ParseInput( cv::pollKey() & 0xFF );

		// Capture frame
		Capture.GetFrame();

		// Detect ArUco tags
		Aruco.FindTags();

		// Check touchscreen input
		Touch.GetCursorPosition();

		// Send serial commands
		if ( shared->FLAG_SERIAL_OPEN ) {
			// Serial.Send( shared->serialPacket );

			// Share packet if serial is enabled
			if ( shared->FLAG_SERIAL_ENABLED ) {

				shared->serialPacket = "EM" +
					Serial.PadValues( shared->arucoTags[shared->arucoActiveID].errorMagnitude2D, 3 ) + "H" +
					Serial.PadValues( shared->arucoTags[shared->arucoActiveID].errorTheta * RAD2DEG, 3 ) + "m" +
					Serial.PadValues( shared->arucoTags[shared->arucoActiveID].velMagnitude, 3 ) + "h" +
					Serial.PadValues( shared->arucoTags[shared->arucoActiveID].velHeading * RAD2DEG, 3 ) + "X\n";
			}
			Serial.Monitor();
		}

		// Update display
		Canvas.Update();
		Canvas.UpdateVisualizer();

		// Update timer (for measuring loop frequency)
		Timing.UpdateTimer();

		// std::cout << shared->touchPosition.x << " , " << shared->touchPosition.y << " | " << shared->touchPosition.z << "\n";
	}


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
