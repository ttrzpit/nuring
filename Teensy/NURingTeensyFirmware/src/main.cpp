/** NURingTeensyFirmware **/

#include <Arduino.h>

// System data manager object handle
#include "T_SharedDataManagerClass.h"	 // Shared data manager header
SharedDataManager DataHandle;


// Custom libraries
#include "T_AmplifierClass.h"	 // Amplifier header
#include "T_Config.h"			 // Global config constants
#include "T_LEDClass.h"			 // LED header
#include "T_SerialClass.h"		 // Serial header


// Custom classes
T_AmplifierClass Amplifier( DataHandle );
T_LEDClass		 LEDs( DataHandle );
T_SerialClass	 SerialPort( DataHandle );


// Handle to shared data
auto shared = DataHandle.getData();


// Interval Timer
IntervalTimer IT_Amplifiers;


/** FUNCTION PROTOTYPES **/
void IT_AmplifierCallback();
void UpdateSystemState();


/**
 * @brief Main program setup
 * 
 */
void setup() {

	// Initialize serial ports
	SerialPort.Begin();

	// Initialize LEDs
	LEDs.Begin();

	// Initialize amplifiers
	Amplifier.Begin();

	// Start interval timers
	IT_Amplifiers.begin( IT_AmplifierCallback, shared->Timing.periodAmplifier );
}



/**
 * @brief Main program loop
 * 
 * 
 */
void loop() {

	// Update serial port
	SerialPort.Update();

	// Update LEDs
	LEDs.Update();

	// Update system state
	UpdateSystemState();
}



void IT_AmplifierCallback() {

	// Update amplifier
	Amplifier.Update();
}


/**
 * @brief Change state 
 * 
 */
void UpdateSystemState() {

	// Select state
	switch ( shared->Amplifier.packetType ) {

		// Waiting (PC not yet connected)
		case 'W': {

			// Update state
			shared->System.state = stateEnum::WAITING;

			// Update LEDs
			shared->LED.isCommunicatingWithPC = false;
			shared->LED.isDrivingMotors		  = false;
			shared->LED.isMeasuringLimits	  = false;

			break;
		}

		// Idle
		case 'I': {

			// Update state
			shared->System.state = stateEnum::IDLE;

			// Update LEDs
			shared->LED.isCommunicatingWithPC = true;
			shared->LED.isDrivingMotors		  = false;
			shared->LED.isMeasuringLimits	  = false;

			break;
		}

		// Driving
		case 'D': {

			// Update state
			shared->System.state = stateEnum::DRIVING_PWM;

			// Update LEDs
			shared->LED.isCommunicatingWithPC = true;
			shared->LED.isDrivingMotors		  = true;
			shared->LED.isMeasuringLimits	  = false;

			break;
		}

		// Measure limits
		case 'L': {

			// Update state
			shared->System.state = stateEnum::MEASURING_LIMITS;

			// Update LEDs
			shared->LED.isCommunicatingWithPC = true;
			shared->LED.isDrivingMotors		  = true;
			shared->LED.isMeasuringLimits	  = true;

			break;
		}

		// Measure currents
		case 'C': {

			// Update state
			shared->System.state = stateEnum::MEASURING_CURRENTS;

			// Update LEDs
			shared->LED.isCommunicatingWithPC = true;
			shared->LED.isDrivingMotors		  = true;
			shared->LED.isMeasuringLimits	  = true;

			break;
		}

		// Default
		default:
			// Update state
			shared->System.state = stateEnum::WAITING;

			// Update LEDs
			shared->LED.isCommunicatingWithPC = false;
			shared->LED.isDrivingMotors		  = false;
			shared->LED.isMeasuringLimits	  = false;

			break;
	}
}