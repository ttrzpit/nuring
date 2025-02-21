#pragma once

// Memory for shared data
#include <memory>

// Serial libraries
#include <fcntl.h>		// File controls
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
	SerialClass( SystemDataManager& dataHandle );

	// Public functions
	void Send( const std::string& msg );
	void Close();
	bool GetStatus();
	void Monitor();


private:
	// Private functions
	void Initialize();

	// Variables
	int			   serialPort;
	struct termios tty;

	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;
};