// Call to class header
#include "T_SerialClass.h"

// System data manager
#include "T_SharedDataManagerClass.h"

// Set initial values
uint8_t T_SerialClass::buffer[64];
uint8_t T_SerialClass::idx			  = 0;
uint8_t T_SerialClass::state		  = 0;
uint8_t T_SerialClass::expectedLength = 0;

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



/**
 * @brief Initialize the incoming and outgoing serial ports
 * 
 */
void T_SerialClass::Update() {

	// Reset values
	idx			   = 0;
	state		   = 0;
	expectedLength = 0;

	// Loop over bytes
	while ( SerialIn.available() ) {

		uint8_t byte = SerialIn.read();

		if ( state == 0 && byte == 0xAA ) {	   // Check if start byte detected

			// Reset index
			idx = 0;

			// Save byte and update state
			buffer[idx++] = byte;
			state		  = 1;

		} else if ( state == 1 ) {	  // Get type

			// Save byte and update
			buffer[idx++] = byte;
			state		  = 2;
		} else if ( state == 2 ) {	  // Checksum

			// Save expected length
			expectedLength = byte;

			// Save byte and update
			buffer[idx++] = byte;
			state		  = 3;

		} else if ( state == 3 ) {	  // Read rest of packet

			// Save byte
			buffer[idx++] = byte;

			// Check for end of packet
			if ( idx == 3 + expectedLength + 1 ) {

				// Update packet information
				uint8_t packetType	   = buffer[1];
				uint8_t packetChecksum = buffer[idx - 1];

				// Compute checksum
				uint8_t computed = packetType ^ buffer[2];
				for ( uint8_t i = 0; i < expectedLength; ++i ) {
					computed ^= buffer[3 + i];
				}

				// Respond to valid checksum
				if ( computed == packetChecksum ) {

					PacketStruct* packet = ( PacketStruct* )&buffer[3];
					ParseIncomingPacket( packetType, packet );

				} else {
					// Checksum failed
					state = 0;
					idx	  = 0;
				}

				// Reset parser
				state = 0;
				idx	  = 0;
			}
		}

		// Reset on overflow if index greater than buffer size
		if ( idx >= sizeof( buffer ) ) {
			state = 0;
			idx	  = 0;
		}
	}
}


