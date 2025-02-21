#pragma once

// Memory for shared data
#include <memory>

// Linux headers for reading mouse position / touch
#include <X11/Xlib.h>


// Forward declarations
class SystemDataManager;
struct ManagedData;



/** 
 * @brief Display class definition
 */
class TouchscreenClass {

public:
	// Constructor
	TouchscreenClass( SystemDataManager& dataHandle );

	// De-constructor
	~TouchscreenClass();

	// Public functions
	void GetCursorPosition();
	void Close();

private:
	// Private functions
	// void Initialize();

	// Private variables
	bool buttonPressed = false;

	// Display handle for X11
	Display* displayHandle = XOpenDisplay( nullptr );

	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;
};