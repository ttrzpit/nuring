#pragma once

#include "config.h"

// OpenCV libraries
#include <opencv2/core.hpp>                         // Core OpenCV functions
#include <opencv2/highgui.hpp>                      // For pollKey() 
#include <opencv2/videoio.hpp>                      // For video capture
#include <opencv2/core/utils/logger.hpp>            // For logging
#include <opencv2/calib3d/calib3d.hpp>              // For undistort
#include <opencv2/aruco.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>     // For AruCo tags
// #include <opencv2/imgproc/imgproc.hpp>              // For resize


class ArucoInterface {

    public:

    // Constructor
    ArucoInterface() {
        Begin() ;
    }

    // Function prototypes
    bool Begin() ;


    private:
    cv::aruco::Dictionary                   dictionary;
    cv::aruco::DetectorParameters           detectorParams;
    cv::aruco::ArucoDetector                arucoDetector;
    std::vector<int>                        markerIDs;
    std::vector<std::vector<cv::Point2f>>   markerCorners , markerRejects;
    std::vector<cv::Vec3d>                  rotationVector , translationVector ;
    std::vector<int>                        markersPresent = { 0 , 0 , 0 , 0 , 0 , 0 }  ;
    cv::Mat                                 objPoints { 4 , 1 , CV_32FC3 };

} ;


bool ArucoInterface::Begin() {

    // Define aruco parameters
    dictionary = cv::aruco::getPredefinedDictionary( cv::aruco::DICT_4X4_50 ) ;
    detectorParams = cv::aruco::DetectorParameters() ;

    // Configure real-world conversion system
    objPoints.ptr<cv::Vec3f>( 0 )[0] = cv::Vec3f( CONFIG_MARKER_WIDTH / -2.f , CONFIG_MARKER_WIDTH / 2.f , 0 );
    objPoints.ptr<cv::Vec3f>( 0 )[1] = cv::Vec3f( CONFIG_MARKER_WIDTH / 2.f , CONFIG_MARKER_WIDTH / 2.f , 0 );
    objPoints.ptr<cv::Vec3f>( 0 )[2] = cv::Vec3f( CONFIG_MARKER_WIDTH / 2.f , CONFIG_MARKER_WIDTH / -2.f , 0 );
    objPoints.ptr<cv::Vec3f>( 0 )[3] = cv::Vec3f( CONFIG_MARKER_WIDTH / -2.f , CONFIG_MARKER_WIDTH / -2.f , 0 );

    // Adjust ArUco detector parameters
    detectorParams.adaptiveThreshWinSizeMin = 3     ;   // 3
    detectorParams.adaptiveThreshWinSizeMax = 23    ;   // 23
    detectorParams.adaptiveThreshWinSizeStep = 10    ;   // 10
    detectorParams.minMarkerPerimeterRate = 0.01  ;   // 0.03
    detectorParams.maxMarkerPerimeterRate = 4.0   ;   // 4.0
    detectorParams.minCornerDistanceRate = 0.05  ;   // 0.05
    detectorParams.minDistanceToBorder = 3     ;   // 3
    detectorParams.cornerRefinementMethod = cv::aruco::CORNER_REFINE_SUBPIX ;
    detectorParams.cornerRefinementMaxIterations = 30    ;   // 30
    detectorParams.cornerRefinementMinAccuracy = 0.1   ;  // 0.1 

    // Re-initialize ArUco detector to apply parameter edits
    try {
        arucoDetector = cv::aruco::ArucoDetector( dictionary , detectorParams ) ;
        return true ;
    } catch (cv::Exception& e) {
        std::cout << "Exception in CameraInterface::Begin(): " << e.msg << "\n" ;
        return false ;
    }

    return false ;

}