/**
  * @brief Parse incoming serial packet
  * 
  * @param type Packet type: (D)rivePWM, (L)imitReading, (C)urrentReading, (W)aiting
  * @param pkt Incoming packet
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
void T_SerialClass::ParseIncomingPacket( uint8_t type, PacketStruct* pkt ) {


	// Select action based on state
	switch ( type ) {

		// Drive amplifier
		case 'D': {

			// Update system state
			shared->System.state = stateEnum::DRIVING_PWM;

			// Save to local
			shared->Serial.counter			= pkt->b8ItemB;
			shared->Amplifier.isEnabled		= pkt->b8ItemC;
			shared->Amplifier.commandedPwmA = pkt->b16ItemA;
			shared->Amplifier.commandedPwmB = pkt->b16ItemB;
			shared->Amplifier.commandedPwmC = pkt->b16ItemC;

			// Populate packet
			PopulatePacket( type );

			break;
		}

		// Measure Limits
		case 'L': {

			// Update system state
			shared->System.state = stateEnum::MEASURING_LIMITS;

			// Save to local
			shared->Serial.counter		= pkt->b8ItemB;
			shared->Amplifier.isEnabled = pkt->b8ItemC;

			// Populate packet
			PopulatePacket( type );

			break;
		}

		// Measure Limits
		case 'C': {

			// Update system state
			shared->System.state = stateEnum::MEASURING_CURRENT;

			// Save to local
			shared->Serial.counter		= pkt->b8ItemB;
			shared->Amplifier.isEnabled = pkt->b8ItemC;

			// Populate packet
			PopulatePacket( type );

			break;
		}

		// Default case
		default: {

			// Set default state
			shared->System.state = stateEnum::IDLE;

			break;
		}
	}
}


/**
  * @brief Send appropriate response packet
  * 
  * @param type Packet type: (D)rivePWM, (L)imitReading, (C)urrentReading, (W)aiting
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
PacketStruct T_SerialClass::PopulatePacket( uint8_t type ) {

	PacketStruct newPacket;

	switch ( type ) {

		// Drive packet
		case 'D': {

			// Populate
			newPacket.b8ItemA  = 'd';
			newPacket.b8ItemB  = shared->Serial.counter;
			newPacket.b8ItemC  = shared->Amplifier.isEnabled;
			newPacket.b16ItemA = shared->Amplifier.commandedPwmA;
			newPacket.b16ItemB = shared->Amplifier.commandedPwmB;
			newPacket.b16ItemC = shared->Amplifier.commandedPwmC;
			newPacket.b32ItemA = 0;
			newPacket.b32ItemB = 0;
			newPacket.b32ItemC = 0;

			break;
		}

		// Limits packet
		case 'L': {

			// Populate
			newPacket.b8ItemA  = 'l';
			newPacket.b8ItemB  = shared->Serial.counter;
			newPacket.b8ItemC  = shared->Amplifier.isEnabled;
			newPacket.b16ItemA = 0;
			newPacket.b16ItemB = 0;
			newPacket.b16ItemC = 0;
			newPacket.b32ItemA = shared->Amplifier.encoderCountA;
			newPacket.b32ItemB = shared->Amplifier.encoderCountB;
			newPacket.b32ItemC = shared->Amplifier.encoderCountC;

			break;
		}

		// Current packet
		case 'C': {

			// Populate
			newPacket.b8ItemA  = 'c';
			newPacket.b8ItemB  = shared->Serial.counter;
			newPacket.b8ItemC  = shared->Amplifier.isEnabled;
			newPacket.b16ItemA = shared->Amplifier.measuredCurrentA;
			newPacket.b16ItemB = shared->Amplifier.measuredCurrentB;
			newPacket.b16ItemC = shared->Amplifier.measuredCurrentC;
			newPacket.b32ItemA = shared->Amplifier.encoderCountA;
			newPacket.b32ItemB = shared->Amplifier.encoderCountB;
			newPacket.b32ItemC = shared->Amplifier.encoderCountC;

			break;
		}

		// Idle packet
		case 'I': {

			// Populate
			newPacket.b8ItemA  = 'i';
			newPacket.b8ItemB  = shared->Serial.counter;
			newPacket.b8ItemC  = shared->Amplifier.isEnabled;
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
			newPacket.b8ItemA  = 0;
			newPacket.b8ItemB  = 0;
			newPacket.b8ItemC  = 0;
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



void T_SerialClass::SendResponsePacket( uint8_t type ) {

	// Create and populate new packet
	PacketStruct outgoingPacket = PopulatePacket( type );

	// Build full packet
	const uint8_t startByte	   = 0xAA;
	const uint8_t packetType   = type;
	const uint8_t packetLength = sizeof( outgoingPacket );

	// Compute checksum
	uint8_t	 checkSum = packetType ^ packetLength;
	uint8_t* raw	  = reinterpret_cast<uint8_t*>( &outgoingPacket );
	for ( uint8_t i = 0; i < packetLength; ++i ) {
		checkSum ^= raw[i];
	}

	// Build packet buffer
	uint8_t buffer[64];
	size_t	idx	  = 0;
	buffer[idx++] = startByte;
	buffer[idx++] = packetType;
	buffer[idx++] = packetLength;

	// Copy buffer into packet
	memcpy( &buffer[idx], &outgoingPacket, packetLength );
	idx += packetLength;
	buffer[idx++] = checkSum;

	// Send over serial port 0
	size_t bytesWritten = SerialOut.write( buffer, idx );
	if ( bytesWritten == 0 ) {
		// Failed
	}
}



// /** PARSING FUNCTIONS **/

// /**
// 	 * @brief Parse incoming drive packet
// 	 *
// 	 * @param pkt Incoming packet
// 	 */
// void T_SerialClass::ParseIncomingDrivePacket( Packet_DrivePWM* pkt ) {

