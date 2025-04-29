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
	void ShowShortcuts();
	void ShowVisualizer();
	void UpdateVisualizer();
	void ClearVisualizer();

	// Public variables
	std::vector<cv::Point3i> trailPoints;
	std::vector<int>		 trailColor;
	int						 trailCounter  = 0;
	int						 trailInterval = 1;
	int						 trailLimit	   = 1000;
	int						 zOffset	   = 0;

private:
	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;

	// Private functions
	void		ShowInterface();
	void		AddText();
	void		DrawCell( std::string str, std::string cell0, short width, short height, float sz, cv::Scalar textColor, cv::Scalar fillColor, bool centered );
	void		DrawKeyCell( std::string str, std::string cell0, short width, short height, float sz, cv::Scalar textColor, cv::Scalar fillColor, bool centered );
	cv::Point2i ProjectIsometric( const cv::Point3i& p3d );
	cv::Point2i GetForwardDirectionFromPose( const cv::Vec3d rvec, const cv::Vec3d tvec, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, float axisLength );

	// Private variables
	float	 fontHeader = 0.0f;
	float	 fontBody	= 0.0f;
	cv::Size textSize;
	// short	 cellPx = 40;
	short WIDTH	 = CONFIG_DIS_CELL_WIDTH;
	short HEIGHT = CONFIG_DIS_CELL_HEIGHT;
	short c0	 = 0;
	short r0	 = 0;
	short nR	 = 0;
	short nC	 = 0;
	short cW	 = 0;
	short rH	 = 0;
	short textX	 = 0;
	short textY	 = 0;

	// Keyboard shortcut variables
	float	 key_fontHeader = 0.0f;
	float	 key_fontBody	= 0.0f;
	cv::Size key_textSize;
	// short	 key_cellPx = 40;
	short key_WIDTH	 = CONFIG_DIS_KEY_CELL_WIDTH;
	short key_HEIGHT = CONFIG_DIS_KEY_CELL_HEIGHT;
	short key_c0	 = 0;
	short key_r0	 = 0;
	short key_nR	 = 0;
	short key_nC	 = 0;
	short key_cW	 = 0;
	short key_rH	 = 0;
	short key_textX	 = 0;
	short key_textY	 = 0;

	// Visualizer settings
	std::vector<cv::Point2i> ProjectedCorners;
	cv::Mat					 matShortcuts  = cv::Mat( CONFIG_DIS_HEIGHT, CONFIG_DIS_KEY_WIDTH, CV_8UC3 );
	cv::Mat					 matVisualizer = cv::Mat( 400, 600, CV_8UC3 );
	int						 vizLimXY	   = 500;
	int						 vizLimZ	   = 1000;
	const float				 focalLength   = 800.0f;
	float					 azimuth	   = CV_PI / 4;		// 45 degrees 4
	float					 elevation	   = CV_PI / 16;	// 30 degrees 6

	// Define 3D corners of a cube
	const std::vector<cv::Point3i> cubeCorners = { { -vizLimXY, -vizLimXY, 0 }, { vizLimXY, -vizLimXY, 0 }, { vizLimXY, vizLimXY, 0 }, { -vizLimXY, vizLimXY, 0 }, { -vizLimXY, -vizLimXY, vizLimZ }, { vizLimXY, -vizLimXY, vizLimZ }, { vizLimXY, vizLimXY, vizLimZ }, { -vizLimXY, vizLimXY, vizLimZ } };



	// Edges of 3D cube
	const std::vector<std::pair<int, int>> edges = {
		{ 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 },	   // bottom face
		{ 4, 5 }, { 5, 6 }, { 6, 7 }, { 7, 4 },	   // top face
		{ 0, 4 }, { 1, 5 }, { 2, 6 }, { 3, 7 }	   // vertical edges
	};
};