#pragma once

// Memory for shared data
#include <memory>

// Packet types
#include "PacketTypes.h"

// Serial libraries
#include <fcntl.h>		// File controls
#include <iomanip>		// For padding zeroes
#include <termios.h>	// Terminal control
#include <unistd.h>		// Write, read, and close functions

// Vectors
#include <vector>

// Forward declarations
class SystemDataManager;
struct ManagedData;



/** 
 * @brief Display class definition
 */
class SerialClass {

public:
	// Data manager handle
	SerialClass( SystemDataManager& dataHandle, uint8_t nPorts );

	// Public functions
	void   Send( const std::string& msg );
	void   CheckForPacket();
	void   Close();
	int8_t Sign( int val );
	void   Update();



private:
	// Private functions
	void InitializePort0();
	void InitializePort1();

	// Serial functions
	PacketStruct BuildPacket( int8_t type );
	void		 SendPacket();
	void		 ParsePacket( int8_t type, uint8_t length, std::vector<uint8_t> bytes  );
	void		 ConvertPacketToString( int8_t type, PacketStruct pkt );

	// void SendDrivePacket();
	// void SendLimitsPacket();
	// void SendWaitingPacket();
	// Variables
	int			   serialPort0;
	int			   serialPort1;
	struct termios tty0;
	struct termios tty1;
	uint8_t		   nPortsOpen = 1;

	// Packet variables
	char		buffer[64];
	std::string readBuffer = "";

	uint8_t packetCounter = 0;

	// Private functions
	// void ParseIncomingPacket( uint8_t type, uint8_t length, std::vector<uint8_t> bytes );

	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;
};