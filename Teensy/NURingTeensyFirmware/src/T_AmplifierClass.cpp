// Call to class header
#include "T_AmplifierClass.h"

// System data manager
#include "T_SharedDataManagerClass.h"


T_AmplifierClass::T_AmplifierClass( SharedDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {

	// Set query state
	queryState = queryStateEnum::IDLE;
};


/**
 * @brief Initialize the incoming and outgoing serial ports
 * 
 */
void T_AmplifierClass::Begin() {

	// Configure pin directions
	pinMode( AMPLIFIER_PIN_ENABLE_A, OUTPUT );
	pinMode( AMPLIFIER_PIN_ENABLE_B, OUTPUT );
	pinMode( AMPLIFIER_PIN_ENABLE_C, OUTPUT );
	pinMode( AMPLIFIER_PIN_PWM_A, OUTPUT );
	pinMode( AMPLIFIER_PIN_PWM_B, OUTPUT );
	pinMode( AMPLIFIER_PIN_PWM_C, OUTPUT );

	// Disable amplfiers
	Disable();

	// Set analog resolution
	analogWriteResolution( 12 );

	// Set initial state at zero
	ZeroAmplifierOutput();

	// Establish hardware serial connection to amplifiers
	SerialA.begin( 9600 );
	SerialB.begin( 9600 );
	SerialC.begin( 9600 );
	delay( 1000 );

	// Reset amplifiers into PWM mode (for cogging compensation)
	ResetIntoPwmMode();

	// Enable amplifiers
	Enable();
}



/**
 * @brief Disable amplifiers
 * 
 */
void T_AmplifierClass::Disable() {

	// Send enable commands
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_A, LOW );
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_B, LOW );
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_C, LOW );

	// Update enable flags
	shared->Amplifier.isEnabled = false;
}



/**
 * @brief Sends PWM signals to amplifier
 * 
 */
void T_AmplifierClass::DrivePWM() {

	// Constrain values as a precaution
	shared->Amplifier.commandedPwmA = constrain( shared->Amplifier.commandedPwmA, AMPLIFIER_PWM_MAX, AMPLIFIER_PWM_ZERO );
	shared->Amplifier.commandedPwmB = constrain( shared->Amplifier.commandedPwmB, AMPLIFIER_PWM_MAX, AMPLIFIER_PWM_ZERO );
	shared->Amplifier.commandedPwmC = constrain( shared->Amplifier.commandedPwmC, AMPLIFIER_PWM_MAX, AMPLIFIER_PWM_ZERO );

	// Send command to amplifiers
	analogWrite( AMPLIFIER_PIN_PWM_A, shared->Amplifier.commandedPwmA );
	analogWrite( AMPLIFIER_PIN_PWM_B, shared->Amplifier.commandedPwmB );
	analogWrite( AMPLIFIER_PIN_PWM_C, shared->Amplifier.commandedPwmC );
}



/**
 * @brief Enable amplifiers
 * 
 */
void T_AmplifierClass::Enable() {

	// Send enable commands
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_A, HIGH );
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_B, HIGH );
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_C, HIGH );

	// Update enable flags
	shared->Amplifier.isEnabled = true;
}



/**
 * @brief Reset amplifiers 
 * 
 */
void T_AmplifierClass::Reset() {

	// Send reset commands to amp A (high->low->high)
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_A, HIGH );
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_A, LOW );
	delay( 500 );
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_A, HIGH );

	// Send reset commands to amp B (high->low->high)
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_B, HIGH );
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_B, LOW );
	delay( 500 );
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_B, HIGH );

	// Send reset commands to amp C (high->low->high)
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_C, HIGH );
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_C, LOW );
	delay( 500 );
	digitalWriteFast( AMPLIFIER_PIN_ENABLE_C, HIGH );
}



/**
 * @brief Reset the amplifiers into PWM Current mode
 *        ( needed for cogging compensation )
 * 
 */
