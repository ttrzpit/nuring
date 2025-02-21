#pragma once

// Memory for shared data
#include <memory>

// OpenCV core functions
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>



// Forward declarations
class SystemDataManager;
struct ManagedData;



/** 
 * @brief Display class definition
 */
class ArucoClass {

public:
	// Data manager handle
	ArucoClass( SystemDataManager& dataHandle );

	// Public functions
	void FindTags();


private:
	// Private functions
	void Initialize();

	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;
};