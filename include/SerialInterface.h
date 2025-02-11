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

	// Functions
	void Begin();
	void Close();
	void Send( std::string msg );

private:
	int			   serialPort;
	struct termios tty;
};


void SerialInterface::Begin() {

	// Open serial port
	serialPort = open( "/dev/ttyACM0", O_RDWR );
	// serialPort = open( "usb1/1-1/1-1.4", O_RDWR );

	// Get existing settings
	if( tcgetattr( serialPort, &tty ) != 0 ) {
		printf( "Error %i from tctgetattr: %s\n", errno, strerror( errno ) );
	} else {
		std::cout << "Serial opened.\n";
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

	tty.c_cc[ VTIME ] = 10;	   // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
	tty.c_cc[ VMIN ]  = 0;

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
	write( serialPort, msg.c_str(), sizeof( msg ) );
}

void SerialInterface::Send( std::string msg ) {
	msg += "\n";
	write( serialPort, msg.c_str(), sizeof( msg ) );
}


void SerialInterface::Close() {
	close( serialPort );
}

// /* Serial Interface*/

// #pragma once

// #include <windows.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string>
// #include <iostream>
// #include <algorithm>
// #include <numeric>
// #include <math.h>
// #include <utility>
// #include <iomanip>
// #include <sstream>

// #define MAX_DATA_LENGTH     14


// class SerialInterface {

//     private:

//     HANDLE handler ;
//     bool connected ;
//     COMSTAT status ;
//     DWORD errors ;
//     uint8_t MAX_OUTGOING_DATA_LENGTH = 20 ;

//     int nZeroesPayload = 3 ;
//     int nZeroesNumber = 4 ;
//     std::string outgoingPacket ;
//     std::string packetHeader ;
//     std::string packetFilename ;
//     std::string packetNumber ;
//     std::string packetPayloadA ;
//     std::string packetPayloadB ;
//     std::string packetFooter ;



//     public:

//         /* Initialize new serial controller
//         @param  portName    [char] The name of the port to be used

//         @returns            None */
//     SerialInterface( char* portName , int baudRate ) {

//         this->connected = false ;

//         this->handler = CreateFileA( static_cast< LPCSTR >( portName ) ,
//             GENERIC_READ | GENERIC_WRITE ,  // dwDesiredAccess
//             // GENERIC_WRITE ,  // dwDesiredAccess
//             0 ,                             // dwShareMode              0
//             NULL ,                          // lpSecurityAttributes
//             OPEN_EXISTING ,                 // dwCreationDisposition    OPEN_EXISTING
//             FILE_ATTRIBUTE_NORMAL ,         // dwFlagsAndAttributes
//             NULL );                         // hTemplateFile            NULL

//         if ( this->handler == INVALID_HANDLE_VALUE ) {
//             if ( GetLastError() == ERROR_FILE_NOT_FOUND ) {
//                 printf( "ERROR: Handle was not attached. Reason: %s is not available\n" , portName );
//             } else {
//                 printf( "ERROR! Serial device may already be in use." );
//             }
//         } else {
//             DCB dcbSerialParameters = { 0 };

//             if ( !GetCommState( this->handler , &dcbSerialParameters ) ) {
//                 printf( "Failed to get current serial parameters" );
//             } else {
//                 // dcbSerialParameters.BaudRate = CBR_256000;
//                 dcbSerialParameters.BaudRate = baudRate ;
//                 dcbSerialParameters.ByteSize = 8;
//                 dcbSerialParameters.StopBits = ONESTOPBIT;
//                 dcbSerialParameters.Parity = NOPARITY;
//                 // dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;
//                 dcbSerialParameters.fDtrControl = DTR_CONTROL_DISABLE ;

//                 if ( !SetCommState( handler , &dcbSerialParameters ) ) {
//                     printf( "ALERT: could not set Serial port parameters\n" );
//                 } else {
//                     this->connected = true;
//                     PurgeComm( this->handler , PURGE_RXCLEAR | PURGE_TXCLEAR );

//                 }
//             }
//         }
//     }



//     /* Close the serial controller*/
//     ~SerialInterface() {
//         if ( this->connected ) {
//             this->connected = false ;
//             CloseHandle( this->handler ) ;
//         }
//     }

//     /* Write data to serial port
//     @param      buffer      [char] Text to be sent
//     @param      buf_size    [int] Size of buffer in bytes
//     @returns                True/false  */
//     void writeSerial( char* buffer , unsigned int buf_size ) {

//         DWORD bytesSend ;

//         if ( !WriteFile( this->handler , ( void* ) buffer , buf_size , &bytesSend , 0 ) ) {

//             ClearCommError( this->handler , &this->errors , &this->status ) ;
//             // return false ;
//         } else {

//             // return true ;
//         }


//     }



//     /* Read serial port
//     @param      buffer      [char] Text to be read
//     @param      buf_size    [int] Size of buffer in bytes
//     @returns                Status */
//     int readSerial( char* buffer , unsigned int buf_size ) {

//         DWORD bytesRead ;
//         unsigned int toRead = 0 ;

//         ClearCommError( this->handler , &this->errors , &this->status ) ;

//         // Check size
//         if ( this->status.cbInQue > 0 ) {

//             if ( this->status.cbInQue > buf_size ) {
//                 toRead = buf_size ;
//             } else {
//                 toRead = this->status.cbInQue ;
//             }
//         }

//         if ( ReadFile( this->handler , buffer , toRead , &bytesRead , NULL ) ) {
//             return bytesRead ;
//         }

//         return 0 ;

//     }
// } ;
