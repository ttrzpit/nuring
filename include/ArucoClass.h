#pragma once

// Memory for shared data
#include <memory>

// OpenCV core functions
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>



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

	// Private variables


	// ArUco detector
	cv::aruco::Dictionary		  arucoDictionary;
	cv::aruco::DetectorParameters arucoDetectorParams;
	cv::aruco::ArucoDetector	  arucoDetector;

	// Aruco variables
	std::vector<bool>					  arucoTagsPresent;												// Is a given tag present?
	std::vector<int>					  arucoDetectedIDs;												// Collection of IDs detected
	// cv::Vec3d							  arucoRotationVector	  = cv::Vec3d( 0, 0, 0 );				// Rotation vector
	// cv::Vec3d							  arucoTranslationVector  = cv::Vec3d( 0, 0, 0 );				// Translation vector
	// bool								  arucoDetected			  = false;								// Is there a tag detected
	// uint8_t								  arucoActiveID			  = 0;									// ID of desired tag
	cv::Point3f							  arucoPositionError3dNew = cv::Point3f( 0.0f, 0.0f, 0.0f );	// [mm] New raw position of tag relative to camera
	cv::Point2i							  arucoPositionError2d	  = cv::Point2i( 0, 0 );				// [px] Position of tag relative to camera
	std::vector<std::vector<cv::Point2f>> arucoCorners, arucoRejects;
	std::vector<cv::Vec3d>				  arucoRotationVector, arucoTranslationVector;
	cv::Mat								  arucoPoints { 4, 1, CV_32FC3 };
	std::vector<cv::Point2i>			  arucoActiveCorners = { cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ), cv::Point2i( 0, 0 ) };
};
