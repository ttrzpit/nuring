#pragma once

// Chrono for benchmarking time
#include <chrono>
#include <memory>



// Forward declarations
class SystemDataManager;
struct ManagedData;



/** 
 * @brief Display class definition
 */
class TimingClass {

public:
	// Data manager handle
	TimingClass( SystemDataManager& dataHandle );

	// Public functions
	void StartTimer();
	void UpdateTimer();
	void TaskTimerStart();
	void TaskTimerEnd();


private:
	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;

	// Private variables
	unsigned long long					  loopCounter = 0;
	std::chrono::steady_clock::time_point currentTime;
	std::chrono::steady_clock::time_point previousTime;
	std::chrono::duration<double>		  elapsedTime;
	std::chrono::steady_clock::time_point taskTimeStart;
	std::chrono::steady_clock::time_point taskTimeEnd;
	std::chrono::duration<double>		  taskTimeElapsed;
	bool								  TASK_TIMER_STARTED = false;
};