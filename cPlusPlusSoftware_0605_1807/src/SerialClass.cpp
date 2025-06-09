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
	} else if ( nPortsOpen == 2 ) {
		InitializePort0();
		InitializePort1();
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
	SerialOut = open( CONFIG_SERIAL_PORT_0.c_str(), O_RDWR | O_NONBLOCK );

	// Get existing settings
	if ( tcgetattr( SerialOut, &tty0 ) != 0 ) {
		printf( "SerialClass:  Error %i from tctgetattr: %s\n", errno, strerror( errno ) );
		shared->Serial.isSerialSendOpen = false;
	} else {
		std::cout << "SerialClass:  Serial interface " << CONFIG_SERIAL_PORT_0 << " (PC -> Teensy) connected.\n";
		shared->Serial.isSerialSendOpen = true;
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
	if ( tcsetattr( SerialOut, TCSANOW, &tty0 ) != 0 ) {
		printf( "SerialClass:  Error %i from tcsetattr: %s\n", errno, strerror( errno ) );
	}

	// Send handshake packet to Teensy serial
	std::string msg	   = "C_READY\n";
	ssize_t		packet = write( SerialOut, msg.c_str(), msg.size() );
}


/**
 * @brief Initialize second serial port
 * 
 */
void SerialClass::InitializePort1() {
	// Open serial port
	SerialIn = open( CONFIG_SERIAL_PORT_1.c_str(), O_RDWR | O_NONBLOCK );

	// Get existing settings
	if ( tcgetattr( SerialIn, &tty1 ) != 0 ) {
		printf( "SerialClass:  Error %i from tctgetattr: %s\n", errno, strerror( errno ) );
		shared->Serial.isSerialReceiveOpen = false;
	} else {
		std::cout << "SerialClass:  Serial interface " << CONFIG_SERIAL_PORT_1 << " (Teensy -> PC) connected.\n";
		shared->Serial.isSerialReceiveOpen = true;
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
	if ( tcsetattr( SerialIn, TCSANOW, &tty1 ) != 0 ) {
		printf( "SerialClass:  Error %i from tcsetattr: %s\n", errno, strerror( errno ) );
	}

	// Send handshake packet to Teensy serial
	// std::string msg	   = "C_READY\n";
	// ssize_t		packet = write( serialPort0, msg.c_str(), msg.size() );
}



void SerialClass::Send( const std::string& msg ) {
	shared->Serial.packetOut = msg + "\n";
	ssize_t bytesWritten	 = write( serialPort0, shared->Serial.packetOut.c_str(), shared->Serial.packetOut.size() );
	if ( bytesWritten < 0 ) {
		std::cerr << "SerialClass:  Serial packet not sent.\n";
	}
	// shared->serialPacket0 = "";
}



void SerialClass::Close() {
	close( SerialOut );
}


int8_t SerialClass::Sign( int val ) {
	return ( ( val < 0 ) ? 1 : 0 );
}



/**
 * @brief Update serial interface
 * 
 */
void SerialClass::Update() {

	// Make sure outgoing serial port is open and running
	if ( shared->Serial.isSerialSendOpen && shared->Serial.isSerialSending ) {

		// Send packet
		SendPacketToTeensy();
	}

	// Make sure incoming serial port is open and running
	if ( shared->Serial.isSerialReceiveOpen && shared->Serial.isSerialReceiving ) {
		CheckForPacket();
	}
}



/**
 * @brief Send outgoing packet
 * 
 */
void SerialClass::SendPacketToTeensy() {

	// Create new packet
	PacketStruct outgoingPacket;
	int8_t		 outgoingType = 'I';

	// Update packet counter
	if ( shared->Serial.packetCounter < 100 ) {
		shared->Serial.packetCounter++;
	} else {
		shared->Serial.packetCounter = 0;
	}

	// Select type
	if ( shared->System.state == stateEnum::IDLE ) {
		outgoingType = 'I';
	} else if ( shared->System.state == stateEnum::DRIVING_PWM ) {
		outgoingType = 'D';
	} else if ( shared->System.state == stateEnum::MEASURING_LIMITS ) {
		outgoingType = 'L';
	} else if ( shared->System.state == stateEnum::MEASURING_CURRENT ) {
		outgoingType = 'C';
	} else {
		outgoingType = 'I';
	}

	// Build packet
	outgoingPacket = BuildPacket( outgoingType );

	// Build full packet
	const uint8_t startByte	   = 0xAA;
	const int8_t  packetType   = outgoingType;
	const uint8_t packetLength = sizeof( outgoingPacket );

	// Compute checksum
	uint8_t	 checkSum = outgoingType ^ packetLength;
	uint8_t* raw	  = reinterpret_cast<uint8_t*>( &outgoingPacket );
	for ( uint8_t i = 0; i < packetLength; ++i ) {
		checkSum ^= raw[i];
	}

	// Build packet buffer
	uint8_t buffer[64];
	size_t	idx	  = 0;
	buffer[idx++] = startByte;
	buffer[idx++] = outgoingType;
	buffer[idx++] = packetLength;

	// Copy buffer into packet
	memcpy( &buffer[idx], &outgoingPacket, packetLength );
	idx += packetLength;
	buffer[idx++] = checkSum;

	// Send over serial port 0
	ssize_t bytesWritten = write( SerialOut, buffer, idx );
	if ( bytesWritten < 0 ) {
		shared->Serial.packetOut = "FAILED!";
		std::cerr << "SerialClass: Failed to send drive packet!\n";
	} else {

		// Save packet to string
		ConvertPacketToString( outgoingType, outgoingPacket );
	}
}


/**
 * @brief Check for new incoming packet
 * 
 */
void SerialClass::CheckForPacket() {

	// Read bytes from the serial port
	int bytesRead = read( serialPort1, buffer, sizeof( buffer ) );

	if ( bytesRead <= 0 ) {
		if ( bytesRead < 0 && errno != EAGAIN ) {
			std::cerr << "SerialClass: Serial read error: " << strerror( errno ) << "\n";
		}
		return;
	}

	// Append new bytes to the readBuffer
	readBuffer.append( buffer, bytesRead );

	while ( readBuffer.size() >= 4 ) {	  // At least startByte, type, length, and checksum

		std::cout << "Buf";
		// Look for start byte
		if ( ( uint8_t )readBuffer[0] != 0xAA ) {
			readBuffer.erase( 0, 1 );	 // Discard invalid header byte
			continue;
		}

		// Wait for enough data
		uint8_t packetType	 = readBuffer[1];
		uint8_t packetLength = readBuffer[2];
		size_t	totalLength	 = 3 + packetLength + 1;

		if ( readBuffer.size() < totalLength ) {
			// Wait for more data
			return;
		}

		// Extract packet
		std::vector<uint8_t> packetBytes( readBuffer.begin(), readBuffer.begin() + totalLength );

		// Validate checksum
		uint8_t computedChecksum = packetBytes[1] ^ packetBytes[2];
		for ( uint8_t i = 0; i < packetLength; ++i ) {
			computedChecksum ^= packetBytes[3 + i];
		}

		uint8_t receivedChecksum = packetBytes.back();

		if ( computedChecksum != receivedChecksum ) {
			std::cerr << "SerialClass: Checksum failed\n";
			readBuffer.erase( 0, totalLength );
			continue;
		}

		// Parse packet
		ParsePacket( packetType, packetLength, packetBytes );

		// Remove parsed packet from buffer
		readBuffer.erase( 0, totalLength );
	}
}



/**
 * @brief Parse incoming packet
 * 
 * @param type Packet type
 * @param length Length of packet
 * @param bytes Bytes to convert
 * 
 * @note b8ItemA | 8bit | PWM:type | LIM:type | CUR:type | WAI:type
 * @note b8ItemB | 8bit | PWM:coun | LIM:coun | CUR:coun | WAI:coun
 * @note b8ItemC | 8bit | PWM:enab | LIM:enab | CUR:enab | WAI:enab
 * @note b16ItemA | 16bit | PWM:pwmA | LIM:0000 | CUR:curA | WAI:0000
 * @note b16ItemB | 16bit | PWM:pwmB | LIM:0000 | CUR:curB | WAI:0000
 * @note b16ItemC | 16bit | PWM:pwmC | LIM:0000 | CUR:curC | WAI:0000
 * @note b32ItemA | 32bit | PWM:0000 | LIM:encA | CUR:encA | WAI:0000
 * @note b32ItemB | 32bit | PWM:0000 | LIM:encB | CUR:encB | WAI:0000
 * @note b32ItemC | 32bit | PWM:0000 | LIM:encC | CUR:encC | WAI:0000
 */
void SerialClass::ParsePacket( int8_t type, uint8_t length, std::vector<uint8_t> bytes ) {



	// Build and populate temp packet
	PacketStruct packet;
	std::memcpy( &packet, &bytes[3], sizeof( packet ) );

	// Select based on type
	switch ( type ) {

		// Drive packet
		case 'd': {

			// Store values
			shared->Amplifier.measuredPwmA = packet.b16ItemA;
			shared->Amplifier.measuredPwmB = packet.b16ItemB;
			shared->Amplifier.measuredPwmC = packet.b16ItemC;

			break;
		}

		// Limits packet
		case 'l': {

			// Store values
			shared->Amplifier.measuredPositionA = packet.b32ItemA;
			shared->Amplifier.measuredPositionB = packet.b32ItemB;
			shared->Amplifier.measuredPositionC = packet.b32ItemC;

			break;
		}

		// Current packet
		case 'c': {

			// Store values
			shared->Amplifier.measuredCurrentA	= packet.b16ItemA;
			shared->Amplifier.measuredCurrentB	= packet.b16ItemB;
			shared->Amplifier.measuredCurrentC	= packet.b16ItemC;
			shared->Amplifier.measuredPositionA = packet.b32ItemA;
			shared->Amplifier.measuredPositionB = packet.b32ItemB;
			shared->Amplifier.measuredPositionC = packet.b32ItemC;

			break;
		}
	}
}



/**
 * @brief Convert packet to string
 * 
 * @param pkt Packet
 */
void SerialClass::ConvertPacketToString( int8_t type, PacketStruct pkt ) {

	// Create output string
	std::ostringstream oss;

	// Select based on packet type (I,D,L,C)
	if ( type == 'I' ) {

		// Build output string
		oss << pkt.b8ItemA << "|";	  // Type
		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemB ) << "|";
		oss << std::setw( 1 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemC );

		// Save packet
		shared->Serial.packetOut = oss.str();

	} else if ( type == 'D' ) {

		// Build output string
		oss << pkt.b8ItemA << "|";	  // Type
		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemB ) << "|";
		oss << std::setw( 1 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemC ) << "|";
		oss << "A" << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( pkt.b16ItemA ) << "|";
		oss << "B" << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( pkt.b16ItemB ) << "|";
		oss << "C" << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( pkt.b16ItemC );

		// Save packet
		shared->Serial.packetOut = oss.str();

	} else if ( type == 'L' ) {

		// Build output string
		oss << pkt.b8ItemA << "|";	  // Type
		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemB ) << "|";
		oss << std::setw( 1 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemC );

		// Save packet
		shared->Serial.packetOut = oss.str();

	} else if ( type == 'C' ) {

		// Build output string
		oss << pkt.b8ItemA << "|";	  // Type
		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemB ) << "|";
		oss << std::setw( 1 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemC );

		// Save packet
		shared->Serial.packetOut = oss.str();

	} else if ( type == 'i' ) {

		// Build output string
		oss << pkt.b8ItemA << "|";	  // Type
		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemB ) << "|";
		oss << std::setw( 1 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemC );

		// Save packet
		shared->Serial.packetIn = oss.str();

	} else if ( type == 'd' ) {

		// Build output string
		oss << pkt.b8ItemA << "|";	  // Type
		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemB ) << "|";
		oss << std::setw( 1 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemC ) << "|";
		oss << "A" << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( pkt.b16ItemA ) << "|";
		oss << "B" << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( pkt.b16ItemB ) << "|";
		oss << "C" << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( pkt.b16ItemC );

		// Save packet
		shared->Serial.packetIn = oss.str();

	} else if ( type == 'l' ) {

		// Build output string
		oss << pkt.b8ItemA << "|";	  // Type
		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemB ) << "|";
		oss << std::setw( 1 ) << std::setfill( '0' ) << static_cast<int>( pkt.b8ItemC ) << "|";
		oss << "A" << std::setw( 6 ) << std::setfill( '0' ) << static_cast<int>( pkt.b32ItemA ) << "|";
		oss << "B" << std::setw( 6 ) << std::setfill( '0' ) << static_cast<int>( pkt.b32ItemB ) << "|";
		oss << "C" << std::setw( 6 ) << std::setfill( '0' ) << static_cast<int>( pkt.b32ItemC );

		// Save packet
		shared->Serial.packetIn = oss.str();

	} else if ( type == 'c' ) {

		// Build output string
		oss << pkt.b8ItemA << "|";	  // Type
		oss << "A" << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( pkt.b16ItemA ) << "|";
		oss << "B" << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( pkt.b16ItemB ) << "|";
		oss << "C" << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( pkt.b16ItemC ) << "|";
		oss << "A" << std::setw( 6 ) << std::setfill( '0' ) << static_cast<int>( pkt.b32ItemA ) << "|";
		oss << "B" << std::setw( 6 ) << std::setfill( '0' ) << static_cast<int>( pkt.b32ItemB ) << "|";
		oss << "C" << std::setw( 6 ) << std::setfill( '0' ) << static_cast<int>( pkt.b32ItemC );

		// Save packet
		shared->Serial.packetIn = oss.str();
	}
}



