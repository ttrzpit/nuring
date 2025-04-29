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
	void ParseClick();
	void ProcessEvents();

	// Private variables
	bool buttonPressedNew = false;
	bool buttonPressedOld = false;

	// Variables for pointer position
	int			 rootX, rootY, winX, winY;
	unsigned int buttonMask;
	Window		 childReturn, rootReturn, root;

	// Display handle for X11
	Display* displayHandle = XOpenDisplay( nullptr );

	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;
};