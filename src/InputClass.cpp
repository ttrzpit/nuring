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
		case 27: {	  // ESC key 1048603
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
		}
		case '0': {	   // '0'
			shared->targetMarkerActiveID = 0;
			shared->displayString		 = "InputClass: Updated active marker to #" + std::string( shared->targetMarkerActiveID == 0 ? "0." : "0." );
			break;
		}
		case '1': {	   // '1'
			shared->targetMarkerActiveID = 1;
			shared->displayString		 = "InputClass: Updated active marker to #" + std::string( shared->targetMarkerActiveID == 1 ? "1." : "0." );
			break;
		}
		case '2': {	   // '2'
			shared->targetMarkerActiveID = 2;
			shared->displayString		 = "InputClass: Updated active marker to #" + std::string( shared->targetMarkerActiveID == 2 ? "2." : "0." );
			break;
		}
		case '3': {	   // '3'
			shared->targetMarkerActiveID = 3;
			shared->displayString		 = "InputClass: Updated active marker to #" + std::string( shared->targetMarkerActiveID == 3 ? "3." : "0." );
			break;
		}
		case '4': {	   // '4'
			shared->targetMarkerActiveID = 4;
			shared->displayString		 = "InputClass: Updated active marker to #" + std::string( shared->targetMarkerActiveID == 4 ? "4." : "0." );
			break;
		}
		case '5': {	   // '5'
			shared->targetMarkerActiveID = 5;
			shared->displayString		 = "InputClass: Updated active marker to #" + std::string( shared->targetMarkerActiveID == 5 ? "5." : "0." );
			break;
		}
		case '`': {	   // ` (tilde)
			shared->displayString		 = "InputClass: Disabling all active markers.";
			shared->targetMarkerActiveID = 0;
			break;
		}
		case 'a': {	   // 'e'
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
		}
		case 's': {	   // 's'
			// Toggle serial flags
			shared->FLAG_SERIAL0_ENABLED = !shared->FLAG_SERIAL0_ENABLED;
			shared->FLAG_SERIAL1_ENABLED = !shared->FLAG_SERIAL1_ENABLED;
			if ( shared->FLAG_SERIAL0_ENABLED ) {
				shared->displayString				  = "InputClass: Enabling serial IO.";
				shared->FLAG_SERIAL0_ENABLED		  = true;
				shared->FLAG_SERIAL1_ENABLED		  = true;
				shared->FLAG_TEENSY_AMPLIFIER_ENABLED = false;
			} else {
				// shared->serialPacket0		 = "DX\n";
				shared->FLAG_PACKET_WAITING			  = true;
				shared->FLAG_SERIAL0_ENABLED		  = false;
				shared->FLAG_SERIAL1_ENABLED		  = false;
				shared->FLAG_TEENSY_AMPLIFIER_ENABLED = false;
				shared->displayString				  = "InputClass: Disabling serial IO.";
			}
			break;
		}
		case 'x': {	   // 'x'
			shared->displayString = "InputClass: Software E-Stop Triggered. Amplifiers and serial output disabled.";
			if ( shared->FLAG_SERIAL0_OPEN ) {
				shared->serialPacket0		= "DX\n";
				shared->FLAG_PACKET_WAITING = true;
			}
			shared->FLAG_AMPLIFIERS_ENABLED = false;
			break;
		}

		// Reset gains
		case 116: {	   // 'y'
			shared->controllerKp  = cv::Point3f( 0.0f, 0.0f, 0.0f );
			shared->controllerKi  = cv::Point3f( 0.0f, 0.0f, 0.0f );
			shared->controllerKd  = cv::Point3f( 0.0f, 0.0f, 0.0f );
			shared->displayString = "InputClass: Setting gains to 0";
			break;
		}


		// Gains X
		case 'u': {	   // 'u'
			IncrementValueF( "Kp(x)", shared->controllerKp.x, -0.2f );
			break;
		}
		case 'i': {	   // 'i'
			IncrementValueF( "Kp(x)", shared->controllerKp.x, 0.2f );
			break;
		}
		case 'o': {	   // 'o'
			IncrementValueF( "Ki(x)", shared->controllerKi.x, -0.2f );
			break;
		}
		case 'p': {	   // 'p'
			IncrementValueF( "Ki(x)", shared->controllerKi.x, 0.2f );
			break;
		}
		case '[': {	   // '['
			IncrementValueF( "Kd(x)", shared->controllerKd.x, -0.2f );
			break;
		}
		case ']': {	   // ']'
			IncrementValueF( "Kd(x)", shared->controllerKd.x, 0.2f );
			break;
		}

		// Gains Y
		case 'h': {	   // 'h'
			IncrementValueF( "Kp(y)", shared->controllerKp.y, -0.2f );
			break;
		}
		case 'j': {	   // 'j'
			IncrementValueF( "Kp(y)", shared->controllerKp.y, 0.2f );
			break;
		}
		case 'k': {	   // 'k'
			IncrementValueF( "Ki(y)", shared->controllerKi.y, -0.2f );
			break;
		}
		case 'l': {	   // 'l'
			IncrementValueF( "Ki(y)", shared->controllerKi.y, 0.2f );
			break;
		}
		case ';': {	   // ';'
			IncrementValueF( "Kd(y)", shared->controllerKd.y, -0.2f );
			break;
		}
		case 39: {	  // '''
			IncrementValueF( "Kd(y)", shared->controllerKd.y, 0.2f );
			break;
		}

		// case 91: {	  // '['
		// 	shared->controllerKd.x -= 0.01;
		// 	shared->displayString = "InputClass: Decreasing Kd(x) to " + std::to_string( shared->controllerKd.x ) + ".";
		// 	break;
		// }
		// case 93: {	  // ']'
		// 	shared->controllerKd.x += 0.01;
		// 	shared->displayString = "InputClass: Increasing Kd(x) to " + std::to_string( shared->controllerKd.x ) + ".";
		// 	break;
		// }
		// case 107: {	   // 'k'
		// 	shared->controllerKp.y -= 0.20f;
		// 	shared->displayString = "InputClass: Decreasing Kp(y) to " + std::to_string( shared->controllerKp.y ) + ".";
		// 	break;
		// }
		// case 108: {	   // 'l'
		// 	shared->controllerKp.y += 0.20f;
		// 	shared->displayString = "InputClass: Increasing Kp(y) to " + std::to_string( shared->controllerKp.y ) + ".";
		// 	break;
		// }
		// case 59: {	  // ';'
		// 	shared->controllerKd.y -= 0.01;
		// 	shared->displayString = "InputClass: Decreasing Kd(y) to " + std::to_string( shared->controllerKp.y ) + ".";
		// 	break;
		// }
		// case 39: {	  // '''
		// 	shared->controllerKd.y += 0.01;
		// 	shared->displayString = "InputClass: Increasing Kd(y) to " + std::to_string( shared->controllerKp.y ) + ".";
		// 	break;
		// }
		case 183: {	   // numpad 7
			IncrementValueF( "Tension A", shared->controllerTension.x, 0.01 );
			break;
		}
		case 180: {	   // numpad 4
			IncrementValueF( "Tension A", shared->controllerTension.x, -0.01 );
			break;
		}
		case 184: {	   // numpad 8
			IncrementValueF( "Tension B", shared->controllerTension.y, 0.01 );
			break;
		}
		case 181: {	   // numpad 5
			IncrementValueF( "Tension B", shared->controllerTension.y, -0.01 );
			break;
		}
		case 185: {	   // numpad 9
			IncrementValueF( "Tension B", shared->controllerTension.z, 0.01 );
			break;
		}
		case 182: {	   // numpad 6
			IncrementValueF( "Tension B", shared->controllerTension.z, -0.01 );
			break;
		}
		case 122: {	   // 'z'
			shared->vizClear	  = true;
			shared->displayString = "InputClass: Resetting visualization trails.";
			break;
		}
		case 99: {	  // 'c'
			shared->TASK_NAME			= "CALIB";
			shared->calibrationComplete = false;
			shared->displayString		= "InputClass: Starting calibration.";
			break;
		}
		case 118: {	   // 'v'
			shared->vizEnabled	  = !shared->vizEnabled;
			shared->displayString = "InputClass: 3D Visualization " + std::string( shared->vizEnabled ? "enabled." : "disabled." );
			break;
		}
		// case 116: {	   // 't'
		// 	shared->angleEnabled  = !shared->angleEnabled;
		// 	shared->displayString = "InputClass: Angle Visualization " + std::string( shared->angleEnabled ? "enabled." : "disabled." );
		// 	break;
		// }
		case 102: {	   // 'f'
			// Run fitts-law test
			shared->FLAG_TARGET_RESET = true;
			shared->touchDetected	  = 0;
			shared->TASK_RUNNING	  = false;
			shared->TASK_NAME		  = "FITTS";
			shared->TASK_REP_NUMBER++;
			// shared->TASK_RUNNING = true;
			// shared->fittsTestStarted = false;
			shared->displayString = "InputClass: Starting fitts-law test.";
			break;
		}
		case 103: {	   // 'g'
			// End test
			shared->touchDetected = 1;
			// shared->TASK_RUNNING = false;
			// shared->TASK_NAME	 = "FITTS_ANGLE";
			// shared->angleEnabled = true;
			// shared->TASK_RUNNING = true;
			// shared->fittsTestStarted = false;
			shared->displayString = "InputClass: Starting angle test.";
			break;
		}
		case 98: {	  // 'b'
			shared->touchDetected = 1;
			shared->displayString = "InputClass: Ending angle test.";
			break;
		}
		case 113: {	   // 'q'
			shared->serialTrigger = "reset";
			shared->displayString = "InputClass: Encoder reset.";
			break;
		}
		default: {
			shared->displayString = "InputClass: Unknown input.";
			break;
		}
		}
	} else {
		// shared->serialPacket0		= "DX\n";
		shared->FLAG_PACKET_WAITING = true;
	}
}

// PRINT_VAR(shared->vizEnabled) ;


// Functions
void InputClass::IncrementValueF( std::string name, float& target, float increment ) {

	if ( ( target + increment ) >= 0 ) {
		// Increment
		target = target + increment;
	}

	// Update
	shared->displayString = "InputClass: " + name + ( ( increment > 0 ) ? " increased to " : " decreased to " ) + shared->FormatDecimal( target, 1, 2 );
}
