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


struct Marker {
	short		ID			   = 0;
	bool		present		   = false;
	cv::Point3i error3D		   = cv::Point3i( 0, 0, 0 );
	cv::Point2i error2D		   = cv::Point2i( 0, 0 );
	float		theta		   = 0.0f;
	short		errorMagnitude = 0;
};


// Class for the capture interface
class CaptureInterface {

public:
	// Function prototypes
	void GetFrame();
	void FindTags();

	// Variables
	bool				frameCaptured = false;
	bool				markerFound	  = false;
	cv::Mat				matFrame	  = cv::Mat( CONFIG_CAP_HEIGHT, CONFIG_CAP_WIDTH, CV_8UC3 );
	cv::Mat				matMarkers	  = cv::Mat( CONFIG_CAP_HEIGHT, CONFIG_CAP_WIDTH, CV_8UC3 );
	cv::Mat				matGray		  = cv::Mat( CONFIG_CAP_HEIGHT, CONFIG_CAP_WIDTH, CV_8UC1 );
	std::vector<Marker> Markers;
	short				activeMarker = 0;

	// Constructor
	CaptureInterface()
		: Markers( 10 ) {

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
	cv::Mat			 matRawFrame = cv::Mat( CONFIG_CAP_HEIGHT, CONFIG_CAP_WIDTH, CV_8UC3 );
	cv::Mat			 matRemap1;
	cv::Mat			 matRemap2;
	cv::cuda::GpuMat gpuRawFrame		 = cv::cuda::GpuMat( CONFIG_CAP_HEIGHT, CONFIG_CAP_WIDTH, CV_8UC3 );
	cv::cuda::GpuMat gpuGrayFrame		 = cv::cuda::GpuMat( CONFIG_CAP_HEIGHT, CONFIG_CAP_WIDTH, CV_8UC1 );
	cv::cuda::GpuMat gpuUndistortedFrame = cv::cuda::GpuMat( CONFIG_CAP_HEIGHT, CONFIG_CAP_WIDTH, CV_8UC1 );
	cv::cuda::GpuMat gpuRemap1;
	cv::cuda::GpuMat gpuRemap2;

	// Aruco properties
	cv::aruco::Dictionary				  dictionary;
	cv::aruco::DetectorParameters		  detectorParams;
	cv::aruco::ArucoDetector			  arucoDetector;
	std::vector<int>					  markerIDs;
	std::vector<std::vector<cv::Point2f>> markerCorners, markerRejects;
	std::vector<cv::Vec3d>				  rotationVector, translationVector;
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
		Capture.set( cv::CAP_PROP_FRAME_WIDTH, CONFIG_CAP_WIDTH );
		Capture.set( cv::CAP_PROP_FRAME_HEIGHT, CONFIG_CAP_HEIGHT );
		Capture.set( cv::CAP_PROP_FPS, CONFIG_CAP_FRAMERATE );
		Capture.set( cv::CAP_PROP_HW_ACCELERATION, cv::VIDEO_ACCELERATION_ANY );

		// Camera settings
		Capture.set( cv::CAP_PROP_BRIGHTNESS, CONFIG_CAM_BRIGHTNESS );
		Capture.set( cv::CAP_PROP_CONTRAST, CONFIG_CAM_CONTRAST );
		Capture.set( cv::CAP_PROP_SATURATION, CONFIG_CAM_SATURATION );
		Capture.set( cv::CAP_PROP_HUE, CONFIG_CAM_HUE );
		Capture.set( cv::CAP_PROP_AUTO_WB, CONFIG_CAM_AUTO_WHITEBALANCE );
		Capture.set( cv::CAP_PROP_GAMMA, CONFIG_CAM_GAMMA );
		Capture.set( cv::CAP_PROP_GAIN, CONFIG_CAM_GAIN );
		Capture.set( cv::CAP_PROP_SHARPNESS, CONFIG_CAM_SHARPNESS );
		Capture.set( cv::CAP_PROP_BACKLIGHT, CONFIG_CAM_BACKLIGHT );
		Capture.set( cv::CAP_PROP_AUTO_EXPOSURE, CONFIG_CAM_AUTO_EXPOSURE );
		Capture.set( cv::CAP_PROP_EXPOSURE, CONFIG_CAM_EXPOSURE_LEVEL );
		Capture.set( cv::CAP_PROP_PAN, CONFIG_CAM_PAN );
		Capture.set( cv::CAP_PROP_TILT, CONFIG_CAM_TILT );
		Capture.set( cv::CAP_PROP_FOCUS, CONFIG_CAM_FOCUS_LEVEL );
		Capture.set( cv::CAP_PROP_AUTOFOCUS, CONFIG_CAM_AUTO_FOCUS );
		Capture.set( cv::CAP_PROP_ZOOM, CONFIG_CAM_ZOOM );

		// Ensure capture devices open and running
		if( !Capture.isOpened() ) {
			std::cerr << "Error: Could not open the camera." << std::endl;
		}
	} catch( cv::Exception& e ) {
		std::cout << "Exception in CameraInterface::Begin(): " << e.msg << "\n";
	}

