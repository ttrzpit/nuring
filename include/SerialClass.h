#pragma once

// Memory for shared data
#include <memory>

// Serial libraries
#include <fcntl.h>		// File controls
#include <iomanip>		// For padding zeroes
#include <termios.h>	// Terminal control
#include <unistd.h>		// Write, read, and close functions

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
	void		Send( const std::string& msg );
	void		CheckForPacket();
	void		Close();
	bool		GetStatus();
	void		Monitor();
	std::string PadValues( int val, int nZeroes );
	int8_t		Sign( int val );


private:
	// Private functions
	void InitializePort0();
	void InitializePort1();

	// Variables
	int			   serialPort0;
	int			   serialPort1;
	struct termios tty0;
	struct termios tty1;
	uint8_t		   nPortsOpen = 1;

	// Packet variables
	char		buffer[64];
	std::string readBuffer = "";
	



	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;
};