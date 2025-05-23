#pragma once

// OpenCV
#include <opencv2/core.hpp>

// Memory for shared data
#include <memory>

// Configuration
#include <config.h>

// Forward declarations
class SystemDataManager;
struct ManagedData;

class TasksClass {

public:
	// Constructor
	TasksClass( SystemDataManager& dataHandle );

private:
	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;
};


class CalibrateClass {

public:
	// Constructor
	CalibrateClass( SystemDataManager & dataHandle );

	// Calibration tasks
	void CalibrateStart();
	void CalibrateEnd();

private:
	// Data manager handle
	SystemDataManager & dataHandle;
	std::shared_ptr<ManagedData> shared;


	// Calibration update
	void CalibrateUpdate();
};
