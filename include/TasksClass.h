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

	// Calibration tasks
    void CalibrateStart() ;
    void CalibrateUpdate() ;
    void CalibrateEnd() ;

    // Angle tasks
    

} ;