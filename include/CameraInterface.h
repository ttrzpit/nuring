#pragma once

// OpenCV libraries
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>                         // Core OpenCV functions
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>                      // For pollKey() 
#include <opencv2/videoio.hpp>                      // For video capture
#include <opencv2/calib3d/calib3d.hpp>              // For undistort
#include <opencv2/imgproc.hpp>                      // For remapping

class CameraInterface {

    public:
    cv::Mat GetRawFrame() ;

    // Constructor
    CameraInterface() {
        Begin() ;
    }

    // Deconstructor
    ~CameraInterface() {
        std::cout << "OpenCV shutting down...\n" ;
        Capture.release() ;
    }

    // Function prototypes
    bool Begin() ;

    private:
    cv::VideoCapture Capture ;
    cv::Mat matRawFrame = cv::Mat( CONFIG_CAM_HEIGHT , CONFIG_CAM_WIDTH , CV_8UC3 ) ;
    cv::Mat matGray = cv::Mat( CONFIG_CAM_HEIGHT , CONFIG_CAM_WIDTH , CV_8UC1 ) ;
    cv::Mat matUndistorted = cv::Mat( CONFIG_CAM_HEIGHT , CONFIG_CAM_WIDTH , CV_8UC1 ) ;
    cv::Mat matRemap1 ;
    cv::Mat matRemap2 ;


} ;


// capture.release() ;

bool CameraInterface::Begin() {

    // Create and initialize camera
    cv::setUseOptimized( true );                                // Use optimizations

    // Initialize undistort map to speed up remapping
    cv::initUndistortRectifyMap( CONFIG_CAMERA_MATRIX ,
        CONFIG_DISTORTION_COEFFS ,
        cv::Mat() ,
        CONFIG_CAMERA_MATRIX ,
        matRawFrame.size() ,
        CV_16SC2 ,
        matRemap1 ,
        matRemap2 ) ;

    // Try-catch for opening camera interface
    try {

        // Capture device using DirectShow 
        Capture.open( 0 ) ;

        // Set capture settings
        // Capture.set( cv::CAP_PROP_FRAME_WIDTH , CONFIG_CAM_WIDTH ) ;
        // Capture.set( cv::CAP_PROP_FRAME_HEIGHT , CONFIG_CAM_HEIGHT ) ;
        // Capture.set( cv::CAP_PROP_FPS , CONFIG_CAM_FRAMERATE ) ;
        // Capture.set( cv::CAP_PROP_AUTO_EXPOSURE , CONFIG_CAM_AUTO_EXPOSURE ) ;
        // Capture.set( cv::CAP_PROP_AUTO_WB , CONFIG_CAM_AUTO_WHITEBALANCE ) ;
        // Capture.set( cv::CAP_PROP_FOCUS , false ) ;
        // Capture.set( cv::CAP_PROP_GAIN , CONFIG_CAM_GAIN_LEVEL ) ;
        // Capture.set( cv::CAP_PROP_EXPOSURE , CONFIG_CAM_EXPOSURE_LEVEL ) ;
        // Capture.set( cv::CAP_PROP_HW_ACCELERATION , cv::VIDEO_ACCELERATION_ANY ) ;
        // Capture.set( cv::CAP_PROP_BUFFERSIZE , 3 ) ;

        // Ensure capture devices open and running
        if ( !Capture.isOpened() ) {
            std::cerr << "Error: Could not open the camera." << std::endl;
            return false ;
        }

    } catch ( cv::Exception& e ) {
        std::cout << "Exception in CameraInterface::Begin(): " << e.msg << "\n" ;
    }


    std::cout << "\nInitializeCamera: Camera initialized at ( "
        << Capture.get( cv::CAP_PROP_FRAME_HEIGHT )
        << " x "
        << Capture.get( cv::CAP_PROP_FRAME_WIDTH )
        << " ) @ "
        << Capture.get( cv::CAP_PROP_FPS )
        << " fps, mode = "
        << Capture.get( cv::CAP_PROP_BACKEND )
        << "\n\n";
    ;

    return true ;

}

cv::Mat CameraInterface::GetRawFrame() {

    // Capture >> matRawFrame ;
    // Capture.read(matRawFrame) ; 
    Capture.grab() ;
    Capture.retrieve( matRawFrame ) ;

    // Make sure frame isn't empty
    if ( matRawFrame.empty() ) {
        std::cerr << "CameraInterface::GetRawFrame: Empty frame captured.\n" ;
        return cv::Mat( CONFIG_CAM_HEIGHT , CONFIG_CAM_WIDTH , CV_8UC3 ) ;
    } else {
        cv::remap ( matRawFrame , matUndistorted , matRemap1 , matRemap2 , cv::INTER_LINEAR ) ; 
        return matRawFrame ;
    }


}