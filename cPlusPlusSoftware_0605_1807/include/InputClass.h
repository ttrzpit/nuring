#pragma once

// Memory for shared data
#include <memory>

// OpenCV core functions
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// Forward declarations
class SystemDataManager;
struct ManagedData;

// Keymap
struct keyMappingStruct {

	uint8_t		keyID;
	char		keyChar;
	std::string keyName;
};


/** 
 * @brief Capture class definition
 */
class InputClass {


public:
	// Data manager handle
	InputClass( SystemDataManager& dataHandle );

	// Public functions
	void ParseInput( int key );



private:
	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;

	// Key map
	keyMappingStruct keyMap;

	// Functions
	void IncrementValueF ( std::string name , float& target, float increment ) ;

};
