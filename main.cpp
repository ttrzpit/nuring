// Linux headers for reading mouse position / touch
#include <X11/Xlib.h>
#include <csignal>
#undef Status
#include <fcntl.h>
#include <linux/input.h>
#include <tuple>

// Necessary libraries
#include <iostream>	   // For input/output handling
#include <unistd.h>	   // For sleep



// OpenCV libraries
#include <opencv2/core/utils/logger.hpp>	// For verbose debugging
#include <opencv2/highgui.hpp>				// For pollKey()

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
DisplayInterface OutputDisplay;

// Display interface for mouse input
Display* dpy;



// Global flags
bool FLAG_PROGRAM_RUNNING = true;
bool FLAG_MOTORS_ENABLED  = false;
bool FLAG_SERIAL_ENABLED  = false;

// Function prototypes
void ParseInput( int key );

// Outgoing packet stri
std::string outgoingPacket = "";

// TEMP
cv::Mat matBlank = cv::Mat( 1080, 1920, CV_8UC3 );


// void CalibrationCallback( int event, int x, int y, int flags, void* userdata ) {
// 	if( event == cv::EVENT_MBUTTONUP ) {
// 		std::cout << "Touch at " << x << " , " << y << "\n";
// 		cv::circle( matBlank, cv::Point( x, y ), 10, CONFIG_colOraMd, -1 );
// 	}
// 	if( event == cv::EVENT_LBUTTONUP ) {
// 		std::cout << "Touch at " << x << " , " << y << "\n";
// 		cv::circle( matBlank, cv::Point( x, y ), 10, CONFIG_colRedMd, -1 );
// 	}
// }

void CleanUpOnExit( Display* dpy ) {

	std::cout << "Close command received. \n";

	// Shutdown X11 display
	XCloseDisplay( dpy );

	// Terminal x11 tracking
	std::cout << "\033[?1000l\033[?1002l\033[?1003l";	 // disable tracking
	std::cout.flush();

	// Close serial port
	Serial.Close();

	// Shut down openCV
	cv::destroyAllWindows();


	// Exit
	exit( 0 );
}

// Returns a tuple: (x, y, leftButtonPressed)
std::tuple<int, int, bool> getMouseInfo( Display* dpy ) {


	if( !dpy ) {
		std::cerr << "Cannot open display" << std::endl;
		return { -1, -1, false };
	}

	// Get the root window (usually the entire screen)
	Window root = DefaultRootWindow( dpy );

	// Variables to store pointer position and window info
	int			 root_x, root_y;	// Pointer position relative to the root window
	int			 win_x, win_y;		// Pointer position relative to the target window (not used here)
	unsigned int mask;				// Button/mask state
	Window		 child_return, root_return;

	// Query the pointer's position and button state
	if( !XQueryPointer( dpy, root, &root_return, &child_return, &root_x, &root_y, &win_x, &win_y, &mask ) ) {
		std::cerr << "XQueryPointer failed" << std::endl;
		XCloseDisplay( dpy );
		return { -1, -1, false };
	}

	// Check if the left mouse button (Button1) is pressed.
	bool leftButtonPressed = ( mask & Button1Mask ) != 0;

	// Close the X display connection


	return { root_x - 3440, root_y - 32, leftButtonPressed };
}



// Signal handler for SIGINT (Ctrl+C)
void signalHandler( int signum ) {
	std::cout << "\nInterrupt signal (" << signum << ") received.\n";

	FLAG_PROGRAM_RUNNING = false;
	CleanUpOnExit( dpy );
	exit( signum );
}



/**
 * @brief Main program function
 * @return 0 on successful close
 */
