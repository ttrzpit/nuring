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
void SwitchState();


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

	// Update
	shared->LED.isInitialized = true;

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

	// Check system state
	// SwitchState();
}



void IT_AmplifierCallback() {

	// Update amplifier
	Amplifier.Update();
}


/**
 * @brief Change state 
 * 
 */
void SwitchState() {

	// Select state
	switch ( shared->System.state ) {

		// System is idle
		case ::stateEnum::IDLE: {
			// Update status LEDs
			shared->LED.isDriving	= false;
			shared->LED.isMeasuring = false;

			break;
		}

		// System is measuring encoder limits
		case stateEnum::DRIVING_PWM: {

			// Update status LED
			shared->LED.isDriving	= true;
			shared->LED.isMeasuring = false;

			break;
		}


		// System is measuring current
		case stateEnum::MEASURING_CURRENT: {

			// Update status LED
			shared->LED.isDriving	= false;
			shared->LED.isMeasuring = true;

			break;
		}

		// Default
		default: {

			// Update status LED
			shared->LED.isDriving	= false;
			shared->LED.isMeasuring = false;

			break;
		}
	}
}