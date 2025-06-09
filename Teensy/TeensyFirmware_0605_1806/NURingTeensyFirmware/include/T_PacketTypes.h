/** Packet Types **/

#pragma once

// Memory for shared data
#include "T_Config.h"
#include <Arduino.h>

/**
 * @brief Struct for software serial packet (C++ to Teensy)
 * 
 */
#pragma pack( push, 1 )	   // Avoid padding
struct PacketStruct {

	// Elements
	int8_t	b8ItemA	 = 0;
	int8_t	b8ItemB	 = 0;
	int8_t	b8ItemC	 = 0;
	int16_t b16ItemA = 0;
	int16_t b16ItemB = 0;
	int16_t b16ItemC = 0;
	int32_t b32ItemA = 0;
	int32_t b32ItemB = 0;
	int32_t b32ItemC = 0;
};
#pragma pack( pop )