/**
 * @brief Build appropriate packet
 * 
 * @param type Packet type: (D)rivePWM, (L)imitReading, (C)urrentReading, (W)aiting
 * @return PacketStruct Packet to send out
 * 
 * @note b8ItemA | 8bit | PWM:type | LIM:type | CUR:type | WAI:type
 * @note b8ItemB | 8bit | PWM:coun | LIM:coun | CUR:coun | WAI:coun
 * @note b8ItemC | 8bit | PWM:enab | LIM:enab | CUR:enab | WAI:enab
 * @note b16ItemA | 16bit | PWM:pwmA | LIM:0000 | CUR:curA | WAI:0000
 * @note b16ItemB | 16bit | PWM:pwmB | LIM:0000 | CUR:curB | WAI:0000
 * @note b16ItemC | 16bit | PWM:pwmC | LIM:0000 | CUR:curC | WAI:0000
 * @note b32ItemA | 32bit | PWM:0000 | LIM:encA | CUR:encA | WAI:0000
 * @note b32ItemB | 32bit | PWM:0000 | LIM:encB | CUR:encB | WAI:0000
 * @note b32ItemC | 32bit | PWM:0000 | LIM:encC | CUR:encC | WAI:0000
 */
PacketStruct SerialClass::BuildPacket( int8_t type ) {


	PacketStruct newPacket;

	switch ( type ) {

		// Drive packet
		case 'D': {

			// Populate
			newPacket.b8ItemA  = 'D';
			newPacket.b8ItemB  = shared->Serial.packetCounter;
			newPacket.b8ItemC  = shared->Amplifier.isAmplifierActive;
			newPacket.b16ItemA = shared->Controller.commandedPwmABC.x;
			newPacket.b16ItemB = shared->Controller.commandedPwmABC.y;
			newPacket.b16ItemC = shared->Controller.commandedPwmABC.z;
			newPacket.b32ItemA = 0;
			newPacket.b32ItemB = 0;
			newPacket.b32ItemC = 0;

			break;
		}

		// Limits packet
		case 'L': {

			// Populate
			newPacket.b8ItemA  = 'L';
			newPacket.b8ItemB  = shared->Serial.packetCounter;
			newPacket.b8ItemC  = shared->Amplifier.isAmplifierActive;
			newPacket.b16ItemA = 0;
			newPacket.b16ItemB = 0;
			newPacket.b16ItemC = 0;
			newPacket.b32ItemA = 0;
			newPacket.b32ItemB = 0;
			newPacket.b32ItemC = 0;

			break;
		}

		// Current packet
		case 'C': {

			// Populate
			newPacket.b8ItemA  = 'C';
			newPacket.b8ItemB  = shared->Serial.packetCounter;
			newPacket.b8ItemC  = shared->Amplifier.isAmplifierActive;
			newPacket.b16ItemA = 0;
			newPacket.b16ItemB = 0;
			newPacket.b16ItemC = 0;
			newPacket.b32ItemA = 0;
			newPacket.b32ItemB = 0;
			newPacket.b32ItemC = 0;

			break;
		}

		// Idle packet
		case 'I': {

			// Populate
			newPacket.b8ItemA  = 'I';
			newPacket.b8ItemB  = shared->Serial.packetCounter;
			newPacket.b8ItemC  = shared->Amplifier.isAmplifierActive;
			newPacket.b16ItemA = 0;
			newPacket.b16ItemB = 0;
			newPacket.b16ItemC = 0;
			newPacket.b32ItemA = 0;
			newPacket.b32ItemB = 0;
			newPacket.b32ItemC = 0;

			break;
		}

		// Default case
		default: {

			// Populate
			newPacket.b8ItemA  = 'I';
			newPacket.b8ItemB  = shared->Serial.packetCounter;
			newPacket.b8ItemC  = shared->Amplifier.isAmplifierActive;
			newPacket.b16ItemA = 0;
			newPacket.b16ItemB = 0;
			newPacket.b16ItemC = 0;
			newPacket.b32ItemA = 0;
			newPacket.b32ItemB = 0;
			newPacket.b32ItemC = 0;

			break;
		}
	}

	return newPacket;
}



