// Call to class header
#include "CaptureClass.h"

// System data manager
#include "SystemDataManager.h"

// Constructor
CaptureClass::CaptureClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	std::cout << "Capture: Thread " << cv::getThreadNum() << " of " << cv::getNumThreads() << "\n";
	Initialize();
}


void CaptureClass::Initialize() {



	// Configure OpenCV Optimizations
	cv::setUseOptimized( true );

	// Initialize undistort map tool
	cv::initUndistortRectifyMap( CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, cv::Mat(), CONFIG_CAMERA_MATRIX, shared->matFrameRaw.size(), CV_32F, shared->matRemap1, shared->matRemap2 );

	// Try catch to open camera device
	try {
		// Capture device
		Capture.open( "/dev/video0", cv::CAP_V4L2 );

		// Capture settings
		Capture.set( cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc( 'M', 'J', 'P', 'G' ) );
		Capture.set( cv::CAP_PROP_FRAME_WIDTH, CONFIG_CAM_WIDTH );
		Capture.set( cv::CAP_PROP_FRAME_HEIGHT, CONFIG_CAM_HEIGHT );
		Capture.set( cv::CAP_PROP_FPS, CONFIG_CAM_FRAMERATE );
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
		if ( !Capture.isOpened() ) {
			std::cerr << "Error: Could not open the camera." << std::endl;
		}
	} catch ( cv::Exception& e ) {
		std::cout << "Exception in CameraInterface::Begin(): " << e.msg << "\n";
	}

	// Output confirmation
	std::cout << "CaptureClass: Camera initialized at ( " << Capture.get( cv::CAP_PROP_FRAME_HEIGHT ) << " x " << Capture.get( cv::CAP_PROP_FRAME_WIDTH ) << " ) @ " << Capture.get( cv::CAP_PROP_FPS ) << " fps, mode = " << Capture.get( cv::CAP_PROP_BACKEND )
			  << " , Accel = " << Capture.get( cv::CAP_PROP_HW_ACCELERATION ) << "\n";
}

/**
 * @brief Captures the latest frame
 */
void CaptureClass::GetFrame() {

	// Reset flag
	shared->FLAG_FRAME_READY = false;

	// Capture latest frame
	Capture.grab();
	Capture.retrieve( shared->matFrameRaw );

	// Make sure frame isn't empty
	if ( !shared->matFrameRaw.empty() ) {

		// Upload mats to GPU
		shared->GpuMatFrameRaw.upload( shared->matFrameRaw );
		shared->GpuMatRemap1.upload( shared->matRemap1 );
		shared->GpuMatRemap2.upload( shared->matRemap2 );

		// Remap using GPU
		cv::cuda::remap( shared->GpuMatFrameRaw, shared->GpuMatFrameUndistorted, shared->GpuMatRemap1, shared->GpuMatRemap2, cv::INTER_NEAREST );

		// Convert to grayscale using GPU
		cv::cuda::cvtColor( shared->GpuMatFrameUndistorted, shared->GpuMatFrameGray, cv::COLOR_BGR2GRAY );

		// Extract frames from GPU
		shared->GpuMatFrameUndistorted.download( shared->matFrameUndistorted );
		shared->GpuMatFrameGray.download( shared->matFrameGray );

		// Update flag
		shared->FLAG_FRAME_READY = true;

	} else {
		std::cerr << "CaptureClass: Captured frame is empty!\n";
		shared->FLAG_FRAME_READY = false;
	}
}
