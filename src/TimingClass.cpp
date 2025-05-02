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

	if ( TASK_TIMER_STARTED ) {
		taskTimeElapsed			= currentTime - taskTimeStart;
		shared->timingTimestamp = taskTimeElapsed.count();
	}

	// Check if 1 second has passed
	if ( elapsedTime.count() >= 1.0 ) {
		shared->timingFrequency = short( loopCounter / elapsedTime.count() );
		shared->timingTimestep	= 1.0f / shared->timingFrequency;
		loopCounter				= 0;
		previousTime			= currentTime;
	}
}

void TimingClass::TaskTimerStart() {

	// Capture current time
	taskTimeStart = std::chrono::steady_clock::now();

	// Update flag
	TASK_TIMER_STARTED = true;
}


void TimingClass::TaskTimerEnd() {

	// Capture final time
	taskTimeEnd = std::chrono::steady_clock::now();

	// Save final time
	if ( shared->TASK_NAME == "FITTS" ) {
		taskTimeElapsed				= taskTimeEnd - taskTimeStart;
		shared->fittsCompletionTime = taskTimeElapsed.count();
	}

	// Update flag
	TASK_TIMER_STARTED = false;
}