// PacketStruct BuildPacket( int8_t type );
// 	void		 SendPacket( int8_t type );
// 	void		 ParsePacket( int8_t type );


// void SerialClass::SendDrivePacket() {

// 	// Create new packet
// 	Packet_DrivePWM packet;

// 	if ( packetCounter < 255 ) {
// 		packetCounter++;
// 	} else {
// 		packetCounter = 0;
// 	}

// 	// Fill packet
// 	packet.counter	 = packetCounter;
// 	packet.isEnabled = shared->Amplifier.isAmplifierActive;
// 	packet.pwmA		 = shared->Controller.commandedPwmABC.x;
// 	packet.pwmB		 = shared->Controller.commandedPwmABC.y;
// 	packet.pwmC		 = shared->Controller.commandedPwmABC.z;

// 	// Build full packet
// 	const uint8_t startByte	   = 0xAA;
// 	const uint8_t packetType   = 'D';
// 	const uint8_t packetLength = sizeof( packet );

// 	// Compute checksum
// 	uint8_t	 checkSum = packetType ^ packetLength;
// 	uint8_t* raw	  = reinterpret_cast<uint8_t*>( &packet );
// 	for ( uint8_t i = 0; i < packetLength; ++i ) {
// 		checkSum ^= raw[i];
// 	}

