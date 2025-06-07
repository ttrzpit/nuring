/** Serial Class **/

#pragma once

// Memory for shared data
#include <Arduino.h>
#include <memory>

// Shared data manager
#include "T_SharedDataManagerClass.h"
class SharedDataManager;
struct ManagedData;

// Packet types
#include "T_PacketTypes.h"

#define SerialIn Serial
#define SerialOut SerialUSB1


/** 
 * @brief Serial class definition
 */
class T_SerialClass {

public:
	// Default constructor
	T_SerialClass( SharedDataManager& dataHandle );

	// Public functions
	void Begin();
	void Update();

private:
	// Data manager handle
	SharedDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;

	// Serial send/receive
	void CheckForPacketFromPC();
	void SendResponsePacketToPC( uint8_t type );

	// Packet builders
	PacketStruct PopulatePacket( uint8_t type );
	PacketStruct BuildOutgoingPacket( uint8_t type );

	// Packet parsers
	void ParsePacketFromPC(  PacketStruct* pkt );

	// Packet variables
	static uint8_t buffer[64];
};