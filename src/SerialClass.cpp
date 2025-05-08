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
	shared->serialPacket0 = "";
}


void SerialClass::Close() {
	close( serialPort0 );
}


bool SerialClass::GetStatus() {
	return shared->FLAG_SERIAL0_OPEN;
}

void SerialClass::Monitor() {
	if ( shared->FLAG_SERIAL0_OPEN && shared->FLAG_PACKET_WAITING ) {
		std::string outgoingPacket = shared->serialPacket0 + "\n";
		ssize_t		bytesWritten   = write( serialPort0, outgoingPacket.c_str(), outgoingPacket.size() );
		if ( bytesWritten < 0 ) {
			std::cerr << "SerialClass:  Serial packet not sent.\n";
		}
		// shared->serialPacket		= "";
		shared->FLAG_PACKET_WAITING = false;
	}
}

/**
 * @brief Pad the given value with a specified number of zeros
 * @param val Value to be padded
 * @param nZeroes Number of padding zeroes
 */
std::string SerialClass::PadValues( int val, int nZeroes ) {
	std::ostringstream sstream;
	sstream << std::setw( nZeroes ) << std::setfill( '0' ) << val;
	return sstream.str();
}

int8_t SerialClass::Sign( int val ) {
	return ( ( val < 0 ) ? 1 : 0 );
}


void SerialClass::CheckForPacket() {

	// Pre-allocate memory
	memset( buffer, 0, sizeof( buffer ) );

	// Read a single byte
	int16_t bytesRead = read( serialPort1, buffer, sizeof( buffer ) - 1 );

	// Check for valid packet
	if ( bytesRead > 0 ) {

		// Append
		readBuffer.append( buffer, bytesRead );

		// Look for new line character
		int8_t newLinePosition = readBuffer.find( '\n' );
		if ( newLinePosition != std::string::npos ) {

			// shared->serialPacket1 = readBuffer.substr( 0, newLinePosition );

			// Make sure packet is valid before trying anything
			if ( readBuffer[0] == 'A' && readBuffer[newLinePosition - 2] == 'X' ) {

				try {

					shared->controllerOutput.x = std::stoi( readBuffer.substr( 1, 2 ) );
					shared->controllerOutput.y = std::stoi( readBuffer.substr( 4, 5 ) );
					shared->controllerOutput.z = std::stoi( readBuffer.substr( 7, 8 ) );
					shared->serialPacket1	   = "A" + PadValues( shared->controllerOutput.x, 2 ) + "B" + PadValues( shared->controllerOutput.y, 2 ) + "C" + PadValues( shared->controllerOutput.z, 2 ) + "\n";

					if ( ( shared->controllerOutput.x + shared->controllerOutput.y + shared->controllerOutput.z ) > 0 ) {
						shared->controllerActive = true;
					} else {
						shared->controllerActive = false;
					}
				} catch ( const std::exception& e ) {
					std::cerr << "SerialClass:  Error parsing packet [" << readBuffer.substr( 0, newLinePosition ) << "] on serial1: " << e.what() << "\n";

					shared->controllerActive = false;
				}

			} else {
				std::cerr << "SerialClass:  Error parsing packet [" << readBuffer.substr( 0, newLinePosition ) << "] on serial1: " << "\n";
				shared->serialPacket1	 = "INVALID!";
				shared->controllerActive = false;
			}


			// shared->serialPacket1 = readBuffer.substr( 0, newLinePosition );
			readBuffer.erase( 0, newLinePosition + 1 );
			// shared->angleTheta = std::stof( shared->serialPacket1 );
			// std::cout << "New Packet: " << shared->serialPacket1 << "\n";
		}

	} else if ( bytesRead < 0 && errno != EAGAIN ) {
		std::cerr << "SerialClass:  Serial1 read error: " << strerror( errno ) << "\n";
	}
}

void SerialClass::ParseIncomingPacket( std::string packet ) { }