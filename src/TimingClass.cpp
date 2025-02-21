// Call to class header
#include "TimingClass.h"

// System data manager
#include "SystemDataManager.h"


// Constructor
TimingClass::TimingClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	std::cout << "TimingClass:  Timer initialized.\n";
}



/**
 * @brief Initialize ArUco detector
 */
void TimingClass::StartTimer() {

	// Capture current time
	previousTime = std::chrono::steady_clock::now();
}



/**
 * @brief Check frequency
 */
void TimingClass::UpdateTimer() {

	// Increment loop counter
	loopCounter++;

	// Get current time
	currentTime = std::chrono::steady_clock::now();

	// Get elapsed time
	elapsedTime = currentTime - previousTime;

	// Check if 1 second has passed
	if( elapsedTime.count() >= 1.0 ) {
		shared->timingFrequency = short( loopCounter / elapsedTime.count() );
		loopCounter				= 0;
		previousTime			= currentTime;
	}
}