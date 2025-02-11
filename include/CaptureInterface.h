#pragma once

// OpenCV libraries
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d/calib3d.hpp>	  // For undistort
#include <opencv2/core.hpp>				  // Core OpenCV functions
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/highgui.hpp>	  // For pollKey()
#include <opencv2/imgproc.hpp>	  // For remapping
#include <opencv2/opencv.hpp>


// #include <opencv2/objdetect/aruco_detector.hpp> // For AruCo tags
// #include <opencv2/videoio.hpp> // For video capture

class CaptureInterface {

public:
	// Function prototypes
	void GetFrame();
	void FindTags();

	// Variables
	bool	frameCaptured = false;
	bool	markerFound	  = false;
	cv::Mat matFrame	  = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::Mat matMarkers	  = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::Mat matGray		  = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC1 );


	// Constructor
	CaptureInterface() {
		BeginCapture();
		BeginAruco();
	}

	// Deconstructor
	~CaptureInterface() {
		std::cout << "OpenCV shutting down...\n";
		Capture.release();
		exit( 0 );
	}

private:
	// Functions
	void BeginCapture();
	void BeginAruco();

	// Camera properties
	cv::VideoCapture Capture;
	cv::Mat			 matRawFrame = cv::Mat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::Mat			 matRemap1;
	cv::Mat			 matRemap2;
	cv::cuda::GpuMat gpuRawFrame		 = cv::cuda::GpuMat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	cv::cuda::GpuMat gpuGrayFrame		 = cv::cuda::GpuMat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC1 );
	cv::cuda::GpuMat gpuUndistortedFrame = cv::cuda::GpuMat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC1 );
	cv::cuda::GpuMat gpuRemap1;
	cv::cuda::GpuMat gpuRemap2;

	// Aruco properties
	cv::aruco::Dictionary				  dictionary;
	cv::aruco::DetectorParameters		  detectorParams;
	cv::aruco::ArucoDetector			  arucoDetector;
	std::vector<int>					  markerIDs;
	std::vector<std::vector<cv::Point2f>> markerCorners, markerRejects;
	std::vector<cv::Vec3d>				  rotationVector, translationVector;
	std::vector<int>					  markersPresent = { 0, 0, 0, 0, 0, 0 };
	cv::Mat								  objPoints{ 4, 1, CV_32FC3 };
};

// capture.release() ;

void CaptureInterface::BeginCapture() {

	// Create and initialize camera
	cv::setUseOptimized( true );	// Use optimizations

	// Initialize undistort map to speed up remapping
	cv::initUndistortRectifyMap( CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, cv::Mat(), CONFIG_CAMERA_MATRIX, matRawFrame.size(), CV_32F, matRemap1, matRemap2 );

	// Try-catch for opening camera interface
	try {

		// Capture device
		Capture.open( "/dev/video0", cv::CAP_V4L2 );

		// Capture settings
		Capture.set( cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc( 'M', 'J', 'P', 'G' ) );
		Capture.set( cv::CAP_PROP_FRAME_WIDTH, CONFIG_CAM_WIDTH );
		Capture.set( cv::CAP_PROP_FRAME_HEIGHT, CONFIG_CAM_HEIGHT );
		Capture.set( cv::CAP_PROP_FPS, CONFIG_CAM_FRAMERATE );
		Capture.set( cv::CAP_PROP_AUTO_EXPOSURE, CONFIG_CAM_AUTO_EXPOSURE );
		Capture.set( cv::CAP_PROP_AUTO_WB, CONFIG_CAM_AUTO_WHITEBALANCE );
		Capture.set( cv::CAP_PROP_FOCUS, CONFIG_CAM_AUTO_FOCUS );
		Capture.set( cv::CAP_PROP_GAIN, CONFIG_CAM_GAIN_LEVEL );
		Capture.set( cv::CAP_PROP_EXPOSURE, CONFIG_CAM_EXPOSURE_LEVEL );
		Capture.set( cv::CAP_PROP_HW_ACCELERATION, cv::VIDEO_ACCELERATION_ANY );

		// Ensure capture devices open and running
		if( !Capture.isOpened() ) {
			std::cerr << "Error: Could not open the camera." << std::endl;
		}
	} catch( cv::Exception& e ) {
		std::cout << "Exception in CameraInterface::Begin(): " << e.msg << "\n";
	}

	std::cout << "\nInitializeCamera: Camera initialized at ( " << Capture.get( cv::CAP_PROP_FRAME_HEIGHT ) << " x " << Capture.get( cv::CAP_PROP_FRAME_WIDTH ) << " ) @ " << Capture.get( cv::CAP_PROP_FPS ) << " fps, mode = " << Capture.get( cv::CAP_PROP_BACKEND )
			  << " , Accel = " << Capture.get( cv::CAP_PROP_HW_ACCELERATION ) << "\n\n";
	;
}



