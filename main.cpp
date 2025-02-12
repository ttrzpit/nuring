

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
#include "include/DisplayInterface.h"
#include "include/SerialInterface.h"
#include "include/TimingInterface.h"

// Class objects
CaptureInterface Capture;
SerialInterface	 Serial;
TimingInterface	 Timing;
DisplayInterface Display;

// Global flags
bool FLAG_PROGRAM_RUNNING = true;
bool FLAG_MOTORS_ENABLED  = false;
bool FLAG_SERIAL_ENABLED  = false;


// Function prototypes
void ParseInput( int key );



/**
 * @brief Main program function
 * @return 0 on successful close
 */
int main() {

	// Ensure all OpenCV objects closed
	cv::destroyAllWindows();

	// Outgoing packet string
	std::string outgoingPacket = "";

	// For debugging
	// std::cout << "OpenCV Build: " << cv::getBuildInformation() << std::endl;
	// cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_VERBOSE);

	// Optimization for C/C++ data types
	std::ios_base::sync_with_stdio( false );
	std::cout.setf( std::ios::unitbuf );

	// Temporary counter for testing serial output
	int counter = 0;

	// Start timer for measuring loop frequency
	Timing.StartTimer();

	// Check serial status

	FLAG_SERIAL_ENABLED = Serial.GetStatus();
	Display.FLAG_SERIAL = FLAG_SERIAL_ENABLED;

	while( FLAG_PROGRAM_RUNNING ) {

		// Parse input ( pollKey is needed for openCv to loop properly )
		ParseInput( cv::pollKey() );

		// Get current frame and find aruco tags
		Capture.GetFrame();
		Capture.FindTags();

		// Show updated image
		cv::imshow( "Raw", Display.Update( Capture.matFrame, Capture ) );

		// Send serial packet
		if( Capture.markerFound ) {

			// If motors have been enabled, start sending serial packet
			if( FLAG_MOTORS_ENABLED && FLAG_SERIAL_ENABLED ) {
				outgoingPacket = "x" + std::to_string( Capture.Markers[Capture.activeMarker].error3D.x ) + "y" + std::to_string( Capture.Markers[Capture.activeMarker].error3D.y ) + "z" + std::to_string( Capture.Markers[Capture.activeMarker].error3D.z ) + "\n";
				Serial.Send( outgoingPacket );
				Display.setSerialString( outgoingPacket.substr( 0, outgoingPacket.length() - 1 ) );
			} else if( !FLAG_MOTORS_ENABLED && FLAG_SERIAL_ENABLED ) {
				outgoingPacket = "Standby\n";
				Serial.Send( outgoingPacket );
				Display.setSerialString( outgoingPacket.substr( 0, outgoingPacket.length() - 1 ) );
			}
		}

		// Check program frequency
		Display.setFrequency( Timing.CheckFrequency() );
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
		std::cout << "Detected key press = " << key << "\n";

		switch( key ) {
		case 1179675:	 // ESC key
			Display.setStatusString( "CAPS LOCK IS ON!" );
			// std::cout << "Stop key pressed, exiting.\n";
			// RUNNING = false;
			break;
		case 1048603:	 // ESC key
			Display.setStatusString( "Stop key pressed, exiting." );
			std::cout << "Stop key pressed, exiting.\n";
			FLAG_PROGRAM_RUNNING = false;
			break;
		case 1048625:	 // 1
			Display.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 1 ? "0." : "1." ) );
			( Capture.activeMarker == 1 ? Capture.activeMarker = 0 : Capture.activeMarker = 1 );
			break;
		case 1048626:	 // 2
			Display.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 2 ? "0." : "2." ) );
			( Capture.activeMarker == 2 ? Capture.activeMarker = 0 : Capture.activeMarker = 2 );
			break;
		case 1048627:	 // 3
			Display.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 3 ? "0." : "3." ) );
			( Capture.activeMarker == 3 ? Capture.activeMarker = 0 : Capture.activeMarker = 3 );
			break;
		case 1048628:	 // 4
			Display.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 4 ? "0." : "4." ) );
			( Capture.activeMarker == 4 ? Capture.activeMarker = 0 : Capture.activeMarker = 4 );
			break;
		case 1048629:	 // 5
			Display.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 5 ? "0." : "5." ) );
			( Capture.activeMarker == 5 ? Capture.activeMarker = 0 : Capture.activeMarker = 5 );
			break;
		case 1048630:	 // 6
			Display.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 6 ? "0." : "6." ) );
			( Capture.activeMarker == 6 ? Capture.activeMarker = 0 : Capture.activeMarker = 6 );
			break;
		case 1048631:	 // 7
			Display.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 7 ? "0." : "7." ) );
			( Capture.activeMarker == 7 ? Capture.activeMarker = 0 : Capture.activeMarker = 7 );
			break;
		case 1048632:	 // 8
			Display.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 8 ? "0." : "8." ) );
			( Capture.activeMarker == 8 ? Capture.activeMarker = 0 : Capture.activeMarker = 8 );
			break;
		case 1048633:	 // 9
			Display.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 9 ? "0." : "9." ) );
			( Capture.activeMarker == 9 ? Capture.activeMarker = 0 : Capture.activeMarker = 9 );
			break;
		case 1048634:	 // 10
			Display.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 10 ? "0." : "10." ) );
			( Capture.activeMarker == 10 ? Capture.activeMarker = 0 : Capture.activeMarker = 10 );
			break;
		case 1048672:	 // ~
			Display.setStatusString( "Disabling all active markers." );
			Capture.activeMarker = 0;
			break;
		case 1048677:	 // E
			FLAG_MOTORS_ENABLED = !FLAG_MOTORS_ENABLED;
			if( FLAG_MOTORS_ENABLED ) {
				Display.FLAG_AMPLIFIERS = true;
				Display.setStatusString( "Amplifiers enabled." );
			} else {
				Display.FLAG_AMPLIFIERS = false;
				Display.setStatusString( "Amplifiers disabled." );
			}
			break;
		case 1048691:	 // S
			FLAG_SERIAL_ENABLED = !FLAG_SERIAL_ENABLED;
			if( FLAG_SERIAL_ENABLED ) {
				Display.FLAG_SERIAL = true;
				Display.setStatusString( "Enabling Serial output." );
			} else {
				Display.FLAG_SERIAL = false;
				Display.setStatusString( "Disabling Serial output." );
			}
			break;
		case 1048696:	 // X
			Display.setStatusString( "Software E-Stop Triggered. Amplifiers and serial output disabled." );
			FLAG_SERIAL_ENABLED		= false;
			FLAG_MOTORS_ENABLED		= false;
			Display.FLAG_SERIAL		= FLAG_SERIAL_ENABLED;
			Display.FLAG_AMPLIFIERS = FLAG_MOTORS_ENABLED;
			break;
		}
	}
}