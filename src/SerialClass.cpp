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

// Initialize buffer
uint8_t SerialClass::buffer[32];



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
	serialPort0 = open( CONFIG_SERIAL_PORT_0.c_str(), O_RDWR | O_NONBLOCK );

	// Get existing settings
	if ( tcgetattr( serialPort0, &tty0 ) != 0 ) {
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
	if ( tcsetattr( serialPort0, TCSANOW, &tty0 ) != 0 ) {
		printf( "SerialClass:  Error %i from tcsetattr: %s\n", errno, strerror( errno ) );
	}
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
	if ( tcsetattr( serialPort1, TCSANOW, &tty1 ) != 0 ) {
		printf( "SerialClass:  Error %i from tcsetattr: %s\n", errno, strerror( errno ) );
	}
}



void SerialClass::Close() {
	close( serialPort0 );
}



/** ==================================================
 *  ================================================== 
 * 
 *  UU   UU  PPPPP    DDDDD      AAA    TTTTTT  EEEEEE 
 *  UU   UU  PP   PP  DD   DD   AA AA     TT    EE    
 *  UU   UU  PP   PP  DD   DD   AA AA     TT    EE  
 *  UU   UU  PPPPP    DD   DD  AAAAAAA    TT    EEEEE    
 *  UU   UU  PP       DD   DD  AA   AA    TT    EE    
 *  UU   UU  PP       DD   DD  AA   AA    TT    EE    
 *   UUUUU   PP       DDDDD    AA   AA    TT    EEEEEE 
 * 
 *  ================================================== 
 *  ================================================== 
*/



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
		CheckForPacketFromTeensy();
	}
}



/** =============================================== 
 *  =============================================== 
 * 
 *  SSSS   EEEEEE  NN   NN   DDDD  
 * SS      EE      NNN  NN   DD  DD 
 * SS      EE      NNN  NN   DD  DD 
 *  SSSS   EEEE    NN NNNN   DD  DD 
 *     SS  EE      NN  NNN   DD  DD 
 *     SS  EE      NN   NN   DD  DD 
 *  SSSS   EEEEEE  NN   NN   DDDD  
 * 
 *  =============================================== 
 *  =============================================== 
*/



/**
 * @brief Send outgoing packet
 * 
 */
void SerialClass::SendPacketToTeensy() {

	// Create new buffer
	uint8_t buffer[32];

	// Create new packet
	PacketStruct outgoingPacket;
	uint8_t		 outgoingType;


	// Update packet counter
	if ( shared->Amplifier.packetCounter == 99 ) {
		shared->Amplifier.packetCounter = 0;
	} else {
		shared->Amplifier.packetCounter++;
	}

	// Build appropriate packet
	switch ( shared->System.state ) {

		// Idle
		case stateEnum::IDLE: {

			// Build packet
			outgoingType   = 'I';
			outgoingPacket = BuildOutgoingPacket( outgoingType );

			break;
		}

		// Driving
		case stateEnum::DRIVING_PWM: {

			// Build packet
			outgoingType   = 'D';
			outgoingPacket = BuildOutgoingPacket( outgoingType );

			break;
		}

		// Measuring limits
		case stateEnum::MEASURING_LIMITS: {

			// Build packet
			outgoingType   = 'L';
			outgoingPacket = BuildOutgoingPacket( outgoingType );

			break;
		}

		// Current
		case stateEnum::MEASURING_CURRENT: {

			// Build packet
			outgoingType   = 'C';
			outgoingPacket = BuildOutgoingPacket( outgoingType );

			break;
		}
	}

	// Calculate length
	const int8_t packetStructLength = sizeof( outgoingPacket );

	// Initialize checksum
	int8_t checkSum = outgoingType ^ packetStructLength;

	// Flatten packet
	uint8_t* rawBytes = reinterpret_cast<uint8_t*>( &outgoingPacket );

	// XOR over all bytes to build checksum
	for ( int8_t i = 0; i < packetStructLength; ++i ) {
		checkSum ^= rawBytes[i];
	}

	// Initialize header with buffer
	buffer[0] = 0xAA;				 // Start byte
	buffer[1] = sizeof( buffer );	 // Packet length

	// Copy packet into buffer
	memcpy( &buffer[3], &outgoingPacket, sizeof( outgoingPacket ) );

	// Copy checksum into buffer
	buffer[2] = checkSum;

	buffer[31] = '\n';

	// Send over serial port 0
	ssize_t bytesWritten = write( serialPort0, buffer, sizeof( buffer ) );

	// Check if successful
	if ( bytesWritten < 0 ) {

		shared->Serial.packetOut = "FAILED!";
		std::cerr << "SerialClass: Failed to send drive packet!\n";

	} else {

		std::cout << "Written: " << std::dec << bytesWritten << "  ||  ";
		// Print for debugging
		PrintBuffer( buffer, sizeof( buffer ) );
		std::cout << "\n";
	}
}


