/** Amplifier Class **/

#pragma once

// Memory for shared data
#include "T_Config.h"
#include <Arduino.h>
#include <memory>

// Forward declarations
class SharedDataManager;
struct ManagedData;

// Hardware serial ports
#define SerialA Serial5	   // AdEx serial
#define SerialB Serial4	   // AbEx serial
#define SerialC Serial3	   // Flex serial

/** 
 * @brief Amplifier  class definition
 */
class T_AmplifierClass {

public:
	// Default constructor
	T_AmplifierClass( SharedDataManager& dataHandle );

	// Public functions
	void Begin();
	void Disable();
	void DrivePWM();
	void Enable();
	void ReadMotorEncoders();
	void ZeroAmplifierOutput();
	void ResetMotorEncoders();
	void Update();


private:
	// Data manager handle
	SharedDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;

	// Private functions
	void ResetIntoPwmMode();
	void Reset();
	void ParseEncoderPacketA();
	void ParseEncoderPacketB();
	void ParseEncoderPacketC();

	// HW Serial elements
	enum class queryStateEnum { IDLE, REQUEST_ENCODER_POSITION, WAIT_ENCODER_POSITION, REQUEST_CURRENT, WAIT_CURRENT, REQUEST_NAME, WAIT_NAME, REQUEST_ENCODER_RESET, WAIT_ENCODER_RESET , REQUEST_BAUD, WAIT_BAUD, REQUEST_BAUD_UPDATE , WAIT_BAUD_UPDATE };
	queryStateEnum		queryState;
	void				QueryAmplifierSerial();
	bool				ReadAmplifierSerial();
	char				queryBuffer[32];
	size_t				queryBufferIndex = 0;
	unsigned long		queryTimeLast	 = 0;
	const unsigned long queryInterval	 = 10;

	// Serial commands
	String asciiGetCurrent		= "g r0x0c\n";
	String asciiGetBaud			= "g r0x90\n";
	String asciiGetEncoderCount = "g r0x17\n";
	String asciiGetName			= "g f0x92\n";
	String asciiSetBaud115237	= "s r0x90 115237\n";
	String asciiSetCurrentMode	= "s r0x24 3\n";
	String asciiSetEncoderCount = "s r0x17 0\n";
};
