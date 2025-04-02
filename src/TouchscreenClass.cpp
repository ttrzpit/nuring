// Call to class header
#include "TouchscreenClass.h"

// System data manager
#include "SystemDataManager.h"

// Constructor
TouchscreenClass::TouchscreenClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {

	std::cout << "TouchClass:   Touchscreen initialized.\n";

	if ( displayHandle ) {
		// Select input events for button press, release, and pointer motion on the root window
		XSelectInput( displayHandle, DefaultRootWindow( displayHandle ), ButtonPressMask | ButtonReleaseMask | PointerMotionMask );

		// [NEW CODE] Grab the pointer to ensure our application receives all pointer events.
		int grabStatus = XGrabPointer( displayHandle, DefaultRootWindow( displayHandle ), True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime );
		if ( grabStatus != GrabSuccess ) {
			std::cerr << "TouchClass: XGrabPointer failed with status " << grabStatus << "\n";
		}
	}
}

/**
 * @brief Deconstructor
 */
TouchscreenClass::~TouchscreenClass() {
	XUngrabPointer( displayHandle, CurrentTime );	 // [NEW CODE] Release the pointer grab.
	XCloseDisplay( displayHandle );
	std::cout << "TouchClass:   X11 shutdown complete.\n";
}

/**
 * @brief Get cursor position and click
 */
void TouchscreenClass::GetCursorPosition() {
	// Call event-processing loop
	ProcessEvents();
}

/**
 * @brief Close the touchscreen class (to end X11 polling)
 */
void TouchscreenClass::Close() {
	// Close X11 handle
	XCloseDisplay( displayHandle );
}

void TouchscreenClass::ParseClick() {
	if ( buttonPressedNew == true ) {
		// Switch-case for active task
		switch ( shared->TASK_NUMBER ) {
		case 0:
			// Nothing
			break;
		case 1:
			// Discrimination
			break;
		case 2:	   // Fitts
			if ( shared->touchPosition.z == 1 ) {
				shared->TASK_COMMAND = 'c';
			} else {
				shared->TASK_COMMAND = 0;
			}
			break;
		case 3:
			//
			break;
		case 4:
			//
			break;
		default:
			//
			break;
		}
		buttonPressedNew = false;
	}
}

void TouchscreenClass::ProcessEvents() {
	// Process all pending events
	while ( XPending( displayHandle ) ) {
		XEvent event;
		XNextEvent( displayHandle, &event );
		switch ( event.type ) {
		case ButtonPress:
			// [NEW CODE] Update state for a button press.
			buttonMask				= event.xbutton.state;
			buttonPressedNew		= true;
			shared->touchPosition.z = buttonPressedNew;
			break;
		case ButtonRelease:
			// [NEW CODE] Update state for a button release.
			buttonMask				= event.xbutton.state;
			buttonPressedNew		= false;
			shared->touchPosition.z = buttonPressedNew;
			break;
		case MotionNotify:
			// [NEW CODE] Update pointer position on motion.
			shared->touchPosition.x = event.xmotion.x - 3440;
			shared->touchPosition.y = event.xmotion.y - 32;
			break;
		default:
			break;
		}
	}

	ParseClick();
}
