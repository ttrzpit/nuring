#pragma once

// Necessary libraries
#include <iostream>

// Chrono for benchmarking time
#include <chrono>


class TimingInterface {

public:
	// Constructor
	TimingInterface() {
		// Nothing
	}

	// Functions
	void StartTimer();
	void CheckFrequency();


private:
	double									   loopFrequency = 0.0;
	unsigned long long						   loopCounter	 = 0;
	std::chrono::_V2::system_clock::time_point currentTime;
	std::chrono::_V2::system_clock::time_point previousTime;
	std::chrono::duration<double>			   elapsedTime;
};


/** 
 * @brief Start the timer
 */
void TimingInterface::StartTimer() {

	// Capture current time
	previousTime = std::chrono::high_resolution_clock::now();
}



/**
 * @brief Checks the frequency
 */
void TimingInterface::CheckFrequency() {

	// Increment loop counter
	loopCounter++;

	// Get current time
	currentTime = std::chrono::high_resolution_clock::now();

	// Get elapsed time
	elapsedTime = currentTime - previousTime;

	// Check if 1 second has passed
	if( elapsedTime.count() >= 1.0 ) {
		loopFrequency = loopCounter / elapsedTime.count();
		std::cout << "Loop Frequency: " << loopFrequency << " Hz.\n";
		loopCounter	 = 0;
		previousTime = currentTime;
	}
}