// 	// Build packet buffer
// 	uint8_t buffer[64];
// 	size_t	idx	  = 0;
// 	buffer[idx++] = startByte;
// 	buffer[idx++] = packetType;
// 	buffer[idx++] = packetLength;

// 	// Copy buffer into packet
// 	memcpy( &buffer[idx], &packet, packetLength );
// 	idx += packetLength;
// 	buffer[idx++] = checkSum;

// 	// Send over serial port 0
// 	ssize_t bytesWritten = write( serialPort0, buffer, idx );
// 	if ( bytesWritten < 0 ) {
// 		shared->Serial.packetOut = "FAILED!";
// 		std::cerr << "SerialClass: Failed to send drive packet!\n";
// 	} else {

// 		// Build output string
// 		std::ostringstream oss;
// 		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( packet.counter ) << "|";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.pwmA ) << "|";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.pwmB ) << "|";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.pwmC );

// 		// Save packet
// 		shared->Serial.packetOut = oss.str();
// 	}
// }



// void SerialClass::SendWaitingPacket() {

// 	// Create new packet
// 	Packet_Wait packet;

// 	if ( packetCounter < 255 ) {
// 		packetCounter++;
// 	} else {
// 		packetCounter = 0;
// 	}

// 	// Fill packet
// 	packet.counter	 = packetCounter;
// 	packet.isEnabled = shared->Amplifier.isAmplifierActive;

