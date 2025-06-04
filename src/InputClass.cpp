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
			shared->Serial.isSerialSending		= false;
			shared->Amplifier.isAmplifierActive = false;
			shared->Serial.packetOut			= "DX\n";
			// shared->FLAG_PACKET_WAITING		= true;
			shared->Display.statusString			= "InputClass: Shutting down...";
			shared->Serial.isSerialSending	= false;
			shared->Serial.isSerialSendOpen = false;

			shared->Display.statusString = "InputClass: Stop key pressed, exiting.";
			cv::destroyAllWindows();
			std::cout << "shutdown initiated.\n";
			shared->Serial.packetOut = "DX\n";
			// shared->FLAG_PACKET_WAITING = true;
			shared->Main.isShuttingDown = true;
			break;
		}
		case '0': {	   // '0'
			shared->Telemetry.activeID = 0;
			shared->Display.statusString	   = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 0 ? "0." : "0." );
			break;
		}
		case '1': {	   // '1'
			shared->Telemetry.activeID = 1;
			shared->Display.statusString	   = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 1 ? "1." : "0." );
			break;
		}
		case '2': {	   // '2'
			shared->Telemetry.activeID = 2;
			shared->Display.statusString	   = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 2 ? "2." : "0." );
			break;
		}
		case '3': {	   // '3'
			shared->Telemetry.activeID = 3;
			shared->Display.statusString	   = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 3 ? "3." : "0." );
			break;
		}
		case '4': {	   // '4'
			shared->Telemetry.activeID = 4;
			shared->Display.statusString	   = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 4 ? "4." : "0." );
			break;
		}
		case '5': {	   // '5'
			shared->Telemetry.activeID = 5;
			shared->Display.statusString	   = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 5 ? "5." : "0." );
			break;
		}
		case '`': {	   // ` (tilde)
			shared->Display.statusString	   = "InputClass: Disabling all active markers.";
			shared->Telemetry.activeID = 0;
			break;
		}
		case 'a': {	   // 'e'
			shared->Amplifier.isAmplifierActive = !shared->Amplifier.isAmplifierActive;
			if ( shared->Amplifier.isAmplifierActive ) {
				// shared->serialPacket		= "e\n";
				// shared->FLAG_PACKET_WAITING = true;
				shared->Display.statusString = "InputClass: Amplifiers enabled.";
			} else {
				shared->Serial.packetOut = "DX\n";
				// shared->FLAG_PACKET_WAITING = true;
				shared->Display.statusString = "InputClass: Amplifiers disabled.";
			}
			break;
		}
		case 's': {	   // 's'
			// Toggle serial flags
			shared->Serial.isSerialSending	 = !shared->Serial.isSerialSending;
			shared->Serial.isSerialReceiving = !shared->Serial.isSerialReceiving;
			if ( shared->Serial.isSerialSending ) {
				shared->Display.statusString				 = "InputClass: Enabling serial IO.";
				shared->Serial.isSerialSending		 = true;
				shared->Serial.isSerialReceiving	 = true;
				shared->Teensy.isAmplifierResponding = false;
			} else {
				// shared->serialPacket0		 = "DX\n";
				// shared->FLAG_PACKET_WAITING			  = true;
				shared->Serial.isSerialSending		 = false;
				shared->Serial.isSerialReceiving	 = false;
				shared->Teensy.isAmplifierResponding = false;
				shared->Display.statusString				 = "InputClass: Disabling serial IO.";
			}
			break;
		}
		case 'x': {	   // 'x'
			shared->Display.statusString = "InputClass: Software E-Stop Triggered. Amplifiers and serial output disabled.";
			if ( shared->Serial.isSerialSendOpen ) {
				shared->Serial.packetOut = "DX\n";
				// shared->FLAG_PACKET_WAITING = true;
			}
			shared->Amplifier.isAmplifierActive = false;
			break;
		}

		// Reset gains
		case 116: {	   // 'y'
			shared->controllerKp = cv::Point3f( 0.0f, 0.0f, 0.0f );
			shared->controllerKi = cv::Point3f( 0.0f, 0.0f, 0.0f );
			shared->controllerKd = cv::Point3f( 0.0f, 0.0f, 0.0f );
			shared->Display.statusString = "InputClass: Setting gains to 0";
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
			IncrementValueF( "Kd(x)", shared->controllerKd.x, -0.01f );
			break;
		}
		case ']': {	   // ']'
			IncrementValueF( "Kd(x)", shared->controllerKd.x, 0.01f );
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
			IncrementValueF( "Kd(y)", shared->controllerKd.y, -0.01f );
			break;
		}
		case 39: {	  // '''
			IncrementValueF( "Kd(y)", shared->controllerKd.y, 0.01f );
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
			IncrementValueF( "Tension A", shared->Controller.commandedTensionABC.x, 0.01 );
			break;
		}
		case 180: {	   // numpad 4
			IncrementValueF( "Tension A", shared->Controller.commandedTensionABC.x, -0.01 );
			break;
		}
		case 184: {	   // numpad 8
			IncrementValueF( "Tension B", shared->Controller.commandedTensionABC.y, 0.01 );
			break;
		}
		case 181: {	   // numpad 5
			IncrementValueF( "Tension B", shared->Controller.commandedTensionABC.y, -0.01 );
			break;
		}
		case 185: {	   // numpad 9
			IncrementValueF( "Tension B", shared->Controller.commandedTensionABC.z, 0.01 );
			break;
		}
		case 182: {	   // numpad 6
			IncrementValueF( "Tension B", shared->Controller.commandedTensionABC.z, -0.01 );
			break;
		}
		case 122: {	   // 'z'
			shared->vizClear	 = true;
			shared->Display.statusString = "InputClass: Resetting visualization trails.";
			break;
		}
		case 99: {	  // 'c'
			shared->Task.name			= "CALIB";
			shared->calibrationComplete = false;
			shared->Display.statusString		= "InputClass: Starting calibration.";
			break;
		}
		case 118: {	   // 'v'
			shared->vizEnabled	 = !shared->vizEnabled;
			shared->Display.statusString = "InputClass: 3D Visualization " + std::string( shared->vizEnabled ? "enabled." : "disabled." );
			break;
		}
		// case 116: {	   // 't'
		// 	shared->angleEnabled  = !shared->angleEnabled;
		// 	shared->displayString = "InputClass: Angle Visualization " + std::string( shared->angleEnabled ? "enabled." : "disabled." );
		// 	break;
		// }
		case 102: {	   // 'f'
			// Run fitts-law test
			shared->Telemetry.isTargetReset = true;
			shared->Controller.isRampingUp	= true;
			shared->Touchscreen.isTouched	= 0;
			shared->Task.isRunning			= false;
			shared->Task.name				= "FITTS";
			shared->Task.repetitionNumber++;
			// shared->TASK_RUNNING = true;
			// shared->fittsTestStarted = false;
			shared->Display.statusString = "InputClass: Starting fitts-law test.";
			break;
		}
		case 103: {	   // 'g'
			// End test
			shared->Touchscreen.isTouched = 1;
			// shared->TASK_RUNNING = false;
			// shared->TASK_NAME	 = "FITTS_ANGLE";
			// shared->angleEnabled = true;
			// shared->TASK_RUNNING = true;
			// shared->fittsTestStarted = false;
			shared->Display.statusString = "InputClass: Starting angle test.";
			break;
		}
		case 98: {	  // 'b'
			shared->Touchscreen.isTouched = 1;
			shared->Display.statusString		  = "InputClass: Ending angle test.";
			break;
		}
		case 113: {	   // 'q'
			shared->serialTrigger = "reset";
			shared->Display.statusString  = "InputClass: Encoder reset.";
			break;
		}
		default: {
			shared->Display.statusString = "InputClass: Unknown input.";
			break;
		}
		}
	} else {
		// shared->serialPacket0		= "DX\n";
		// shared->FLAG_PACKET_WAITING = true;
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
	shared->Display.statusString = "InputClass: " + name + ( ( increment > 0 ) ? " increased to " : " decreased to " ) + shared->FormatDecimal( target, 1, 2 );
}
