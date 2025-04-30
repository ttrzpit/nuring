// Call to class header
#include "SerialClass.h"

// System data manager
#include "SystemDataManager.h"

// Constructor
SerialClass::SerialClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	Initialize();
}



/**
 * @brief Initialize ArUco detector
 */
void SerialClass::Initialize() {

	// Open serial port
	serialPort = open( CONFIG_SERIAL_PORT.c_str(), O_RDWR | O_NONBLOCK );

	// Get existing settings
	if ( tcgetattr( serialPort, &tty ) != 0 ) {
		printf( "SerialClass:  Error %i from tctgetattr: %s\n", errno, strerror( errno ) );
		shared->FLAG_SERIAL_OPEN = false;
	} else {
		std::cout << "SerialClass:  Serial interface initialized.\n";
		shared->FLAG_SERIAL_OPEN = true;
	}



	// Configure port
	tty.c_cflag &= ~PARENB;			  // Clear parity bit, disabling parity (most common)
	tty.c_cflag &= ~CSTOPB;			  // Clear stop field, only one stop bit used in communication (most common)
	tty.c_cflag &= ~CSIZE;			  // Clear all bits that set the data size
	tty.c_cflag |= CS8;				  // 8 bits per byte (most common)
	tty.c_cflag &= ~CRTSCTS;		  // Disable RTS/CTS hardware flow control (most common)
	tty.c_cflag |= CREAD | CLOCAL;	  // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;															  // Disable echo
	tty.c_lflag &= ~ECHOE;															  // Disable erasure
	tty.c_lflag &= ~ECHONL;															  // Disable new-line echo
	tty.c_lflag &= ~ISIG;															  // Disable interpretation of INTR, QUIT and SUSP
	tty.c_iflag &= ~( IXON | IXOFF | IXANY );										  // Turn off s/w flow ctrl
	tty.c_iflag &= ~( IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL );	  // Disable any special handling of received bytes

	tty.c_oflag &= ~OPOST;	  // Prevent special interpretation of output bytes (e.g. newline chars)
	tty.c_oflag &= ~ONLCR;	  // Prevent conversion of newline to carriage return/line feed

	tty.c_cc[VTIME] = 2;	// Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[VMIN]	= 0;

	// Set in/out baud rate to be 9600
	cfsetispeed( &tty, B1000000 );
	cfsetospeed( &tty, B1000000 );

	// Save tty settings, also checking for error
	if ( tcsetattr( serialPort, TCSANOW, &tty ) != 0 ) {
		printf( "SerialClass:  Error %i from tcsetattr: %s\n", errno, strerror( errno ) );
	}

	// Send handshake packet to Teensy serial
	std::string msg	   = "C_READY\n";
	ssize_t		packet = write( serialPort, msg.c_str(), msg.size() );
}

void SerialClass::Send( const std::string& msg ) {
	shared->serialPacket = msg + "\n";
	ssize_t bytesWritten = write( serialPort, shared->serialPacket.c_str(), shared->serialPacket.size() );
	if ( bytesWritten < 0 ) {
		std::cerr << "SerialClass:  Serial packet not sent.\n";
	}
	shared->serialPacket = "";
}


void SerialClass::Close() {
	close( serialPort );
}


bool SerialClass::GetStatus() {
	return shared->FLAG_SERIAL_OPEN;
}

void SerialClass::Monitor() {
	if ( shared->FLAG_SERIAL_OPEN && shared->FLAG_PACKET_WAITING ) {
		std::string outgoingPacket = shared->serialPacket + "\n";
		ssize_t		bytesWritten   = write( serialPort, outgoingPacket.c_str(), outgoingPacket.size() );
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