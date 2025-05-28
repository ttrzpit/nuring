#pragma once

// Libraries
#include <fstream>	  // For saving files
#include <iomanip>	  // For padding zeroes

// OpenCV core functions
#include <opencv2/core.hpp>

// Memory for shared data
#include <memory>

// Configuration
#include <config.h>

// Forward declarations
class SystemDataManager;
struct ManagedData;

/* Struct for data to be logged*/
struct loggingStruct {

	float		timestamp;
	short		errXmm;
	short		errYmm;
	short		errZmm;
	std::string custom1;
	std::string custom2;
	std::string custom3;
	std::string custom4;
	std::string custom5;
	
};



class LoggingClass {

public:
	// Data manager
	LoggingClass( SystemDataManager& dataHandle );

	// Public functions
	void AddEntry();
	void Save();
	void Initialize();


private:
	// Private functions
	std::string PadValues( int val, int nZeroes );

	// Vector to log data
	std::vector<loggingStruct> logFile;

	// Variables
	loggingStruct newEntry;

	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;
};