void SerialClass::PrintBuffer( const uint8_t* buffer, size_t length ) {
	std::cout << "Hex: ";
	for ( size_t i = 0; i < length; i++ ) {
		std::cout << std::hex << std::setw( 2 ) << std::setfill( '0' ) << static_cast<int>( buffer[i] ) << " ";
	}
	// std::cout << "  |||   Dec: ";
	// for ( size_t i = 0; i <= length; i++ ) {
	// 	std::cout << std::dec << std::setw( 2 ) << std::setfill( '0' ) << static_cast<int>( buffer[i] ) << " ";
	// }
}


PacketStruct SerialClass::BuildOutgoingPacket( uint8_t type ) {

	// Create new packet
	PacketStruct newPacket;

	// Populate
	newPacket.packetType	 = type;
	newPacket.packetCounter	 = shared->Amplifier.packetCounter;
	newPacket.amplifierState = static_cast<uint8_t>( shared->Amplifier.isAmplifierActive );
	newPacket.pwmA			 = static_cast<uint16_t>( shared->Controller.commandedPwmABC.x );
	newPacket.pwmB			 = static_cast<uint16_t>( shared->Controller.commandedPwmABC.y );
	newPacket.pwmC			 = static_cast<uint16_t>( shared->Controller.commandedPwmABC.z );
	newPacket.encoderA		 = 0;
	newPacket.encoderB		 = 0;
	newPacket.encoderC		 = 0;
	newPacket.currentA		 = 0;
	newPacket.currentB		 = 0;
	newPacket.currentC		 = 0;

	// Convert to string
	ConvertPacketToSerialString( newPacket );

	return newPacket;
}



/** =========================================================== 
 *  ===========================================================  
 * 
 *  RRRRR    EEEEEE    CCCCC  EEEEEE  IIIIII  VV     VV  EEEEEE 
 *  RR   RR  EE      CC       EE        II    VV     VV  EE     
 *  RR   RR  EE      CC       EE        II     VV   VV   EE   
 *  RRRRR    EEEEE   CC       EEEEE     II     VV   VV   EEEEE     
 *  RR   RR  EE      CC       EE        II      VV VV    EE     
 *  RR   RR  EE      CC       EE        II      VV VV    EE     
 *  RR   RR  EEEEEE    CCCCC  EEEEEE  IIIIII     VVV     EEEEEE 
 * 
 *  ===========================================================  
 *  ===========================================================  
*/

/**
 * @brief Check for new incoming packet
 * 
 */
void SerialClass::CheckForPacketFromTeensy() {

	// Read bytes from the serial port
	int bytesRead = read( serialPort1, buffer, sizeof( buffer ) );

	// Iterate over buffer
	while ( readBuffer.size() >= 4 ) {	  // At least startByte, type, length, and checksum

		// Look for start byte
		if ( ( uint8_t )readBuffer[0] != 0xAA ) {
			readBuffer.erase( readBuffer.begin() );	   // Discard invalid header byte
			continue;
		}

		// Wait for enough data
		uint8_t packetType	 = readBuffer[1];
		uint8_t packetLength = readBuffer[2];
		size_t	totalLength	 = 3 + packetLength + 1;

		// Wait for more data
		if ( readBuffer.size() < totalLength ) {
			return;
		}

		// Validate checksum
		const uint8_t* data				= reinterpret_cast<const uint8_t*>( readBuffer.data() );
		uint8_t		   computedChecksum = packetType ^ packetLength;
		for ( uint8_t i = 0; i < packetLength; ++i ) {
			computedChecksum ^= data[3 + i];
		}

		if ( computedChecksum != data[totalLength - 1] ) {
			std::cerr << "SerialClass: Checksum failed\n";
			readBuffer.erase( readBuffer.begin(), readBuffer.begin() + totalLength );
			continue;
		}

		// Build and populate temp packet
		PacketStruct packet;
		std::memcpy( &packet, &data[3], sizeof( PacketStruct ) );

		// Parse packet
		ParsePacketFromTeensy( packet );

		// Remove parsed packet from buffer
		readBuffer.erase( readBuffer.begin(), readBuffer.begin() + totalLength );



		// }
	}
}



