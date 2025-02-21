// Call to class header
#include "InputClass.h"

// System data manager
#include "SystemDataManager.h"

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
		// std::cout << "InputClass:   Detected key press = " << key << ", ";

		switch ( key ) {
		case 27:	// ESC key 1048603
			if ( shared->FLAG_SERIAL_OPEN ) {
				shared->serialPacket  = "C\n";
				shared->displayString = shared->serialPacket.substr( 0, shared->serialPacket.length() - 1 );
			}
			shared->FLAG_SERIAL_OPEN = false;
			shared->displayString	 = "InputClass: Stop key pressed, exiting.";
			cv::destroyAllWindows();
			std::cout << "shutdown initiated.\n";
			shared->FLAG_MAIN_RUNNING = false;
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
		// case 182:	 // Numpad 6
		case '6':	 // 6
			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->arucoActiveID == 6 ? "0." : "6." );
			( shared->arucoActiveID == 6 ? shared->arucoActiveID = 0 : shared->arucoActiveID = 6 );
			break;
		// case 183:	 // Numpad 7
		case '7':	 // 7
			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->arucoActiveID == 7 ? "0." : "7." );
			( shared->arucoActiveID == 7 ? shared->arucoActiveID = 0 : shared->arucoActiveID = 7 );
			break;
		// case 184:	 // Numpad 8
		case '8':	 // 8
			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->arucoActiveID == 8 ? "0." : "8." );
			( shared->arucoActiveID == 8 ? shared->arucoActiveID = 0 : shared->arucoActiveID = 8 );
			break;
		// case 185:	 // Numpad 9
		case '9':	 // 9
			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->arucoActiveID == 9 ? "0." : "9." );
			( shared->arucoActiveID == 9 ? shared->arucoActiveID = 0 : shared->arucoActiveID = 9 );
			break;
		// case 176:	 // Numpad 0
		case '0':	 // 10
			shared->displayString = "InputClass: Updated active marker to #" + std::string( shared->arucoActiveID == 10 ? "0." : "10." );
			( shared->arucoActiveID == 10 ? shared->arucoActiveID = 0 : shared->arucoActiveID = 10 );
			break;

		case '`':	 // ` (tilde)
			shared->displayString = "InputClass: Disabling all active markers.";
			shared->arucoActiveID = 0;
			break;
		case 'e':	 // E
			shared->FLAG_AMPLIFIERS_READY = !shared->FLAG_AMPLIFIERS_READY;
			if ( shared->FLAG_AMPLIFIERS_READY ) {
				shared->serialPacket		= "e\n";
				shared->FLAG_PACKET_WAITING = true;
				shared->displayString		= "InputClass: Amplifiers enabled.";
			} else {
				shared->serialPacket		= "d\n";
				shared->FLAG_PACKET_WAITING = true;
				shared->displayString		= "InputClass: Amplifiers disabled.";
			}
			break;
		case 's':	 // S
			shared->FLAG_SERIAL_OPEN = !shared->FLAG_SERIAL_OPEN;
			if ( shared->FLAG_SERIAL_OPEN ) {
				shared->displayString = "InputClass: Enabling serial output.";
			} else {
				shared->displayString = "InputClass: Disabling serial output.";
			}
			break;
		case 'x':	 // X
			shared->displayString = "InputClass: Software E-Stop Triggered. Amplifiers and serial output disabled.";
			if ( shared->FLAG_SERIAL_OPEN ) {
				shared->serialPacket = "X\n";
			}
			shared->FLAG_SERIAL_OPEN	  = false;
			shared->FLAG_AMPLIFIERS_READY = false;
			break;
			// 		case 'r':
			// 			Fitts.testStarted = false;
			// 			Fitts.StartTest( matBlank );
			// 			OutputDisplay.setStatusString( "Randomizing Fitts Test Marker." );
			// 		}
			// 	}
			// }
		case 176:	 // Numpad 0
			shared->TASK_COMMAND = 'r';
			break;
		case 't':
			if ( shared->ACTIVE_TASK < 3 ) {
				shared->ACTIVE_TASK++;
			} else {
				shared->ACTIVE_TASK = 0;
			}
			std::cout << "Test: " << std::to_string( shared->ACTIVE_TASK );
			break;
		}
		// std::cout << "\n";
	}
}