	std::cout << "\nInitializeCamera: Camera initialized at ( " << Capture.get( cv::CAP_PROP_FRAME_HEIGHT ) << " x " << Capture.get( cv::CAP_PROP_FRAME_WIDTH ) << " ) @ " << Capture.get( cv::CAP_PROP_FPS ) << " fps, mode = " << Capture.get( cv::CAP_PROP_BACKEND )
			  << " , Accel = " << Capture.get( cv::CAP_PROP_HW_ACCELERATION ) << "\n";
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
		cv::cuda::remap( gpuRawFrame, gpuUndistortedFrame, gpuRemap1, gpuRemap2, cv::INTER_NEAREST );

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
	detectorParams.adaptiveThreshConstant		 = 7;
	detectorParams.adaptiveThreshWinSizeMin		 = 3;								   // 3
	detectorParams.adaptiveThreshWinSizeMax		 = 33;								   // 23
	detectorParams.adaptiveThreshWinSizeStep	 = 10;								   // 10
	detectorParams.minMarkerPerimeterRate		 = 0.03;							   // 0.03
	detectorParams.maxMarkerPerimeterRate		 = 4.0;								   // 4.0
	detectorParams.minCornerDistanceRate		 = 0.05;							   // 0.05
	detectorParams.minDistanceToBorder			 = 3;								   // 3
	detectorParams.cornerRefinementMethod		 = cv::aruco::CORNER_REFINE_SUBPIX;	   // cv::aruco::CORNER_REFINE_SUBPIX;
	detectorParams.cornerRefinementMaxIterations = 30;								   // 30
	detectorParams.cornerRefinementMinAccuracy	 = 0.1;								   // 0.1
	// detectorParams.useAruco3Detection			 = true;

	// Create new aruco detector object
	arucoDetector = cv::aruco::ArucoDetector( dictionary, detectorParams );

	// Configure real-world conversion system
	objPoints.ptr<cv::Vec3f>( 0 )[0] = cv::Vec3f( -CONFIG_MARKER_WIDTH / 2.f, CONFIG_MARKER_WIDTH / 2.f, 0 );
	objPoints.ptr<cv::Vec3f>( 0 )[1] = cv::Vec3f( CONFIG_MARKER_WIDTH / 2.f, CONFIG_MARKER_WIDTH / 2.f, 0 );
	objPoints.ptr<cv::Vec3f>( 0 )[2] = cv::Vec3f( CONFIG_MARKER_WIDTH / 2.f, -CONFIG_MARKER_WIDTH / 2.f, 0 );
	objPoints.ptr<cv::Vec3f>( 0 )[3] = cv::Vec3f( -CONFIG_MARKER_WIDTH / 2.f, -CONFIG_MARKER_WIDTH / 2.f, 0 );
}



void CaptureInterface::FindTags() {

	// Reset marker state
	for( size_t c = 0; c < Markers.size(); c++ ) {
		Markers[c].present = false;
	}

	// Clear active marker
	// activeMarker = 0;

	// Run detector
	arucoDetector.detectMarkers( matGray, markerCorners, markerIDs, markerRejects );

	// Check if markers found
	if( !markerIDs.empty() ) {

		// Update marker flag
		markerFound = true;

		// Clear previous marker matrix
		matMarkers = 0;

		// // Draw markers
		cv::aruco::drawDetectedMarkers( matFrame, markerCorners, markerIDs );

		// Estimate pose
		cv::aruco::estimatePoseSingleMarkers( markerCorners, CONFIG_MARKER_WIDTH, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, rotationVector, translationVector );


		// Process each detected marker
		for( size_t i = 0; i < markerIDs.size(); i++ ) {

			// Only process markers in valid set
			if( markerIDs[i] > 0 && markerIDs[i] < 11 ) {

				// Shortcut for index
				short index = markerIDs[i];

				// // If no active marker set, set first one detected
				// if( activeMarker == 0 ) {
				// 	activeMarker = index;
				// }

				// Calculate 2D pixel space
				int avgX = int( ( markerCorners[i][0].x + markerCorners[i][1].x + markerCorners[i][2].x + markerCorners[i][3].x ) / 4.0f );
				int avgY = int( ( markerCorners[i][0].y + markerCorners[i][1].y + markerCorners[i][2].y + markerCorners[i][3].y ) / 4.0f );

				// Update list of present markers
				Markers[index].present = true;

				// Update marker positions
				Markers[index].error3D		  = cv::Point3i( int( translationVector[i][0] ), int( translationVector[i][1] ), int( translationVector[i][2] ) );
				Markers[index].error2D		  = cv::Point2d( int( ( markerCorners[i][0].x + markerCorners[i][1].x + markerCorners[i][2].x + markerCorners[i][3].x ) / 4.0f ), int( ( markerCorners[i][0].y + markerCorners[i][1].y + markerCorners[i][2].y + markerCorners[i][3].y ) / 4.0f ) );
				Markers[index].theta		  = rotationVector[i][1];
				Markers[index].errorMagnitude = sqrt( ( Markers[index].error3D.x * Markers[index].error3D.x ) + ( Markers[index].error3D.y * Markers[index].error3D.y ) + ( Markers[index].error3D.z * Markers[index].error3D.z ) );

				// Draw frame axis
				cv::drawFrameAxes( matFrame, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, rotationVector[i], translationVector[i], CONFIG_MARKER_WIDTH, 1 );
			}
		}
	} else {
		markerFound = false;
	}
}