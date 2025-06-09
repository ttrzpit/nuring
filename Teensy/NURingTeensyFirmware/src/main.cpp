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
IntervalTimer IT_SerialPC;


/** FUNCTION PROTOTYPES **/
void IT_Callback_Amplifier();
void IT_Callback_SerialPC();
void UpdateSystemState();


/**
 * @brief Main program setup
 * 
 */
void setup() {


	// Initialize LEDs
	LEDs.Begin();

	// Initialize serial ports
	SerialPort.Begin();

	// Initialize amplifiers
	Amplifier.Begin();

	// Start interval timers
	IT_Amplifiers.begin( IT_Callback_Amplifier, shared->Timing.periodAmplifier );
	IT_SerialPC.begin( IT_Callback_SerialPC, shared->Timing.periodSoftwareSerial );

	shared->System.state = stateEnum::WAITING;
}



/**
 * @brief Main program loop
 * 
 * 
 */
void loop() {


	// Update LEDs
	LEDs.Update();


	// Update system state
	UpdateSystemState();
}



void IT_Callback_Amplifier() {


	// Update amplifier
	Amplifier.Update();
}



void IT_Callback_SerialPC() {

	// Update serial port
	SerialPort.Update();
}


/**
 * @brief Change state 
 * 
 */
void UpdateSystemState() {

	// Select state
	switch ( shared->System.state ) {

		// Waiting (PC not yet connected)
		case stateEnum::WAITING: {

			// Update amplifier state
			shared->Amplifier.isEnabled = false;

			// Update LEDs
			shared->LED.isCommunicatingWithPC = false;
			shared->LED.isDrivingMotors		  = false;
			shared->LED.isMeasuringLimits	  = false;

			break;
		}

		// Idle (PC connected, not driving)
		case stateEnum::IDLE: {

			// Update amplifier state
			shared->Amplifier.isEnabled = false;

			// Update LEDs
			shared->LED.isCommunicatingWithPC = true;
			shared->LED.isDrivingMotors		  = false;
			shared->LED.isMeasuringLimits	  = false;

			break;
		}

		// Driving PWM
		case stateEnum::DRIVING_PWM: {

			// Update amplifier state
			shared->Amplifier.isEnabled = true;

			// Update LEDs
			shared->LED.isCommunicatingWithPC = true;
			shared->LED.isDrivingMotors		  = true;
			shared->LED.isMeasuringLimits	  = false;

			break;
		}

		// Measuring limits / current
		case stateEnum::MEASURING_LIMITS: {

			// Update amplifier state
			shared->Amplifier.isEnabled = true;

			// Update LEDs
			shared->LED.isCommunicatingWithPC = true;
			shared->LED.isDrivingMotors		  = true;
			shared->LED.isMeasuringLimits	  = true;

			break;
		}

		default: {

			// Nothing

			break;
		}
	}
}