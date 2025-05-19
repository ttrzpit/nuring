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
	elapsedTime				= currentTime - previousTime;
	shared->timingTimestamp = elapsedTime.count();

	// if ( TASK_TIMER_STARTED ) {
	// taskTimeElapsed			= currentTime - taskTimeStart;

	// }

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


/**
 * @brief Add a string of the current date and time
 * 
 * @return std::string MMDDYYHHMM
 */
void TimingClass::GetTimestamp() {

	// Get current time
	std::time_t t	= std::time( nullptr );
	std::tm*	now = std::localtime( &t );

	// Format time
	std::ostringstream ssTime;
	ssTime << "_d" << std::put_time( now, "%m%d" ) << "_h" << std::put_time( now, "%H%M%S" );

	// Output
	std::cout << "Timestamp generated: " << ssTime.str() << "\n";
	shared->loggingTimestamp = ssTime.str();
}