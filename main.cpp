

// Necessary libraries
#include <iostream>							// For input/output handling
#include <opencv2/core/utils/logger.hpp>	// For verbose debugging
#include <unistd.h>							// For sleep

// OpenCV libraries
#include <opencv2/highgui.hpp>	  // For pollKey()

// Configuration file
#include "include/config.h"

// Custom libraries
#include "include/CaptureInterface.h"
#include "include/SerialInterface.h"

// Class objects
CaptureInterface Capture;
SerialInterface	 Serial;

// Global flags
bool RUNNING = true;



// Function prototypes
void ParseInput( int key );



/**
 * @brief Main program function
 * @return 0 on successful close
 */
int main() {

	// Ensure all OpenCV objects closed
	cv::destroyAllWindows();

	// For debugging
	// std::cout << "OpenCV Build: " << cv::getBuildInformation() << std::endl;
	// cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_VERBOSE);

	// Optimization for C/C++ data types
	std::ios_base::sync_with_stdio( false );
	std::cout.setf( std::ios::unitbuf );

	while( RUNNING ) {

		// Parse input ( pollKey is needed for openCv to loop properly )
		ParseInput( cv::pollKey() );

		Capture.GetFrame();
		Capture.FindTags();

		cv::imshow( "Raw", Capture.matFrame );
	}

	// Closing message
	std::cout << "Program shutting down... \n";

	// Close serial port
	Serial.Close();

	// Shut down openCV
	cv::destroyAllWindows();

	// Exit & return
	exit( 0 );
	return 0;
}

void ParseInput( int key ) {

	// Ignore mouse clicks or arrows
	if( key != -1 )

	{
		// std::cout << "Key: " << key << "\n" ;

		switch( key ) {
		case 1048603:	 // ESC key
			std::cout << "Stop key pressed, exiting.\n";
			RUNNING = false;
			break;
		}
	}
}