// 	// Build full packet
// 	const uint8_t startByte		= 0xAA;
// 	const uint8_t packetType	= 'W';
// 	const uint8_t payloadLength = sizeof( packet );

// 	// Compute checksum
// 	uint8_t	 checkSum = packetType ^ payloadLength;
// 	uint8_t* raw	  = reinterpret_cast<uint8_t*>( &packet );
// 	for ( uint8_t i = 0; i < payloadLength; ++i ) {
// 		checkSum ^= raw[i];
// 	}

// 	// Build packet buffer
// 	uint8_t buffer[64];
// 	size_t	idx	  = 0;
// 	buffer[idx++] = startByte;
// 	buffer[idx++] = packetType;
// 	buffer[idx++] = payloadLength;

// 	// Copy buffer into packet
// 	memcpy( &buffer[idx], &packet, payloadLength );
// 	idx += payloadLength;
// 	buffer[idx++] = checkSum;

// 	// Send over serial port 0
// 	ssize_t bytesWritten = write( serialPort0, buffer, idx );
// 	if ( bytesWritten < 0 ) {
// 		shared->Serial.packetOut = "FAILED!";
// 		std::cerr << "SerialClass: Failed to send binary packet!\n";
// 	} else {

// 		// Build output string
// 		std::ostringstream oss;
// 		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( packet.counter ) << "|";
// 		oss << std::setw( 0 ) << std::setfill( '0' ) << static_cast<int>( packet.isEnabled );