int main() {


	// Register signal handler for SIGINT (Ctrl+C)
	std::signal( SIGINT, signalHandler );

	// Open connection to the X server
	dpy = XOpenDisplay( nullptr );

	// Ensure all OpenCV objects closed
	cv::destroyAllWindows();



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

	FLAG_SERIAL_ENABLED		  = Serial.GetStatus();
	OutputDisplay.FLAG_SERIAL = FLAG_SERIAL_ENABLED;


	cv::namedWindow( "Target Window", cv::WINDOW_NORMAL );

	// cv::setWindowProperty( "Target Window", cv::WindowPropertyFlags::WND_PROP_TOPMOST, 1.0 );
	cv::moveWindow( "Target Window", 3440, 0 );
	cv::setWindowProperty( "Target Window", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );


	cv::namedWindow( "NURing Tracking", cv::WINDOW_AUTOSIZE );
	cv::moveWindow( "NURing Tracking", 1920, 0 );



	while( FLAG_PROGRAM_RUNNING ) {


		auto [x, y, clicked] = getMouseInfo( dpy );
		OutputDisplay.SetMouseXY( cv::Point3i( x, y, int( clicked ) ) );

		// std::cout << "Mouse Position: (" << x << ", " << y << ")" << std::endl;
		// std::cout << "Left Button " << ( clicked ? "Pressed" : "Not Pressed" ) << std::endl;

		// Parse input ( pollKey is needed for openCv to loop properly )
		ParseInput( cv::pollKey() );

		// Get current frame and find aruco tags
		Capture.GetFrame();
		Capture.FindTags();

		// Switch---case for different experiments

		// Show updated image
		cv::imshow( "Target Window", matBlank );
		cv::imshow( "NURing Tracking", OutputDisplay.Update( Capture.matFrame, Capture ) );


		// Send serial packet
		if( Capture.markerFound ) {

			// std::cout << "Active: " << Capture.activeMarker << "\n";

			// If motors have been enabled, start sending serial packet
			if( FLAG_MOTORS_ENABLED && FLAG_SERIAL_ENABLED ) {
				// outgoingPacket = "x" + std::to_string( Capture.Markers[Capture.activeMarker].error3D.x ) + "y" + std::to_string( -Capture.Markers[Capture.activeMarker].error3D.y ) + "z" + std::to_string( Capture.Markers[Capture.activeMarker].error3D.z ) + "\n";
				outgoingPacket = "r" + std::to_string( Capture.Markers[Capture.activeMarker].errorMagnitudeNorm ) + "t" + std::to_string( int( Capture.Markers[Capture.activeMarker].errorHeading * 100 ) ) + "z" + std::to_string( Capture.Markers[Capture.activeMarker].error3D.z ) + "\n";
				Serial.Send( outgoingPacket );
				OutputDisplay.setSerialString( outgoingPacket.substr( 0, outgoingPacket.length() - 1 ) );
			} else if( !FLAG_MOTORS_ENABLED && FLAG_SERIAL_ENABLED ) {
				outgoingPacket = "w\n";
				Serial.Send( outgoingPacket );
				OutputDisplay.setSerialString( outgoingPacket.substr( 0, outgoingPacket.length() - 1 ) );
			}
		}

		// Check program frequency
		OutputDisplay.setFrequency( Timing.CheckFrequency() );
	}

	// Closing message
	std::cout << "Program shutting down... \n";
	CleanUpOnExit( dpy );
	return 0;
}