// 	// Save to local values
// 	shared->Amplifier.counter		= pkt->counter;
// 	shared->Amplifier.isEnabled		= pkt->isEnabled;
// 	shared->Amplifier.commandedPwmA = pkt->pwmA;
// 	shared->Amplifier.commandedPwmB = pkt->pwmB;
// 	shared->Amplifier.commandedPwmC = pkt->pwmC;

// 	// Echo packet back
// 	SendDrivePacketEcho( pkt );
// }


// void T_SerialClass::SendDrivePacketEcho( Packet_DrivePWM* pkt ) {

// 	// Copy packet
// 	Packet_DrivePWM packet = *pkt;

// 	// Build full packet
// 	const uint8_t startByte		= 0xAA;
// 	const uint8_t packetType	= 'D';
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
// 	size_t bytesWritten = SerialOut.write( buffer, idx );
// 	if ( bytesWritten == 0 ) {
// 		// Failed
// 	}
// }



// /**
// 	 * @brief Parse incoming limits packet
// 	 *
// 	 * @param pkt Incoming packet
// 	 */
// void T_SerialClass::ParseIncomingLimitsPacket( Packet_MeasureLimits* pkt ) {

// 	// Save to local values
// 	shared->Amplifier.counter	= pkt->counter;
// 	shared->Amplifier.isEnabled = pkt->isEnabled;

// 	// Echo packet back
// 	// SendLimitsPacketEcho( pkt );
// }


// void T_SerialClass::SendLimitsPacketEcho( Packet_MeasureLimits* pkt ) {

// 	// Copy packet
// 	Packet_MeasureLimits packet = *pkt;

// 	// Add packet data
// 	packet.counter	 = shared->Amplifier.counter;
// 	packet.isEnabled = shared->Amplifier.isEnabled;
// 	packet.encA		 = shared->Amplifier.encoderCountA;
// 	packet.encB		 = shared->Amplifier.encoderCountB;
// 	packet.encC		 = shared->Amplifier.encoderCountC;

// 	// Build full packet
// 	const uint8_t startByte		= 0xAA;
// 	const uint8_t packetType	= 'L';
// 	const uint8_t payloadLength = sizeof( packet );

// 	// Compute checksum
// 	uint8_t	 checkSum = packetType ^ payloadLength;
// 	uint8_t* raw	  = reinterpret_cast<uint8_t*>( &packet );
// 	for ( uint8_t i = 0; i < payloadLength; ++i ) {
// 		checkSum ^= raw[i];
// 	}

// 	// Build packet buffer
// 	uint8_t buffer[128];
// 	size_t	idx	  = 0;
// 	buffer[idx++] = startByte;
// 	buffer[idx++] = packetType;
// 	buffer[idx++] = payloadLength;

// 	// Copy buffer into packet
// 	memcpy( &buffer[idx], &packet, payloadLength );
// 	idx += payloadLength;
// 	buffer[idx++] = checkSum;

// 	// Send over serial port 0
// 	size_t bytesWritten = SerialOut.write( buffer, idx );
// 	if ( bytesWritten == 0 ) {
// 		// Failed
// 	}
// }



// /**
// 	 * @brief Parse incoming drive packet
// 	 *
// 	 * @param pkt Incoming packet
// 	 */
// void T_SerialClass::ParseIncomingWaitPacket( Packet_Wait* pkt ) {

// 	// Save to local values
// 	shared->Amplifier.counter	= pkt->counter;
// 	shared->Amplifier.isEnabled = pkt->isEnabled;

// 	// Echo packet back
// 	SendWaitPacketEcho( pkt );
// }



// void T_SerialClass::SendWaitPacketEcho( Packet_Wait* pkt ) {

// 	// Copy packet
// 	Packet_Wait packet = *pkt;

// 	// Add packet data
// 	packet.counter	 = shared->Amplifier.counter;
// 	packet.isEnabled = shared->Amplifier.isEnabled;

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
// 	size_t bytesWritten = SerialOut.write( buffer, idx );
// 	if ( bytesWritten == 0 ) {
// 		// Failed
// 	}
// }