// 		// Save packet
// 		shared->Serial.packetOut = oss.str();
// 	}
// }



// /**
//  * @brief Parse incoming packet
//  *
//  * @param type Packet type
//  * @param bytes Incoming packet bytes
//  */
// void SerialClass::ParseIncomingPacket( uint8_t type, uint8_t length, std::vector<uint8_t> bytes ) {

// 	// Check packet type
// 	if ( type == 'D' && length == sizeof( Packet_DrivePWM ) ) {	   // Drive packet

// 		// Build new packet
// 		Packet_DrivePWM pkt;
// 		std::memcpy( &pkt, &bytes[3], sizeof( pkt ) );

// 		// Store values
// 		shared->Teensy.isTeensyResponding		= true;
// 		shared->Teensy.measuredAmplfierOutput.x = pkt.pwmA;
// 		shared->Teensy.measuredAmplfierOutput.y = pkt.pwmB;
// 		shared->Teensy.measuredAmplfierOutput.z = pkt.pwmC;
// 		shared->Teensy.isAmplifierResponding	= pkt.isEnabled;

// 		// Build output string
// 		std::ostringstream oss;
// 		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( pkt.counter ) << " | ";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( shared->Teensy.measuredAmplfierOutput.x ) << " | ";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( shared->Teensy.measuredAmplfierOutput.y ) << " | ";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( shared->Teensy.measuredAmplfierOutput.z );

// 		// Save packet
// 		shared->Serial.packetIn = oss.str();

