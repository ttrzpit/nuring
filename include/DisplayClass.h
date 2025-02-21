#pragma once

// OpenCV core functions
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Memory for shared data
#include <memory>

// Configuration
#include <config.h>

// Forward declarations
class SystemDataManager;
struct ManagedData;


// Constants
#define RAD2DEG 57.2958



/** 
 * @brief Display class definition
 */
class DisplayClass {

public:
	// Data manager handle
	DisplayClass( SystemDataManager& dataHandle );

	// Public functions
	void Update();


private:
	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;

	// Private functions
	void ShowInterface();
	void AddText();
	void DrawCell( std::string str, std::string cell0, short width, short height, float sz, cv::Scalar textColor, cv::Scalar fillColor, bool centered );

	// Private variables
	float	 fontHeader = 0.0f;
	float	 fontBody	= 0.0f;
	cv::Size textSize;
	short	 cellPx = 40;
	short	 WIDTH	= CONFIG_DIS_CELL_WIDTH;
	short	 HEIGHT = CONFIG_DIS_CELL_HEIGHT;
	short	 c0		= 0;
	short	 r0		= 0;
	short	 nR		= 0;
	short	 nC		= 0;
	short	 cW		= 0;
	short	 rH		= 0;
	short	 textX	= 0;
	short	 textY	= 0;
};