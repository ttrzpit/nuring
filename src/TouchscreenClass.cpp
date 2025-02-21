// Call to class header
#include "TouchscreenClass.h"

// System data manager
#include "SystemDataManager.h"

// Constructor
TouchscreenClass::TouchscreenClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {

	std::cout << "Touch: Thread " << cv::getThreadNum() << " of " << cv::getNumThreads() << "\n";
	std::cout << "TouchClass:   Touchscreen initialized.\n";
	// Initialize();
}



/**
 * @brief Deconstructor
 */
TouchscreenClass::~TouchscreenClass() {
	XCloseDisplay( displayHandle );
	std::cout << "TouchClass:   X11 shutdown complete.\n";
}



/**
 * @brief Get cursor position and click
 */
void TouchscreenClass::GetCursorPosition() {

	// Make sure display handle is opened
	if ( !displayHandle ) {
		std::cerr << "TouchClass:  Cannot open X11 display handle\n";
	}

	// Get the root window
	Window root = DefaultRootWindow( displayHandle );

	// Variables for pointer position
	int			 rootX, rootY, winX, winY;
	unsigned int buttonMask;
	Window		 childReturn, rootReturn;

	// Query the pointer position and button state
	if ( !XQueryPointer( displayHandle, root, &rootReturn, &childReturn, &rootX, &rootY, &winX, &winY, &buttonMask ) ) {
		std::cerr << "TouchClass:  XQueryPointer returned empty\n";
		XCloseDisplay( displayHandle );
		shared->touchPosition = cv::Point3i( 0, 0, 0 );
	}

	// Check if mouse button pressed
	bool leftButtonPressed = ( buttonMask & Button1Mask ) != 0;

	// Update values
	shared->touchPosition = cv::Point3i( rootX - 3440, rootY - 32, ( leftButtonPressed && !buttonPressed ? 1 : 0 ) );

	// Uncomment this line to debounce button state / only accept first touch
	// buttonPressed = leftButtonPressed;
}


/**
 * @brief Close the touchscreen class (to end X11 polling)
 */

void TouchscreenClass::Close() {

	// Close X11 handle
	XCloseDisplay( displayHandle );
}