void T_AmplifierClass::ResetIntoPwmMode() {

	// Sent reset commands
	Reset();

	// Sent serial command to use PWM current mode
	SerialA.print( asciiSetCurrentMode );
	delay( 500 );
	SerialB.print( asciiSetCurrentMode );
	delay( 500 );
	SerialC.print( asciiSetCurrentMode );
	delay( 500 );
}



/**
 * @brief Main update functions, handles amplifier operations
 * 
 */
void T_AmplifierClass::Update() {

	switch ( shared->System.state ) {

		// Idle
		case stateEnum::IDLE: {

			break;
		}

		// Drive through PWM
		case stateEnum::DRIVING_PWM: {

			// Send drive command if amplifier is enabled
			if ( shared->Amplifier.isEnabled ) {

				// Drive PWM
				DrivePWM();

			}

			break;
		}

		// Measure limits
		case stateEnum::MEASURING_LIMITS: {

			// Disable amplifier
			shared->Amplifier.isEnabled = false;

			// // Check if encoders have been zeroed
			// if ( !shared->Amplifier.isEncoderReset ) {

			// 	// Set state to zero encoders
			// 	queryState = queryStateEnum::REQUEST_ENCODER_RESET;
			// }

			// Read encoder data
			queryState = queryStateEnum::REQUEST_ENCODER_POSITION;

			// Update LED
			// shared->LED.isDrivingMotors	  = false;
			// shared->LED.isMeasuringLimits = true;

			break;
		}

		// Measure current
		case stateEnum::MEASURING_CURRENTS: {

			// Enablle amplifiers
			Enable();

			// Measure current
			queryState = queryStateEnum::REQUEST_CURRENT;

			// Update LED
			// shared->LED.isDrivingMotors	  = true;
			// shared->LED.isMeasuringLimits = true;

			break;
		}

		// Default
		default: {

			// Update state
			queryState = queryStateEnum::IDLE;

			// Update LED
			// shared->LED.isDrivingMotors	  = false;
			// shared->LED.isMeasuringLimits = false;

			break;
		}
	}

	// Poll amplifier
	QueryAmplifierSerial();
}


/**
 * @brief Set amplifier values to zero (no current)
 * 
 */
void T_AmplifierClass::ZeroAmplifierOutput() {
	shared->Amplifier.commandedPwmA = AMPLIFIER_PWM_ZERO;
	shared->Amplifier.commandedPwmB = AMPLIFIER_PWM_ZERO;
	shared->Amplifier.commandedPwmC = AMPLIFIER_PWM_ZERO;
}


/**
 * @brief Sends a request to the amplifier and handles the response
 * 
 */
