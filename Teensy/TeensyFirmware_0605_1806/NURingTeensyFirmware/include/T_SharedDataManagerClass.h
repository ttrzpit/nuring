/** SharedDataManager Class Header */

#pragma once

#pragma once

// Memory for shared data
#include "T_Config.h"
#include <Arduino.h>
#include <memory>
#include "T_PacketTypes.h"


// Shared data manager
class SharedDataManager;
struct ManagedData;

enum class stateEnum { IDLE, DRIVING_PWM, MEASURING_LIMITS, MEASURING_CURRENT };


struct AmplifierStruct {

	// Amplifier enabled flags
	bool isEnabled		= false;
	bool isEncoderReset = false;
	bool isBaudUpdated	= false;

	// Amplifer variables
	uint16_t commandedPwmA	  = AMPLIFIER_PWM_ZERO;
	uint16_t commandedPwmB	  = AMPLIFIER_PWM_ZERO;
	uint16_t commandedPwmC	  = AMPLIFIER_PWM_ZERO;
	int32_t	 encoderCountA	  = 0;
	int32_t	 encoderCountB	  = 0;
	int32_t	 encoderCountC	  = 0;
	int16_t	 measuredCurrentA = 0;
	int16_t	 measuredCurrentB = 0;
	int16_t	 measuredCurrentC = 0;
	String	 nameA			  = "";
	String	 nameB			  = "";
	String	 nameC			  = "";
	int32_t	 baudA			  = 0;
	int32_t	 baudB			  = 0;
	int32_t	 baudC			  = 0;
};

struct LEDStruct {

	// Flags
	bool isInitialized = false;
	bool isDriving	   = false;
	bool isMeasuring   = false;
};






/**
 * @brief Struct for serial port data
 * 
 */
struct SerialStruct {

	// Serial flags
	bool isIncomingPacketReady = false;
	bool isOutgoingPacketReady = false;

	// Packet
	PacketStruct packet;

	// Packets
	String packetIn	 = "";
	String packetOut = "";

	// Looping counter
	uint8_t counter = 0;
};



/**
 * @brief Struct for system data and flags
 * 
 */
struct SystemStruct {

	stateEnum state = stateEnum::IDLE;
};



struct TimingStruct {

	// Interval timer periods
	uint16_t periodAmplifier = 1000000 / TIMING_FREQ_AMPLIFIER_DRIVE;
	uint16_t periodHWSerial	 = 1000000 / TIMING_FREQ_AMPLIFIER_SERIAL;
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