/** SharedDataManager Class Header */

#pragma once

#pragma once

// Memory for shared data
#include "T_Config.h"
#include "T_PacketTypes.h"
#include <Arduino.h>
#include <memory>


// Shared data manager
class SharedDataManager;
struct ManagedData;

enum class stateEnum { WAITING, IDLE, DRIVING_PWM, MEASURING_LIMITS, MEASURING_CURRENTS };


struct AmplifierStruct {

	// Amplifier enabled flags
	bool isEnabled		= false;
	bool isEncoderReset = false;
	bool isBaudUpdated	= false;

	// Packet variables
	uint8_t	 packetType		  = 'W';	// Waiting initially
	uint8_t	 commandedState	  = 0;
	uint8_t	 packetCounter	  = 0;
	uint16_t commandedPwmA	  = AMPLIFIER_PWM_ZERO;
	uint16_t commandedPwmB	  = AMPLIFIER_PWM_ZERO;
	uint16_t commandedPwmC	  = AMPLIFIER_PWM_ZERO;
	int16_t	 measuredCurrentA = 987;
	int16_t	 measuredCurrentB = 654;
	int16_t	 measuredCurrentC = 321;
	int32_t	 encoderCountA	  = 123456;
	int32_t	 encoderCountB	  = 234567;
	int32_t	 encoderCountC	  = 345678;

	// Other variables
	String	nameA = "";
	String	nameB = "";
	String	nameC = "";
	int32_t baudA = 0;
	int32_t baudB = 0;
	int32_t baudC = 0;
};

struct LEDStruct {

	// Flags
	bool isCommunicatingWithPC = false;
	bool isDrivingMotors	   = false;
	bool isMeasuringLimits	   = false;
};



/**
 * @brief Struct for serial port data
 * 
 */
struct SerialStruct {

	// Serial flags
	bool isIncomingPacketReady = false;
	bool isOutgoingPacketReady = false;

	// Packets
	String packetIn	 = "";
	String packetOut = "";
};



/**
 * @brief Struct for system data and flags
 * 
 */
struct SystemStruct {

	stateEnum state = stateEnum::WAITING;
};



struct TimingStruct {

	// Interval timer periods
	uint16_t periodAmplifier	  = 1000000 / TIMING_FREQ_AMPLIFIER_DRIVE;
	uint16_t periodHWSerial		  = 1000000 / TIMING_FREQ_AMPLIFIER_HWSERIAL;
	uint16_t periodSoftwareSerial = 1000000 / TIMING_FREQ_AMPLIFIER_SOFTWARESERIAL;
};



/**
 * @brief Main struct that gets shared
 * 
 */
struct ManagedData {

	// Declare system structs
	AmplifierStruct Amplifier;
	LEDStruct		LED;
	SerialStruct	Serial;
	SystemStruct	System;
	TimingStruct	Timing;
};


/**
 * @brief 
 * 
 */
class SharedDataManager {
public:
	// Class
	SharedDataManager();

	// Functions
	std::shared_ptr<ManagedData> getData();



private:
	std::shared_ptr<ManagedData> data;
};