void T_AmplifierClass::QueryAmplifierSerial() {

	// Capture current time
	unsigned long now = millis();

	// Select action based on state
	switch ( queryState ) {

		// System is idle
		case queryStateEnum::IDLE: {

			// Do nothing

			break;
		}

		// Send encoder position request
		case queryStateEnum::REQUEST_ENCODER_POSITION: {

			// Check if time elapsed
			if ( now - queryTimeLast > queryInterval ) {

				// Send position request
				SerialA.print( asciiGetEncoderCount );

				// Update state and reset buffer
				queryState		 = queryStateEnum::WAIT_ENCODER_POSITION;
				queryBufferIndex = 0;
				queryTimeLast	 = now;
			}

			break;
		}

		// Receive encoder position
		case queryStateEnum::WAIT_ENCODER_POSITION: {

			// Check if data received
			if ( ReadAmplifierSerialA() ) {

				// Check if packet is valid
				if ( queryBuffer[0] == 'v' && queryBuffer[1] == ' ' ) {

					// Save motor position
					shared->Amplifier.encoderCountA = atol( &queryBuffer[2] );

					// Update state
					queryState = queryStateEnum::IDLE;
				}
			}

			break;
		}

		// Send current measurement request
		case queryStateEnum::REQUEST_CURRENT: {

			// Check if time elapsed
			if ( now - queryTimeLast > queryInterval ) {

				// Send current request
				SerialA.print( asciiGetCurrent );

				// Update state and reset buffer
				queryState		 = queryStateEnum::WAIT_CURRENT;
				queryBufferIndex = 0;
				queryTimeLast	 = now;
			}

			break;
		}

		// Receive current measurement
		case queryStateEnum::WAIT_CURRENT: {

			// Check if data received
			if ( ReadAmplifierSerialA() ) {

				// Check if packet is valid
				if ( queryBuffer[0] == 'v' && queryBuffer[1] == ' ' ) {

					// Save motor position
					shared->Amplifier.measuredCurrentA = atoi( &queryBuffer[2] );

					// Update state
					queryState = queryStateEnum::REQUEST_ENCODER_POSITION;
				}
			}

			break;
		}

		// Send name request
		case queryStateEnum::REQUEST_NAME: {

			// Check if time elapsed
			if ( now - queryTimeLast > queryInterval ) {

				// Send current request
				SerialA.print( asciiGetName );

				// Update state and reset buffer
				queryState		 = queryStateEnum::WAIT_NAME;
				queryBufferIndex = 0;
				queryTimeLast	 = now;
			}

			break;
		}

		// Receive name
		case queryStateEnum::WAIT_NAME: {

			// Check if data received
			if ( ReadAmplifierSerialA() ) {

				// Check if packet is valid
				if ( queryBuffer[0] == 'v' && queryBuffer[1] == ' ' ) {

					// Save motor name
					shared->Amplifier.nameA = String( &queryBuffer[2] );

					// Update state
					queryState = queryStateEnum::IDLE;
				}
			}

			break;
		}

		// Send name request
		case queryStateEnum::REQUEST_ENCODER_RESET: {

			// Check if time elapsed
			if ( now - queryTimeLast > queryInterval ) {

				// Send reset request
				SerialA.print( asciiSetEncoderCount );

				// Update state and reset buffer
				queryState		 = queryStateEnum::WAIT_ENCODER_RESET;
				queryBufferIndex = 0;
				queryTimeLast	 = now;
			}

			break;
		}

		// Receive confirmation of reset request
		case queryStateEnum::WAIT_ENCODER_RESET: {

			// Check if data received
			if ( ReadAmplifierSerialA() ) {

				// Strip newline and carriage return
				queryBuffer[strcspn( queryBuffer, "\r\n" )] = 0;

				// Check if packet is valid
				if ( strcmp( queryBuffer, "ok" ) == 0 ) {

					// Update flag
					shared->Amplifier.isEncoderReset = true;

					// Update state
					queryState = queryStateEnum::IDLE;
				}
			}

			break;
		}


		// Send name request
		case queryStateEnum::REQUEST_BAUD_UPDATE: {

			// Check if time elapsed
			if ( now - queryTimeLast > queryInterval ) {

				// Send reset request
				SerialA.print( asciiSetBaud115237 );

				// Update state and reset buffer
				queryState		 = queryStateEnum::WAIT_BAUD_UPDATE;
				queryBufferIndex = 0;
				queryTimeLast	 = now;
			}

			break;
		}

		// Receive confirmation of reset request
		case queryStateEnum::WAIT_BAUD_UPDATE: {

			// Check if data received
			if ( ReadAmplifierSerialA() ) {

				// Strip newline and carriage return
				queryBuffer[strcspn( queryBuffer, "\r\n" )] = 0;

				// Check if packet is valid
				if ( strcmp( queryBuffer, "ok" ) == 0 ) {

					// Update flag
					shared->Amplifier.isBaudUpdated = true;

					// Update state
					queryState = queryStateEnum::IDLE;
				}
			}

			break;
		}

		// Send current measurement request
		case queryStateEnum::REQUEST_BAUD: {

			// Check if time elapsed
			if ( now - queryTimeLast > queryInterval ) {

				// Send current request
				SerialA.print( asciiGetBaud );

				// Update state and reset buffer
				queryState		 = queryStateEnum::WAIT_BAUD;
				queryBufferIndex = 0;
				queryTimeLast	 = now;
			}

			break;
		}

		// Receive current measurement
		case queryStateEnum::WAIT_BAUD: {

			// Check if data received
			if ( ReadAmplifierSerialA() ) {

				// Check if packet is valid
				if ( queryBuffer[0] == 'v' && queryBuffer[1] == ' ' ) {

					// Save value


					// Update state
					queryState = queryStateEnum::IDLE;
				}
			}

			break;
		}

		// Default case
		default: {

			// Set default state
			queryState = queryStateEnum::IDLE;
			break;
		}
	}
}