void ParseInput( int key ) {

	// Ignore mouse clicks or arrows
	if( key != -1 )
	//17825819
	{
		std::cout << "Detected key press = " << key << "\n";

		switch( key ) {
		case 1179675:	 // ESC key
			OutputDisplay.setStatusString( "CAPS LOCK IS ON!" );
			// std::cout << "Stop key pressed, exiting.\n";
			// RUNNING = false;
			break;
		case 17825819:	  // ESC key 1048603
			if( FLAG_SERIAL_ENABLED ) {
				outgoingPacket = "C\n";
				Serial.Send( outgoingPacket );
				OutputDisplay.setSerialString( outgoingPacket.substr( 0, outgoingPacket.length() - 1 ) );
			}
			FLAG_SERIAL_ENABLED = false;
			OutputDisplay.setStatusString( "Stop key pressed, exiting." );
			FLAG_PROGRAM_RUNNING = false;
			break;
		case 1048603:	 // ESC key 1048603
			if( FLAG_SERIAL_ENABLED ) {
				outgoingPacket = "C\n";
				Serial.Send( outgoingPacket );
				OutputDisplay.setSerialString( outgoingPacket.substr( 0, outgoingPacket.length() - 1 ) );
			}
			FLAG_SERIAL_ENABLED = false;
			OutputDisplay.setStatusString( "Stop key pressed, exiting." );
			FLAG_PROGRAM_RUNNING = false;
			break;
		case 1048625:	 // 1
			OutputDisplay.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 1 ? "0." : "1." ) );
			( Capture.activeMarker == 1 ? Capture.activeMarker = 0 : Capture.activeMarker = 1 );
			break;
		case 1048626:	 // 2
			OutputDisplay.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 2 ? "0." : "2." ) );
			( Capture.activeMarker == 2 ? Capture.activeMarker = 0 : Capture.activeMarker = 2 );
			break;
		case 1048627:	 // 3
			OutputDisplay.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 3 ? "0." : "3." ) );
			( Capture.activeMarker == 3 ? Capture.activeMarker = 0 : Capture.activeMarker = 3 );
			break;
		case 1048628:	 // 4
			OutputDisplay.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 4 ? "0." : "4." ) );
			( Capture.activeMarker == 4 ? Capture.activeMarker = 0 : Capture.activeMarker = 4 );
			break;
		case 1048629:	 // 5
			OutputDisplay.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 5 ? "0." : "5." ) );
			( Capture.activeMarker == 5 ? Capture.activeMarker = 0 : Capture.activeMarker = 5 );
			break;
		case 1048630:	 // 6
			OutputDisplay.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 6 ? "0." : "6." ) );
			( Capture.activeMarker == 6 ? Capture.activeMarker = 0 : Capture.activeMarker = 6 );
			break;
		case 1048631:	 // 7
			OutputDisplay.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 7 ? "0." : "7." ) );
			( Capture.activeMarker == 7 ? Capture.activeMarker = 0 : Capture.activeMarker = 7 );
			break;
		case 1048632:	 // 8
			OutputDisplay.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 8 ? "0." : "8." ) );
			( Capture.activeMarker == 8 ? Capture.activeMarker = 0 : Capture.activeMarker = 8 );
			break;
		case 1048633:	 // 9
			OutputDisplay.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 9 ? "0." : "9." ) );
			( Capture.activeMarker == 9 ? Capture.activeMarker = 0 : Capture.activeMarker = 9 );
			break;
		case 1048634:	 // 10
			OutputDisplay.setStatusString( "Updated active marker to #" + std::string( Capture.activeMarker == 10 ? "0." : "10." ) );
			( Capture.activeMarker == 10 ? Capture.activeMarker = 0 : Capture.activeMarker = 10 );
			break;
		case 1048672:	 // ~
			OutputDisplay.setStatusString( "Disabling all active markers." );
			Capture.activeMarker = 0;
			break;
		case 1048677:	 // E
			FLAG_MOTORS_ENABLED = !FLAG_MOTORS_ENABLED;
			if( FLAG_MOTORS_ENABLED ) {
				if( FLAG_SERIAL_ENABLED ) {
					outgoingPacket = "e\n";
					Serial.Send( outgoingPacket );
					OutputDisplay.setSerialString( outgoingPacket.substr( 0, outgoingPacket.length() - 1 ) );
				}
				OutputDisplay.FLAG_AMPLIFIERS = true;
				OutputDisplay.setStatusString( "Amplifiers enabled." );
			} else {
				if( FLAG_SERIAL_ENABLED ) {
					outgoingPacket = "d\n";
					Serial.Send( outgoingPacket );
					OutputDisplay.setSerialString( outgoingPacket.substr( 0, outgoingPacket.length() - 1 ) );
				}
				OutputDisplay.FLAG_AMPLIFIERS = false;
				OutputDisplay.setStatusString( "Amplifiers disabled." );
			}
			break;
		case 1048691:	 // S
			FLAG_SERIAL_ENABLED = !FLAG_SERIAL_ENABLED;
			if( FLAG_SERIAL_ENABLED ) {
				OutputDisplay.FLAG_SERIAL = true;
				OutputDisplay.setStatusString( "Enabling Serial output." );
			} else {
				OutputDisplay.FLAG_SERIAL = false;
				OutputDisplay.setStatusString( "Disabling Serial output." );
			}
			break;
		case 1048696:	 // X
			OutputDisplay.setStatusString( "Software E-Stop Triggered. Amplifiers and serial output disabled." );
			if( FLAG_SERIAL_ENABLED ) {
				outgoingPacket = "X\n";
				Serial.Send( outgoingPacket );
				OutputDisplay.setSerialString( outgoingPacket.substr( 0, outgoingPacket.length() - 1 ) );
			}
			FLAG_SERIAL_ENABLED			  = false;
			FLAG_MOTORS_ENABLED			  = false;
			OutputDisplay.FLAG_SERIAL	  = FLAG_SERIAL_ENABLED;
			OutputDisplay.FLAG_AMPLIFIERS = FLAG_MOTORS_ENABLED;
			break;
		}
	}
}