/** ==================================================
 *  ================================================== 
 * 
 *  PPPPPP      AAA    RRRRR     SSSS   EEEEEE 
 *  PP    PP   AA AA   RR   RR  SS   	EE    
 *  PP    PP   AA AA   RR   RR  SS      EE  
 *  PPPPPP    AAAAAAA  RRRRR     SSSS   EEEEE    
 *  PP        AA   AA  RR   RR      SS  EE    
 *  PP        AA   AA  RR   RR      SS  EE    
 *  PP        AA   AA  RR   RR   SSSS   EEEEEE 
 * 
 *  ================================================== 
 *  ================================================== 



/**
 * @brief Parse incoming packet
 * 
 * @param type Packet type
 * @param length Length of packet
 * @param bytes Bytes to convert
 */
void SerialClass::ParsePacketFromTeensy( PacketStruct pkt ) {

	// Store values
	shared->Amplifier.packetType	   = pkt.packetType;
	shared->Amplifier.packetCounter	   = pkt.packetCounter;
	shared->Amplifier.packetDriveState = pkt.amplifierState;
	shared->Amplifier.measuredPwmA	   = pkt.pwmA;
	shared->Amplifier.measuredPwmB	   = pkt.pwmB;
	shared->Amplifier.measuredPwmC	   = pkt.pwmC;
	shared->Amplifier.measuredCurrentA = pkt.currentA;
	shared->Amplifier.measuredCurrentB = pkt.currentB;
	shared->Amplifier.measuredCurrentC = pkt.currentC;
	shared->Amplifier.measuredEncoderA = pkt.encoderA;
	shared->Amplifier.measuredEncoderB = pkt.encoderB;
	shared->Amplifier.measuredEncoderC = pkt.encoderC;

	// Save as string
	ConvertPacketToSerialString( pkt );
}



/**
 * @brief Converts and saves the packet to a string
 */
void SerialClass::ConvertPacketToSerialString( PacketStruct packet ) {

	// Create output string
	std::ostringstream oss;

	// Extract type
	uint8_t type = packet.packetType;

	// Build header
	if ( type == 'I' || type == 'i' ) {
		oss << ( std::isupper( type ) ? "IDLE" : "idle" ) << " [ ";
	} else if ( type == 'D' || type == 'd' ) {
		oss << ( std::isupper( type ) ? "DRIVE" : "drive" ) << " [ ";
	} else if ( type == 'L' || type == 'l' ) {
		oss << ( std::isupper( type ) ? "LIMITS" : "limits" ) << " [ ";
	} else if ( type == 'C' || type == 'c' ) {
		oss << ( std::isupper( type ) ? "CURRENT" : "current" ) << " [ ";
	} else {
		oss << "!ERROR!" << " [ ";
	}

	// Build string
	oss << std::setw( 2 ) << std::setfill( '0' ) << static_cast<int>( packet.packetCounter ) << " ] [ ";
	oss << std::setw( 1 ) << std::setfill( '0' ) << static_cast<int>( packet.amplifierState ) << " ] [ ";
	oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.pwmA ) << " ";
	oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.pwmB ) << " ";
	oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.pwmC ) << " ] [ ";
	oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.currentA ) << " ";
	oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.currentB ) << " ";
	oss << std::setw( 4 ) << std::setfill( '0' ) << static_cast<int>( packet.currentC ) << " ] [ ";
	oss << std::setw( 6 ) << std::setfill( '0' ) << static_cast<int>( packet.encoderA ) << " ";
	oss << std::setw( 6 ) << std::setfill( '0' ) << static_cast<int>( packet.encoderB ) << " ";
	oss << std::setw( 6 ) << std::setfill( '0' ) << static_cast<int>( packet.encoderC ) << " ]";

	if ( std::isupper( type ) ) {
		// std::cout << "  << OUT >>  ";
		shared->Serial.packetOut = oss.str();
	} else {
		// std::cout << "  << IN >>  ";
		shared->Serial.packetIn = oss.str();
	}
}



/** =============================================== **/
/** HELPERS HELPERS HELPERS HELPERS HELPERS HELPERS **/
/** HELPERS HELPERS HELPERS HELPERS HELPERS HELPERS **/
/** HELPERS HELPERS HELPERS HELPERS HELPERS HELPERS **/
/** =============================================== **/

/**
 * @brief Print the given byte
 * 
 * @param pktBytes Vector of bytes
 */
void SerialClass::PrintByte( std::vector<uint8_t> pktBytes ) {

	for ( size_t i = 0; i < pktBytes.size(); ++i ) {
		std::cout << std::hex << std::setw( 2 ) << std::setfill( '0' ) << static_cast<int>( pktBytes[i] ) << " ";
	}
	std::cout << std::dec << "\n";
}
