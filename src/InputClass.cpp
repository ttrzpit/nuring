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
			shared->FLAG_SERIAL0_ENABLED	= false;
			shared->FLAG_AMPLIFIERS_ENABLED = false;
			shared->serialPacket0			= "DX\n";
			shared->FLAG_PACKET_WAITING		= true;
			shared->displayString			= "InputClass: Shutting down...";
			shared->FLAG_SERIAL0_ENABLED	= false;
			shared->FLAG_SERIAL0_OPEN		= false;

			shared->displayString = "InputClass: Stop key pressed, exiting.";
			cv::destroyAllWindows();
			std::cout << "shutdown initiated.\n";
			shared->serialPacket0		= "DX\n";
			shared->FLAG_PACKET_WAITING = true;
			shared->FLAG_SHUTTING_DOWN	= true;
			break;
		// case 177:	 // Numpad 1
		case '1':	 // 1
					 // shared->FLAG_TARGET_FOUND = false ;
			shared->targetActiveID = 1;

			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->targetActiveID == 1 ? "1." : "0." );
			break;
		// case 178:	 // Numpad 2
		case '2':	 // 2
			shared->targetActiveID = 2;
			shared->displayString  = "InputClass: Updated active marker to #" + std::string( shared->targetActiveID == 2 ? "2." : "0." );
			break;
		// case 179:	 // Numpad 3
		case '3':	 // 3
			shared->targetActiveID = 3;
			shared->displayString  = "InputClass: Updated active marker to #" + std::string( shared->targetActiveID == 3 ? "3." : "0." );
			break;
		// case 180:	 // Numpad 4
		case '4':	 // 4
			shared->targetActiveID = 4;
			shared->displayString  = "InputClass: Updated active marker to #" + std::string( shared->targetActiveID == 4 ? "4." : "0." );
			break;
		// case 181:	 // Numpad 5
		case '5':	 // 5
					 // shared->FLAG_TARGET_FOUND = false ;
			shared->targetActiveID = 5;
			shared->displayString  = "InputClass: Updated active marker to #" + std::string( shared->targetActiveID == 5 ? "5." : "0." );

			break;

		case '`':	 // ` (tilde)
			shared->displayString  = "InputClass: Disabling all active markers.";
			shared->targetActiveID = 0;
			break;
		case 'a':	 // E
			shared->FLAG_AMPLIFIERS_ENABLED = !shared->FLAG_AMPLIFIERS_ENABLED;
			if ( shared->FLAG_AMPLIFIERS_ENABLED ) {
				// shared->serialPacket		= "e\n";
				// shared->FLAG_PACKET_WAITING = true;
				shared->displayString = "InputClass: Amplifiers enabled.";
			} else {
				shared->serialPacket0		= "DX\n";
				shared->FLAG_PACKET_WAITING = true;
				shared->displayString		= "InputClass: Amplifiers disabled.";
			}
			break;
		case 's': {	   // s
			// Toggle serial flags
			shared->FLAG_SERIAL0_ENABLED = !shared->FLAG_SERIAL0_ENABLED;
			shared->FLAG_SERIAL1_ENABLED = !shared->FLAG_SERIAL1_ENABLED;
			if ( shared->FLAG_SERIAL0_ENABLED ) {
				shared->displayString		   = "InputClass: Enabling serial IO.";
				shared->FLAG_SERIAL0_ENABLED   = true;
				shared->FLAG_SERIAL1_ENABLED   = true;
				shared->teensyAmplifierEnabled = false;
			} else {
				// shared->serialPacket0		 = "DX\n";
				shared->FLAG_PACKET_WAITING	   = true;
				shared->FLAG_SERIAL0_ENABLED   = false;
				shared->FLAG_SERIAL1_ENABLED   = false;
				shared->teensyAmplifierEnabled = false;
				shared->displayString		   = "InputClass: Disabling serial IO.";
			}
			break;
		}
		case 'x':	 // X
			shared->displayString = "InputClass: Software E-Stop Triggered. Amplifiers and serial output disabled.";
			if ( shared->FLAG_SERIAL0_OPEN ) {
				shared->serialPacket0		= "DX\n";
				shared->FLAG_PACKET_WAITING = true;
			}
			shared->FLAG_AMPLIFIERS_ENABLED = false;
			break;
		case 'r':
		// shared->fittsTestStarted = false;
		// Fitts.StartTest();
		// OutputDisplay.setStatusString( "Randomizing Fitts Test Marker." );
		// 		}
		// 	}
		// }
		case 111: {	   // 'o'
			shared->controllerKp.x -= 2.0f;
			shared->displayString = "InputClass: Decreasing Kp(x) to " + std::to_string( shared->controllerKp.x ) + ".";
			break;
		}
		case 112:	 // 'p'
			shared->controllerKp.x += 2.0f;
			shared->displayString = "InputClass: Increasing Kp(x) to " + std::to_string( shared->controllerKp.x ) + ".";
			break;
		case 91: {	  // '['
			shared->controllerKp.y -= 2.0f;
			shared->displayString = "InputClass: Decreasing Kp(y) to " + std::to_string( shared->controllerKp.y ) + ".";
			break;
		}
		case 93:	// ']'
			shared->controllerKp.y += 2.0f;
			shared->displayString = "InputClass: Increasing Kp(y) to " + std::to_string( shared->controllerKp.y ) + ".";
			break;
		case 109:	 // 'k'
			shared->controllerKd.x -= 0.01;
			shared->displayString = "InputClass: Decreasing Kd(x) to " + std::to_string( shared->controllerKd.x ) + ".";
			break;
		case 108:	 // 'l'
			shared->controllerKd.x += 0.01;
			shared->displayString = "InputClass: Increasing Kd(x) to " + std::to_string( shared->controllerKd.x ) + ".";
			break;

		case 59:	// ';'
			shared->controllerKd.y -= 0.01;
			shared->displayString = "InputClass: Decreasing Kd(y) to " + std::to_string( shared->controllerKp.y ) + ".";
			break;
		case 39:	// '''
			shared->controllerKd.y += 0.01;
			shared->displayString = "InputClass: Increasing Kd(y) to " + std::to_string( shared->controllerKp.y ) + ".";
			break;

		case 183:	 // numpad 7
			shared->controllerTension.x += 0.2f;
			shared->displayString = "InputClass: Increasing Tension A to " + shared->FormatDecimal( shared->controllerTension.x, 2 ) + ".";
			break;
		case 180:	 // numpad 4
			if ( shared->controllerTension.x > 0.2 ) {
				shared->controllerTension.x -= 0.2f;
				shared->displayString = "InputClass: Decreasing Tension A to " + shared->FormatDecimal( shared->controllerTension.x, 2 ) + ".";
			}
			break;
		case 184:	 // numpad 8
			shared->controllerTension.y += 0.2f;
			shared->displayString = "InputClass: Increasing Tension B to " + shared->FormatDecimal( shared->controllerTension.y, 2 ) + ".";
			break;
		case 181:	 // numpad 5
			if ( shared->controllerTension.y > 0.2 ) {
				shared->controllerTension.y -= 0.2f;
				shared->displayString = "InputClass: Decreasing Tension B to " + shared->FormatDecimal( shared->controllerTension.y, 2 ) + ".";
			}
			break;
		case 185:	 // numpad 9
			shared->controllerTension.z += 0.2f;
			shared->displayString = "InputClass: Increasing Tension C to " + shared->FormatDecimal( shared->controllerTension.z, 2 ) + ".";
			break;
		case 182:	 // numpad 6
			if ( shared->controllerTension.z > 0.2 ) {
				shared->controllerTension.z -= 0.2f;
				shared->displayString = "InputClass: Decreasing Tension C to " + shared->FormatDecimal( shared->controllerTension.z, 2 ) + ".";
			}
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
		case 102:	 // 'f'
			// Run fitts-law test
			shared->TASK_RUNNING = false;
			shared->TASK_NAME	 = "FITTS";
			// shared->TASK_RUNNING = true;
			// shared->fittsTestStarted = false;
			shared->displayString = "InputClass: Starting fitts-law test.";
			break;
			// case 108:	 // l
			// 	shared->FLAG_LOGGING_STARTED = !shared->FLAG_LOGGING_STARTED;
			// 	shared->displayString		 = "InputClass: Started logging.";
			// 	std::cout << "InputClass: Logging started.\n";
			// 	break;
			// case 113:	 // q
			// 	cv::destroyAllWindows();
			// 	break;
		}
		// std::cout << "\n";
	} else {
		shared->serialPacket0		= "DX\n";
		shared->FLAG_PACKET_WAITING = true;
	}
}
