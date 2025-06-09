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

	// Packet types
	// Packet_Incoming_Drive_Struct	 packetInDrive;
	// Packet_Outgoing_DriveEcho_Struct packetOutDriveEcho;

	// Packet info
	static uint8_t buffer[64];
	static uint8_t idx;
	static uint8_t state;
	static uint8_t expectedLength;

	// Parsing functions
	void ParseIncomingPacket( uint8_t type, PacketStruct* pkt );
	PacketStruct PopulatePacket( uint8_t type) ; 
	void SendResponsePacket( uint8_t type );
	// void ParseIncomingDrivePacket( Packet_DrivePWM* pkt );
	// void ParseIncomingLimitsPacket( Packet_MeasureLimits* pkt );
	// void ParseIncomingWaitPacket( Packet_Wait* pkt );

	// Echo functions
	// void SendDrivePacketEcho( Packet_DrivePWM* pkt );
	// void SendLimitsPacketEcho( Packet_MeasureLimits* pkt );
	// void SendWaitPacketEcho( Packet_Wait* pkt );
};