// Call to class header
#include "InputClass.h"
#include <chrono>
#include <iomanip>	  // For padding zeroes
#include <thread>

// System data manager
#include "SystemDataManager.h"

#include "FittsClass.h"

// Constructor
InputClass::InputClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	std::cout << "InputClass:   Input initialized.\n";
}


void InputClass::ParseInput( int key ) {

	// Ignore mouse clicks or arrows
	if ( key != 255 ) {

		// Uncomment to be able to read detected key presses
		std::cout << "InputClass:   Detected key press = " << key << "\n";

		switch ( key ) {
		case 27:	// ESC key 1048603
			shared->FLAG_SERIAL0_ENABLED  = false;
			shared->FLAG_AMPLIFIERS_READY = false;
			shared->serialPacket0		  = "DX\n";
			shared->FLAG_PACKET_WAITING	  = true;
			shared->displayString		  = "InputClass: Shutting down...";
			shared->FLAG_SERIAL0_ENABLED  = false;
			shared->FLAG_SERIAL0_OPEN	  = false;

			shared->displayString = "InputClass: Stop key pressed, exiting.";
			cv::destroyAllWindows();
			std::cout << "shutdown initiated.\n";
			shared->serialPacket0		= "DX\n";
			shared->FLAG_PACKET_WAITING = true;
			shared->FLAG_SHUTTING_DOWN	= true;
			break;
		// case 177:	 // Numpad 1
		case '1':	 // 1
			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->arucoActiveID == 1 ? "0." : "1." );
			( shared->arucoActiveID == 1 ? shared->arucoActiveID = 0 : shared->arucoActiveID = 1 );
			break;
		// case 178:	 // Numpad 2
		case '2':	 // 2
			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->arucoActiveID == 2 ? "0." : "2." );
			( shared->arucoActiveID == 2 ? shared->arucoActiveID = 0 : shared->arucoActiveID = 2 );
			break;
		// case 179:	 // Numpad 3
		case '3':	 // 3
			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->arucoActiveID == 3 ? "0." : "3." );
			( shared->arucoActiveID == 3 ? shared->arucoActiveID = 0 : shared->arucoActiveID = 3 );
			break;
		// case 180:	 // Numpad 4
		case '4':	 // 4
			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->arucoActiveID == 4 ? "0." : "4." );
			( shared->arucoActiveID == 4 ? shared->arucoActiveID = 0 : shared->arucoActiveID = 4 );
			break;
		// case 181:	 // Numpad 5
		case '5':	 // 5
			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->arucoActiveID == 5 ? "0." : "5." );
			( shared->arucoActiveID == 5 ? shared->arucoActiveID = 0 : shared->arucoActiveID = 5 );
			break;
		case '`':	 // ` (tilde)
			shared->displayString = "InputClass: Disabling all active markers.";
			shared->arucoActiveID = 0;
			break;
		case 'e':	 // E
			shared->FLAG_AMPLIFIERS_READY = !shared->FLAG_AMPLIFIERS_READY;
			if ( shared->FLAG_AMPLIFIERS_READY ) {
				// shared->serialPacket		= "e\n";
				// shared->FLAG_PACKET_WAITING = true;
				shared->displayString = "InputClass: Amplifiers enabled.";
			} else {
				shared->serialPacket0		= "DX\n";
				shared->FLAG_PACKET_WAITING = true;
				shared->displayString		= "InputClass: Amplifiers disabled.";
			}
			break;
		case 's':	 // S
			shared->FLAG_SERIAL0_ENABLED = !shared->FLAG_SERIAL0_ENABLED;
			if ( shared->FLAG_SERIAL0_ENABLED ) {
				shared->displayString		 = "InputClass: Enabling serial output.";
				shared->FLAG_SERIAL0_ENABLED = true;
			} else {
				shared->serialPacket0		 = "DX\n";
				shared->FLAG_PACKET_WAITING	 = true;
				shared->FLAG_SERIAL0_ENABLED = false;
				shared->displayString		 = "InputClass: Disabling serial output.";
			}
			break;
		case 'x':	 // X
			shared->displayString = "InputClass: Software E-Stop Triggered. Amplifiers and serial output disabled.";
			if ( shared->FLAG_SERIAL0_OPEN ) {
				shared->serialPacket0		= "DX\n";
				shared->FLAG_PACKET_WAITING = true;
			}
			shared->FLAG_AMPLIFIERS_READY = false;
			break;
		case 'r':
			// shared->fittsTestStarted = false;
			// Fitts.StartTest();
			// OutputDisplay.setStatusString( "Randomizing Fitts Test Marker." );
			// 		}
			// 	}
			// }
		case 91: {	  // '['
			shared->controllerK -= 0.001;
			shared->displayString = "InputClass: Decreasing K to " + std::to_string( shared->controllerK ) + ".";
			break;
		}
		case 93:	// ']'
			shared->controllerK += 0.001;
			shared->displayString = "InputClass: Increasing K to " + std::to_string( shared->controllerK ) + ".";
			break;
		case 59:	// ';'
			shared->controllerB -= 0.001;
			shared->displayString = "InputClass: Decreasing B to " + std::to_string( shared->controllerB ) + ".";
			break;
		case 39:	// '''
			shared->controllerB += 0.001;
			shared->displayString = "InputClass: Increasing B to " + std::to_string( shared->controllerB ) + ".";
			break;
		case 122:	 // 'z'
			shared->vizClear	  = true;
			shared->displayString = "InputClass: Resetting visualization trails.";
			break;
		case 99:	// 'c'
			shared->TASK_NAME = "CALIBRATE";
			// shared->TASK_RUNNING  = true;
			shared->calibrationComplete = false;
			shared->displayString		= "InputClass: Starting calibration.";
			break;
		case 118:	 // 'v'
			shared->vizEnabled	  = !shared->vizEnabled;
			shared->displayString = "InputClass: 3D Visualization " + std::string( shared->vizEnabled ? "enabled." : "disabled." );
			break;
		// case 97:	// 'a'
		// 	shared->angleEnabled  = !shared->angleEnabled;
		// 	shared->displayString = "InputClass: Angle Visualization " + std::string( shared->angleEnabled ? "enabled." : "disabled." );
		// 	break;
		case 61:	// '-'
			shared->controllerCompZ -= 20;
			shared->displayString = "InputClass: Z-Comp set to " + shared->controllerCompZ;
			break;
		case 45:	// '+'
			shared->controllerCompZ += 20;
			shared->displayString = "InputClass: Z-Comp set to " + shared->controllerCompZ;
			break;

		case 102:	 // 'f'
			// Run fitts-law test
			shared->TASK_RUNNING = false;
			shared->TASK_NAME	 = "FITTS";
			// shared->TASK_RUNNING = true;
			// shared->fittsTestStarted = false;
			shared->displayString = "InputClass: Starting fitts-law test.";
			break;
		case 108:	 // l
			shared->FLAG_LOGGING_STARTED = !shared->FLAG_LOGGING_STARTED;
			shared->displayString		 = "InputClass: Started logging.";
			std::cout << "InputClass: Logging started.\n";
			break;
		case 113:	 // x
			cv::destroyAllWindows();
			break;
		}
		// std::cout << "\n";
	} else {
		shared->serialPacket0		= "DX\n";
		shared->FLAG_PACKET_WAITING = true;
	}
}