// 	} else if ( type == 'L' && length == sizeof( Packet_MeasureLimits ) ) {	   // Encoder packet

// 		// Build new encoder packet
// 		Packet_MeasureLimits pkt;
// 		std::memcpy( &pkt, &bytes[3], sizeof( pkt ) );

// 		// Store values
// 		shared->Teensy.isTeensyResponding = true;
// 		shared->Amplifier.encoderCountA	  = pkt.encA;
// 		shared->Amplifier.encoderCountB	  = pkt.encB;
// 		shared->Amplifier.encoderCountC	  = pkt.encC;

// 		// Build output string
// 		std::ostringstream oss;
// 		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( pkt.counter ) << "|";
// 		oss << std::setw( 0 ) << std::setfill( '0' ) << static_cast<int>( pkt.isEnabled ) << "|";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int32_t>( pkt.encA ) << "|";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int32_t>( pkt.encB ) << "|";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int32_t>( pkt.encC );

// 		// Save packet
// 		shared->Serial.packetIn = oss.str();


// 	} else if ( type == 'W' && length == sizeof( Packet_Wait ) ) {	  // Encoder packet

// 		// Build new encoder packet
// 		Packet_Wait pkt;
// 		std::memcpy( &pkt, &bytes[3], sizeof( pkt ) );

// 		// Store values
// 		shared->Teensy.isTeensyResponding = true;

// 		// Build output string
// 		std::ostringstream oss;
// 		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( pkt.counter ) << "|";
// 		oss << std::setw( 0 ) << std::setfill( '0' ) << static_cast<int>( pkt.isEnabled );


// 		// Save packet
// 		shared->Serial.packetIn = oss.str();


// 	} else {
// 		std::cerr << "SerialClass: Unknown or invalid packet.\n";
// 	}
// }



// void SerialClass::SendLimitsPacket() {

// 	// Create new packet
// 	Packet_MeasureLimits packet;

// 	if ( packetCounter < 255 ) {
// 		packetCounter++;
// 	} else {
// 		packetCounter = 0;
// 	}

// 	// Fill packet
// 	packet.counter	 = packetCounter;
// 	packet.isEnabled = 0;
// 	packet.encA		 = 0;
// 	packet.encB		 = 0;
// 	packet.encC		 = 0;

// 	// Build full packet
// 	const uint8_t startByte	   = 0xAA;
// 	const uint8_t packetType   = 'L';
// 	const uint8_t packetLength = sizeof( packet );

// 	// Compute checksum
// 	uint8_t	 checkSum = packetType ^ packetLength;
// 	uint8_t* raw	  = reinterpret_cast<uint8_t*>( &packet );
// 	for ( uint8_t i = 0; i < packetLength; ++i ) {
// 		checkSum ^= raw[i];
// 	}

// 	// Build packet buffer
// 	uint8_t buffer[64];
// 	size_t	idx	  = 0;
// 	buffer[idx++] = startByte;
// 	buffer[idx++] = packetType;
// 	buffer[idx++] = packetLength;

// 	// Copy buffer into packet
// 	memcpy( &buffer[idx], &packet, packetLength );
// 	idx += packetLength;
// 	buffer[idx++] = checkSum;

// 	// Send over serial port 0
// 	ssize_t bytesWritten = write( serialPort0, buffer, idx );
// 	if ( bytesWritten < 0 ) {
// 		shared->Serial.packetOut = "FAILED!";
// 		std::cerr << "SerialClass: Failed to send limits packet!\n";
// 	} else {

// 		// Build output string
// 		std::ostringstream oss;
// 		oss << std::setw( 3 ) << std::setfill( '0' ) << static_cast<int>( packet.counter ) << "|";
// 		oss << std::setw( 0 ) << std::setfill( '0' ) << static_cast<int>( packet.isEnabled ) << "|";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.encA ) << "|";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.encB ) << "|";
// 		oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.encC );

// 		// Save packet
// 		shared->Serial.packetOut = oss.str();
// 	}
// }