/**
 * @brief Capture and return a single frame
 */
void CaptureInterface::GetFrame() {

	frameCaptured = false;

	//   Capture >> matRawFrame;
	// Capture.read(matRawFrame) ;
	Capture.grab();
	Capture.retrieve( matRawFrame );

	// Upload to GPU
	gpuRawFrame.upload( matRawFrame );

	// Make sure frame isn't empty
	if( matRawFrame.empty() ) {
		std::cerr << "CameraInterface::GetRawFrame: Empty frame captured.\n";
		frameCaptured = false;
	} else {

		// Upload remaps to gpu
		gpuRemap1.upload( matRemap1 );
		gpuRemap2.upload( matRemap2 );

		// Remap using GPU
		cv::cuda::remap( gpuRawFrame, gpuUndistortedFrame, gpuRemap1, gpuRemap2, cv::INTER_LINEAR );

		// Extract frame for visualization
		gpuUndistortedFrame.download( matFrame );

		// Convert to grayscale
		cv::cuda::cvtColor( gpuUndistortedFrame, gpuGrayFrame, cv::COLOR_BGR2GRAY );

		// Extract from GPU
		gpuGrayFrame.download( matGray );

		frameCaptured = true;
	}
}


/**
 * @brief Initialize aruco detector
 */
void CaptureInterface::BeginAruco() {

	// Assign dictionary
	dictionary = cv::aruco::getPredefinedDictionary( cv::aruco::DICT_4X4_50 );

	// Assign ArUco detector parameters
	detectorParams.adaptiveThreshWinSizeMin		 = 3;		// 3
	detectorParams.adaptiveThreshWinSizeMax		 = 23;		// 23
	detectorParams.adaptiveThreshWinSizeStep	 = 10;		// 10
	detectorParams.minMarkerPerimeterRate		 = 0.01;	// 0.03
	detectorParams.maxMarkerPerimeterRate		 = 4.0;		// 4.0
	detectorParams.minCornerDistanceRate		 = 0.05;	// 0.05
	detectorParams.minDistanceToBorder			 = 3;		// 3
	detectorParams.cornerRefinementMethod		 = cv::aruco::CORNER_REFINE_SUBPIX;
	detectorParams.cornerRefinementMaxIterations = 30;	   // 30
	detectorParams.cornerRefinementMinAccuracy	 = 0.1;	   // 0.1

	// Create new aruco detector object
	arucoDetector = cv::aruco::ArucoDetector( dictionary, detectorParams );

	// Configure real-world conversion system
	objPoints.ptr<cv::Vec3f>( 0 )[ 0 ] = cv::Vec3f( -CONFIG_MARKER_WIDTH / 2.f, CONFIG_MARKER_WIDTH / 2.f, 0 );
	objPoints.ptr<cv::Vec3f>( 0 )[ 1 ] = cv::Vec3f( CONFIG_MARKER_WIDTH / 2.f, CONFIG_MARKER_WIDTH / 2.f, 0 );
	objPoints.ptr<cv::Vec3f>( 0 )[ 2 ] = cv::Vec3f( CONFIG_MARKER_WIDTH / 2.f, -CONFIG_MARKER_WIDTH / 2.f, 0 );
	objPoints.ptr<cv::Vec3f>( 0 )[ 3 ] = cv::Vec3f( -CONFIG_MARKER_WIDTH / 2.f, -CONFIG_MARKER_WIDTH / 2.f, 0 );
}


void CaptureInterface::FindTags() {

	// Run detector
	arucoDetector.detectMarkers( matGray, markerCorners, markerIDs, markerRejects );

	// Check if markers found
	if( !markerIDs.empty() ) {

		// Clear previous marker matrix
		matMarkers = 0;

		// Draw markers
		cv::aruco::drawDetectedMarkers( matFrame, markerCorners, markerIDs );

		// Estimate pose
		cv::aruco::estimatePoseSingleMarkers( markerCorners, CONFIG_MARKER_WIDTH, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, rotationVector, translationVector );


		// Process each detected marker
		for( size_t i = 0; i < markerIDs.size(); i++ ) {

			// Only process markers in valid set
			if( markerIDs[ i ] > 0 && markerIDs[ i ] < 6 ) {

				// Update list of present markers

				markersPresent[ i ] = 1;
				cv::drawFrameAxes( matFrame, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, rotationVector[ i ], translationVector[ i ], CONFIG_MARKER_WIDTH, 1 );
			}
		}
	}
}