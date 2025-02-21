// C headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <errno.h>		// Error integer and strerror() function
#include <fcntl.h>		// File controls
#include <termios.h>	// Terminal control
#include <unistd.h>		// Write, read, and close functions



class SerialInterface {

public:
	// Constructor
	SerialInterface() {
		Begin();
	}

	// Variables
	bool IS_SERIAL_ENABLED = false;
	// Functions
	void Begin();
	void Close();
	void Send( const std::string& msg );
	bool GetStatus();

private:
	int			   serialPort;
	struct termios tty;
	std::string	   packet;
};


void SerialInterface::Begin() {

	// Open serial port
	serialPort = open( "/dev/ttyACM0", O_RDWR | O_NONBLOCK );
	// serialPort = open( "usb1/1-1/1-1.4", O_RDWR );

	// Get existing settings
	if( tcgetattr( serialPort, &tty ) != 0 ) {
		printf( "Error %i from tctgetattr: %s\n", errno, strerror( errno ) );
		IS_SERIAL_ENABLED = false;
	} else {
		std::cout << "SerialInterface:  Serial interface initialized.\n";
		IS_SERIAL_ENABLED = true;
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
	if( tcsetattr( serialPort, TCSANOW, &tty ) != 0 ) {
		printf( "Error %i from tcsetattr: %s\n", errno, strerror( errno ) );
	}

	// Write to serial port
	std::string msg = "C_READY\n";
	// unsigned char msg[] = { 'C', 'e', 'l', 'l', 'o', '\n' };
	ssize_t packet = write( serialPort, msg.c_str(), msg.size() );
}

void SerialInterface::Send( const std::string& msg ) {
	packet				 = msg + "\n";
	ssize_t bytesWritten = write( serialPort, packet.c_str(), packet.size() );
	if( bytesWritten < 0 ) {
		std::cerr << "Serial packet not sent.\n";
	}
}


void SerialInterface::Close() {
	close( serialPort );
}


bool SerialInterface::GetStatus() {
	return IS_SERIAL_ENABLED;
}
