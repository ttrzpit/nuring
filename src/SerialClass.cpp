//[DOXYGEN]
/**
 * @file SerialClass.cpp
 * @author your name (you@domain.com)
 * @brief Manage serial communications
 * @version 0.1
 * @date 2025-05-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

// Call to class header
#include "SerialClass.h"

// System data manager
#include "SystemDataManager.h"

/**
 * @brief Construct a new Serial Class:: Serial Class object
 * 
 * @param ctx Handle to data manager
 * @param nPorts Number of ports to activate
 */
SerialClass::SerialClass( SystemDataManager& ctx, uint8_t nPorts )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	nPortsOpen = nPorts;
	if ( nPortsOpen == 1 ) {
		InitializePort0();
		shared->serialPortsOpen = 1;
	} else if ( nPortsOpen == 2 ) {
		InitializePort0();
		InitializePort1();
		shared->serialPortsOpen = 2;
	} else {
		std::cout << "SerialClass: Invalid number of ports to open!";
	}
}



/**
 * @brief Initialize first serial port for sending data
 * 
 */
void SerialClass::InitializePort0() {

	// Open serial port
	serialPort0 = open( CONFIG_SERIAL_PORT_0.c_str(), O_RDWR | O_NONBLOCK );

	// Get existing settings
	if ( tcgetattr( serialPort0, &tty0 ) != 0 ) {
		printf( "SerialClass:  Error %i from tctgetattr: %s\n", errno, strerror( errno ) );
		shared->FLAG_SERIAL0_OPEN = false;
	} else {
		std::cout << "SerialClass:  Serial interface " << CONFIG_SERIAL_PORT_0 << " (PC -> Teensy) connected.\n";
		shared->FLAG_SERIAL0_OPEN = true;
	}

	// Configure port
	tty0.c_cflag &= ~PARENB;		   // Clear parity bit, disabling parity (most common)
	tty0.c_cflag &= ~CSTOPB;		   // Clear stop field, only one stop bit used in communication (most common)
	tty0.c_cflag &= ~CSIZE;			   // Clear all bits that set the data size
	tty0.c_cflag |= CS8;			   // 8 bits per byte (most common)
	tty0.c_cflag &= ~CRTSCTS;		   // Disable RTS/CTS hardware flow control (most common)
	tty0.c_cflag |= CREAD | CLOCAL;	   // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty0.c_lflag &= ~ICANON;
	tty0.c_lflag &= ~ECHO;															   // Disable echo
	tty0.c_lflag &= ~ECHOE;															   // Disable erasure
	tty0.c_lflag &= ~ECHONL;														   // Disable new-line echo
	tty0.c_lflag &= ~ISIG;															   // Disable interpretation of INTR, QUIT and SUSP
	tty0.c_iflag &= ~( IXON | IXOFF | IXANY );										   // Turn off s/w flow ctrl
	tty0.c_iflag &= ~( IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL );	   // Disable any special handling of received bytes

	tty0.c_oflag &= ~OPOST;	   // Prevent special interpretation of output bytes (e.g. newline chars)
	tty0.c_oflag &= ~ONLCR;	   // Prevent conversion of newline to carriage return/line feed

	tty0.c_cc[VTIME] = 2;	 // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty0.c_cc[VMIN]	 = 0;

	// Set in/out baud rate to be 9600
	cfsetispeed( &tty0, B1000000 );
	cfsetospeed( &tty0, B1000000 );

	// Save tty0 settings, also checking for error
	if ( tcsetattr( serialPort0, TCSANOW, &tty0 ) != 0 ) {
		printf( "SerialClass:  Error %i from tcsetattr: %s\n", errno, strerror( errno ) );
	}

	// Send handshake packet to Teensy serial
	std::string msg	   = "C_READY\n";
	ssize_t		packet = write( serialPort0, msg.c_str(), msg.size() );
}


/**
 * @brief Initialize second serial port
 * 
 */
void SerialClass::InitializePort1() {
	// Open serial port
	serialPort1 = open( CONFIG_SERIAL_PORT_1.c_str(), O_RDWR | O_NONBLOCK );

	// Get existing settings
	if ( tcgetattr( serialPort1, &tty1 ) != 0 ) {
		printf( "SerialClass:  Error %i from tctgetattr: %s\n", errno, strerror( errno ) );
		shared->FLAG_SERIAL1_OPEN = false;
	} else {
		std::cout << "SerialClass:  Serial interface " << CONFIG_SERIAL_PORT_1 << " (Teensy -> PC) connected.\n";
		shared->FLAG_SERIAL1_OPEN = true;
	}

	// Configure port
	tty1.c_cflag &= ~PARENB;														   // Clear parity bit, disabling parity (most common)
	tty1.c_cflag &= ~CSTOPB;														   // Clear stop field, only one stop bit used in communication (most common)
	tty1.c_cflag &= ~CSIZE;															   // Clear all bits that set the data size
	tty1.c_cflag |= CS8;															   // 8 bits per byte (most common)
	tty1.c_cflag &= ~CRTSCTS;														   // Disable RTS/CTS hardware flow control (most common)
	tty1.c_cflag |= CREAD | CLOCAL;													   // Turn on READ & ignore ctrl lines (CLOCAL = 1)
	tty1.c_lflag &= ~ICANON;														   // Canonical input
	tty1.c_lflag &= ~ECHO;															   // Disable echo
	tty1.c_lflag &= ~ECHOE;															   // Disable erasure
	tty1.c_lflag &= ~ECHONL;														   // Disable new-line echo
	tty1.c_lflag &= ~ISIG;															   // Disable interpretation of INTR, QUIT and SUSP
	tty1.c_iflag &= ~( IXON | IXOFF | IXANY );										   // Turn off s/w flow ctrl
	tty1.c_iflag &= ~( IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL );	   // Disable any special handling of received bytes
	tty1.c_oflag &= ~OPOST;															   // Prevent special interpretation of output bytes (e.g. newline chars)
	tty1.c_oflag &= ~ONLCR;															   // Prevent conversion of newline to carriage return/line feed
	tty1.c_cc[VTIME] = 2;															   // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty1.c_cc[VMIN]	 = 0;

	// Set in/out baud rate to be 9600
	cfsetispeed( &tty1, B1000000 );
	cfsetospeed( &tty1, B1000000 );

	// Save tty0 settings, also checking for error
	if ( tcsetattr( serialPort1, TCSANOW, &tty1 ) != 0 ) {
		printf( "SerialClass:  Error %i from tcsetattr: %s\n", errno, strerror( errno ) );
	}

	// Send handshake packet to Teensy serial
	// std::string msg	   = "C_READY\n";
	// ssize_t		packet = write( serialPort0, msg.c_str(), msg.size() );
}