/**
 * @brief Read amplifier serial port response
 * 
 */
bool T_AmplifierClass::ReadAmplifierSerialA() {

	// Loop while data available
	while ( SerialA.available() ) {

		char c = SerialA.read();

		// Look for terminating character
		if ( c == '\n' ) {

			// Replace terminator with escape key
			queryBuffer[queryBufferIndex] = '\0';

			// Replace return char
			if ( queryBufferIndex > 0 && queryBuffer[queryBufferIndex - 1] == '\r' ) {
				queryBuffer[queryBufferIndex - 1] = '\0';
			}

			// Return true
			return true;
		}

		if ( queryBufferIndex < 63 ) {
			queryBuffer[queryBufferIndex++] = c;
		} else {
			queryBufferIndex = 0;
		}
	}

	// Return false
	return false;
}



// /**
//  * @brief Zero motor encoder values
//  *
//  */
// void T_AmplifierClass::ResetMotorEncoders() {

// 	// Disable motors for safety
// 	shared->Amplifier.isEnabled = false;

// 	// Send encoder reset command and update flag that data is ready
// 	SerialA.print( asciiSetEncoderCountZero );
// 	shared->Amplifier.isEncoderWaitingA = true;
// 	ParseEncoderPacketA();
// 	SerialB.print( asciiSetEncoderCountZero );
// 	shared->Amplifier.isEncoderWaitingB = true;
// 	SerialC.print( asciiSetEncoderCountZero );
// 	shared->Amplifier.isEncoderWaitingC = true;

// 	// Update reset flag
// 	shared->Amplifier.isEncoderReset = true;
// }



// /**
//  * @brief Read motor encoders
//  *
//  */
// void T_AmplifierClass::ReadMotorEncoders() {

// 	// Read encoderA
// 	SerialA.print( asciiGetEncoderCount );
// 	shared->Amplifier.isEncoderWaitingA = true;
// 	ParseEncoderPacketA();

// 	// // Read encoderB
// 	// SerialB.print( asciiGetEncoderCount );
// 	// shared->Amplifier.isEncoderWaitingB = true;
// 	// ParseEncoderPacketB();

// 	// // Read encoderC
// 	// SerialC.print( asciiGetEncoderCount );
// 	// shared->Amplifier.isEncoderWaitingC = true;
// 	// ParseEncoderPacketC();
// }


// void T_AmplifierClass::ParseEncoderPacketA() {

// 	// Variables for reading
// 	static char	   encoderBuffer[32];
// 	static uint8_t idx = 0;

// 	// Check if data waiting via serial
// 	if ( shared->Amplifier.isEncoderWaitingA ) {

// 		// Cycle over data
// 		while ( SerialA.available() ) {

// 			// Replace with read-string until


// 			// Read character
// 			char c = SerialA.read();

// 			// Parse string when terminating character appears
// 			if ( c == '\n' ) {
// 				encoderBuffer[idx] = '\0';

// 				if ( idx > 0 && encoderBuffer[idx - 1] == '\r' ) {
// 					encoderBuffer[idx - 1] = '\0';
// 				}


// 				// Verify valid packet
// 				if ( encoderBuffer[0] == 'v' && encoderBuffer[1] == ' ' ) {

// 					// Store value
// 					shared->Amplifier.encoderCountA = atol( &encoderBuffer[2] );

// 					// Update flag
// 					shared->Amplifier.isEncoderWaitingA = false;
// 				}

// 				// Reset index
// 				idx = 0;

// 			} else if ( idx < sizeof( encoderBuffer ) - 1 ) {

// 				// Store character while within buffer size
// 				encoderBuffer[idx++] = c;

// 			} else {

// 				// Reset in case of buffer overflow
// 				idx = 0;
// 			}
// 		}
// 	}
// }
