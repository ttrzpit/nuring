#pragma once

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
	char		ID;
	short		errXmm;
	short		errYmm;
	short		errZmm;
	short		errXpx;
	short		errYpx;
	short		magnitude;
	float		heading;
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

private:
	// Vector to log data
	std::vector<loggingStruct> logFile;

	// Variables
	loggingStruct newEntry;

	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;
};