void SerialClass::Send( const std::string& msg ) {
	shared->serialPacket0 = msg + "\n";
	ssize_t bytesWritten  = write( serialPort0, shared->serialPacket0.c_str(), shared->serialPacket0.size() );
	if ( bytesWritten < 0 ) {
		std::cerr << "SerialClass:  Serial packet not sent.\n";
	}
	// shared->serialPacket0 = "";
}



void SerialClass::Close() {
	close( serialPort0 );
}


int8_t SerialClass::Sign( int val ) {
	return ( ( val < 0 ) ? 1 : 0 );
}


void SerialClass::CheckForPacket() {

	// Pre-allocate memory
	// memset( buffer, 0, sizeof( buffer ) );	  // Unnecessary

	// Read serial bytes
	int16_t bytesRead = read( serialPort1, buffer, sizeof( buffer ) - 1 );

	// Check for valid packet
	if ( bytesRead > 0 ) {

		// Append
		readBuffer.append( buffer, bytesRead );

		// Look for new line character
		size_t newLinePosition = readBuffer.find( '\n' );

		// Read packet if terminator '\n' found
		if ( newLinePosition != std::string::npos ) {

			std::string packet = readBuffer.substr( 0, newLinePosition );
			// std::cout << "Size: " << packet.size() << "   Head: " << packet[0] << "   Term: " << packet[packet.size() - 2] << "   A: " << packet.substr( 1, 2 ) << "   B: " << packet.substr( 4, 2 ) << "   C: " << packet.substr( 7, 2 ) << "\n";

			// Make sure packet is valid before trying anything
			if ( packet.size() >= 1 && packet[0] == 'T' && packet[packet.size() - 2] == 'X' ) {

				try {

					// 01234567890123
					// TE0A0B0C0D000X

					// Save packet
					shared->serialPacket1 = packet;

					// Parse packet
					shared->FLAG_TEENSY_SERIAL_RESPONDING  = true;
					shared->FLAG_TEENSY_AMPLIFIER_ENABLED  = bool( std::stoi( packet.substr( 2, 1 ) ) );
					shared->teensyMeasuredAmplfierOutput.x = std::stoi( packet.substr( 4, 1 ) );
					shared->teensyMeasuredAmplfierOutput.y = std::stoi( packet.substr( 6, 1 ) );
					shared->teensyMeasuredAmplfierOutput.z = std::stoi( packet.substr( 8, 1 ) );
					shared->angleTheta					   = ( std::stoi( packet.substr( 10, 1 ) ) == 1 ? -1 : 1 ) * std::stoi( packet.substr( 11, 2 ) );
					// std::cout << "T: " << shared->timingTimestamp << " Theta: " << shared->angleTheta << "\n";
					// Debug
					// std::cout << "E:" << shared->teensyEnabled << " A:" << shared->teensyABC.x << " B:" << shared->teensyABC.y << " C:" << shared->teensyABC.z << "\n";

				} catch ( const std::exception& e ) {
					// std::cerr << "SerialClass:  Error parsing packet [" << readBuffer.substr( 0, newLinePosition ) << "] on serial1: " << e.what() << "\n";
					// shared->controllerActive = false;
					shared->serialPacket1	 = "INVALID!";
					shared->FLAG_TEENSY_SERIAL_RESPONDING = false;
				}

				// Remove processed packet
				readBuffer.erase( 0, newLinePosition + 1 );	   // 🟡 spacing cleaned

			} else {
				// std::cerr << "SerialClass:  Error parsing packet [" << readBuffer.substr( 0, newLinePosition ) << "] on serial1: " << "\n";
				shared->serialPacket1				  = "INVALID!";
				shared->FLAG_TEENSY_SERIAL_RESPONDING = false;
				// shared->controllerActive = false;
			}

			// Remove processed packet
			readBuffer.erase( 0, newLinePosition + 1 );
		}

	} else if ( bytesRead < 0 && errno != EAGAIN ) {
		std::cerr << "SerialClass:  Serial1 read error: " << strerror( errno ) << "\n";
	}
	
}

void SerialClass::ParseIncomingPacket( std::string packet ) { }