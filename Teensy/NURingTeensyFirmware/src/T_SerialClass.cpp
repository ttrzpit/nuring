// Call to class header
#include "T_SerialClass.h"

// System data manager
#include "T_SharedDataManagerClass.h"

// Initialize buffer
uint8_t T_SerialClass::buffer[64];



T_SerialClass::T_SerialClass( SharedDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {};


/**
 * @brief Initialize the incoming and outgoing serial ports
 * 
 */
void T_SerialClass::Begin() {

	SerialIn.begin( 1000000 );	  // Input
	SerialOut.begin( 1000000 );
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
 * @brief Update
 * 
 */
void T_SerialClass::Update() {
	shared->LED.isCommunicatingWithPC = true;

	// See if data available
	// if ( SerialIn.available() ) {

	// Check for incoming packet
	CheckForPacketFromPC();
	// }
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
 * @brief Send packet out
 * 
 * @param packet 
 */
void T_SerialClass::SendResponsePacketToPC( uint8_t type ) {

	// Create new packet
	PacketStruct outgoingPacket;

	outgoingPacket = BuildOutgoingPacket( type );

	// Build full packet
	const int8_t startByte	  = 0xAA;
	const int8_t packetLength = sizeof( outgoingPacket );

	// Compute checksum
	int8_t	checkSum = type ^ packetLength;
	int8_t* raw		 = reinterpret_cast<int8_t*>( &outgoingPacket );
	for ( int8_t i = 0; i < packetLength; ++i ) {
		checkSum ^= raw[i];
	}

	// Build packet buffer
	int8_t buffer[64];
	size_t idx	  = 0;
	buffer[idx++] = startByte;
	buffer[idx++] = type;
	buffer[idx++] = packetLength;

	// Copy buffer into packet
	memcpy( &buffer[idx], &outgoingPacket, packetLength );
	idx += packetLength;
	buffer[idx++] = checkSum;

	// Send over serial port 0
	size_t bytesWritten = SerialOut.write( reinterpret_cast<const uint8_t*>( buffer ), idx );
	if ( bytesWritten == 0 ) {
		// Failed
	}
}


PacketStruct T_SerialClass::BuildOutgoingPacket( uint8_t type ) {

	// Create new packet
	PacketStruct newPacket;

	// Populate packet
	newPacket.packetType	 = type;
	newPacket.packetCounter	 = static_cast<int8_t>( shared->Amplifier.packetCounter );
	newPacket.amplifierState = static_cast<int8_t>( shared->Amplifier.isEnabled );
	newPacket.pwmA			 = static_cast<int16_t>( shared->Amplifier.commandedPwmA );
	newPacket.pwmB			 = static_cast<int16_t>( shared->Amplifier.commandedPwmB );
	newPacket.pwmC			 = static_cast<int16_t>( shared->Amplifier.commandedPwmC );
	newPacket.currentA		 = static_cast<int16_t>( shared->Amplifier.measuredCurrentA );
	newPacket.currentB		 = static_cast<int16_t>( shared->Amplifier.measuredCurrentB );
	newPacket.currentC		 = static_cast<int16_t>( shared->Amplifier.measuredCurrentC );
	newPacket.encoderA		 = static_cast<int32_t>( shared->Amplifier.encoderCountA );
	newPacket.encoderB		 = static_cast<int32_t>( shared->Amplifier.encoderCountB );
	newPacket.encoderC		 = static_cast<int32_t>( shared->Amplifier.encoderCountC );

	// Return built packet
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



void T_SerialClass::CheckForPacketFromPC() {


	while ( SerialIn.available() ) {

		shared->LED.isDrivingMotors = true;

		uint8_t byte = static_cast<uint8_t>( SerialIn.read() );

		if ( byte == 0xaa || byte == 0xAA ) {
			shared->LED.isMeasuringLimits = true;
		}
	}
}

// // Reset values
// uint8_t idx			   = 0;
// uint8_t state		   = 0;
// uint8_t expectedLength = 0;

// // Loop over bytes
// while ( SerialIn.available() ) {

// 	// Read byte and confirm not empty
// 	int8_t byte = static_cast<uint8_t>( SerialIn.read() );
// 	if ( byte == -1 ) {
// 		break;
// 	}

// 	if ( state == 0 && byte == 0xAA ) {	   // Check if start byte detected
// 		shared->LED.isCommunicatingWithPC = true;
// 		// Reset index
// 		idx = 0;

// 		// Save byte and update state
// 		buffer[idx++] = byte;
// 		state		  = 1;

// 	} else if ( state == 1 ) {	  // Get type

// 		// Save expected length
// 		expectedLength = byte;

// 		// Save byte and update
// 		buffer[idx++] = byte;
// 		state		  = 2;

// 	} else if ( state == 2 ) {	  // Checksum

// 					// Save byte and update
// 		buffer[idx++] = byte;
// 		state		  = 3;

// 	} else if ( state == 3 ) {	  // Read rest of packet

// 		// Save byte
// 		buffer[idx++] = byte;

// 		// Check for end of packet
// 		if ( idx ==  expectedLength + 1 ) {

// 			// Update packet information
// 			int8_t packetType	  = buffer[1];
// 			int8_t packetChecksum = buffer[idx - 1];

// 			// Compute checksum
// 			int8_t computed = packetType ^ buffer[2];
// 			for ( int8_t i = 0; i < expectedLength; ++i ) {
// 				computed ^= buffer[3 + i];
// 			}

// 			// Respond to valid checksum
// 			if ( computed == packetChecksum ) {

// 				// Parse packet
// 				PacketStruct packet;
// 				memcpy( &packet, &buffer[3], expectedLength );
// 				ParsePacketFromPC( &packet );


// 			} else {
// 				// Checksum failed
// 				state = 0;
// 				idx	  = 0;
// 			}

// 			// Reset parser
// 			state = 0;
// 			idx	  = 0;
// 		}
// 	}

// 	// Reset on overflow if index greater than buffer size
// 	if ( idx >= sizeof( buffer ) ) {
// 		state = 0;
// 		idx	  = 0;
// 	}
// }
// }



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
 */

/**
 * @brief Parse incoming packet
 * 
 * @param type (I)DLE , (D)RIVE , (L)IMITS , (C)URRENT
 * @param pkt Packet to parse 
 */
void T_SerialClass::ParsePacketFromPC( PacketStruct* pkt ) {

	// Select action based on state
	switch ( pkt->packetType ) {

		// Idle
		case 'I': {


			// Update system state
			shared->System.state = stateEnum::IDLE;

			// Save to local
			shared->Amplifier.packetCounter = pkt->packetCounter;
			shared->Amplifier.isEnabled		= pkt->amplifierState;
			// shared->Amplifier.commandedPwmA = 0;
			// shared->Amplifier.commandedPwmB = 0;
			// shared->Amplifier.commandedPwmC = 0;

			// Send response
			SendResponsePacketToPC( 'i' );


			break;
		}

		// Drive amplifier
		case 'D': {

			// Update system state
			shared->System.state = stateEnum::DRIVING_PWM;

			// Save to local
			shared->Amplifier.packetCounter = pkt->packetCounter;
			shared->Amplifier.isEnabled		= pkt->amplifierState;
			shared->Amplifier.commandedPwmA = pkt->pwmA;
			shared->Amplifier.commandedPwmB = pkt->pwmB;
			shared->Amplifier.commandedPwmC = pkt->pwmC;

			// Send response
			SendResponsePacketToPC( 'd' );



			break;
		}

		// Measure Limits
		case 'L': {

			// Update system state
			shared->System.state = stateEnum::MEASURING_LIMITS;

			// Save to local
			shared->Amplifier.packetCounter = pkt->packetCounter;
			shared->Amplifier.isEnabled		= pkt->amplifierState;

			// Send response
			SendResponsePacketToPC( 'l' );

			break;
		}

		// Measure Limits
		case 'C': {

			// Update system state
			shared->System.state = stateEnum::MEASURING_CURRENTS;

			// Save to local
			shared->Amplifier.packetCounter = pkt->packetCounter;
			shared->Amplifier.isEnabled		= pkt->amplifierState;

			// Send response
			SendResponsePacketToPC( 'c' );

			break;
		}

		// Default case
		default: {

			// Set default state
			// shared->System.state = stateEnum::IDLE;

			// Send response
			// SendResponsePacketToPC( 'i' );

			break;
		}
	}
}
