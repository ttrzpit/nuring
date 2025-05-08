#include "CalibrationClass.h"

// System data manager
#include "SystemDataManager.h"



/**
 * @brief Calibration constructor
 */
CalibrationClass::CalibrationClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {


	// Stuff
}


/**
 * @brief Calibrate the camera and user finger
 */
void CalibrationClass::StartCalibration() {

	// Configure interface
	cv::namedWindow( winCalibration, cv::WINDOW_AUTOSIZE );
	cv::moveWindow( winCalibration, 3440, 0 );


	// Clear window
	matCalibration = CONFIG_colWhite;

	// Draw aruco target
	matAruco.copyTo( matCalibration( cv::Rect( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ) - ( matAruco.cols / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) - ( matAruco.rows / 2 ), matAruco.cols, matAruco.rows ) ) );

	// Draw target on touchscreen
	// cv::circle( matCalibration, cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH / 2, CONFIG_TOUCHSCREEN_HEIGHT / 2 ), 2.5 * MM2PX, CONFIG_colGreDk, -1 );

	// Add instructional text
	std::string line0 = "Please touch the screen while keeping the center of the camera aligned with the dot below. ";
	std::string line1 = "Completed: " + std::to_string( shared->TASK_REP_NUMBER ) + " / 5";
	cv::putText( matCalibration, line0, cv::Point( 10, 40 ), cv::FONT_HERSHEY_SIMPLEX, 1.2, CONFIG_colBlack, 2 );
	cv::putText( matCalibration, line1, cv::Point( 10, 80 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );


	// Show calibration screen
	cv::imshow( winCalibration, matCalibration );
	shared->calibrationLoaded = true;
}


/**
 * @brief Calibration update
 */
void CalibrationClass::Update() {


	// Check if calibration complete
	if ( shared->TASK_RUNNING == true ) {

		// Check if touchscreen pressed
		if ( shared->touchDetected == 1 ) {
			shared->TASK_REP_NUMBER++;
			calibrationPoints.push_back( shared->touchPosition - cv::Point3i( CONFIG_TOUCHSCREEN_WIDTH / 2, CONFIG_TOUCHSCREEN_HEIGHT / 2, 0 ) );
			calibrationZPoints.push_back( shared->arucoTags[shared->arucoActiveID].error3D.z );


			std::cout << "X: " << shared->touchPosition.x << " Y: " << shared->touchPosition.y << "\n";

			// Check if calibration complete
			if ( shared->TASK_REP_NUMBER <= 4 ) {
				shared->touchDetected = 0;
				StartCalibration();
			} else if ( shared->TASK_REP_NUMBER == 5 ) {
				shared->touchDetected		= 0;
				shared->calibrationComplete = true;
				FinishCalibration();
			} else {

				shared->TASK_RUNNING  = false;
				shared->TASK_NAME	  = "";
				shared->arucoActiveID = 0;

				// Check if window is loaded before destroying
				if ( shared->calibrationLoaded ) {
					cv::destroyWindow( winCalibration );
					shared->angleLoaded = false;
				}
			}
		}
	}
}



/**
 * @brief Finish calibration
 */
void CalibrationClass::FinishCalibration() {

	// Update console
	shared->displayString = "CalibrationClass: Calibration complete!";

	// Clear matc
	matCalibration = CONFIG_colWhite;

	// Copy to target window
	cv::rectangle( matCalibration, cv::Point2i( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ) - ( matAruco.cols / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) - ( matAruco.rows / 2 ) ), cv::Point2i( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ) + ( matAruco.cols / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) + ( matAruco.rows / 2 ) ),
				   CONFIG_colGreMd, 4 );
	matAruco.copyTo( matCalibration( cv::Rect( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ) - ( matAruco.cols / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) - ( matAruco.rows / 2 ), matAruco.cols, matAruco.rows ) ) );


	// Draw target on touchscreen
	// cv::circle( matCalibration, cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH / 2, CONFIG_TOUCHSCREEN_HEIGHT / 2 ), 2.5 * MM2PX, CONFIG_colGreMd, -1 );


	// Loop over touched points
	for ( int i = 0; i < calibrationPoints.size(); i++ ) {

		// Extract point
		cv::Point3i touchPoint = calibrationPoints[i] + cv::Point3i( CONFIG_TOUCHSCREEN_WIDTH / 2, CONFIG_TOUCHSCREEN_HEIGHT / 2, 0 );

		// Draw circle at touch point
		cv::circle( matCalibration, cv::Point2i( touchPoint.x, touchPoint.y ), 10, CONFIG_colBluLt, -1 );

		// Add to running average
		shared->calibrationOffsetMM = shared->calibrationOffsetMM + ( calibrationPoints[i] * PX2MM );

		// Debug
		std::cout << i << ": " << calibrationPoints[i].x << " , " << calibrationPoints[i].y << " , " << calibrationZPoints[i] << "\n";
	}

	// Calculate average
	shared->calibrationOffsetMM	  = ( shared->calibrationOffsetMM / 5.0f );
	shared->calibrationOffsetMM.z = std::accumulate( calibrationZPoints.begin(), calibrationZPoints.end(), 0 ) / 5.0f;


	// Draw lines to touch point
	cv::line( matCalibration, cv::Point2i( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), cv::Point2i( shared->calibrationOffsetMM.x * MM2PX + ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), shared->calibrationOffsetMM.y * MM2PX + ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), CONFIG_colVioLt, 2 );
	cv::line( matCalibration, cv::Point2i( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), cv::Point2i( shared->calibrationOffsetMM.x * MM2PX + ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), CONFIG_colRedMd, 2 );
	cv::line( matCalibration, cv::Point2i( shared->calibrationOffsetMM.x * MM2PX + ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ),
			  cv::Point2i( shared->calibrationOffsetMM.x * MM2PX + ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), shared->calibrationOffsetMM.y * MM2PX + ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), CONFIG_colGreMd, 2 );

	// Draw circle at offset
	cv::circle( matCalibration, cv::Point2i( shared->calibrationOffsetMM.x * MM2PX + ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), shared->calibrationOffsetMM.y * MM2PX + ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), 20, CONFIG_colBluMd, 2 );

	std::cout << "Avg: " << std::to_string( shared->calibrationOffsetMM.x ) << " , " << std::to_string( shared->calibrationOffsetMM.y ) << std::to_string( shared->calibrationOffsetMM.z ) << "\n";

	// Completion text
	std::string line0 = "Calibration complete!";
	std::string line1 = "Averaged Error [px] X: " + std::to_string( int( shared->calibrationOffsetMM.x * MM2PX ) ) + "px   Y: " + std::to_string( int( shared->calibrationOffsetMM.y * MM2PX ) ) + "px   Z: " + std::to_string( int( shared->calibrationOffsetMM.z * MM2PX ) )
		+ "px   |R|: " + std::to_string( int( cv::norm( shared->calibrationOffsetMM ) * MM2PX ) ) + "px ";
	;
	std::string line2 = "Averaged Error [mm] X: " + std::to_string( int( shared->calibrationOffsetMM.x ) ) + "mm   Y: " + std::to_string( int( shared->calibrationOffsetMM.y ) ) + "mm   Z: " + std::to_string( int( shared->calibrationOffsetMM.z ) )
		+ "mm   |R|: " + std::to_string( int( cv::norm( shared->calibrationOffsetMM ) ) ) + "mm";
	std::string line3 = "Calibration values saved! Touch screen again to close calibration.";
	cv::putText( matCalibration, line0, cv::Point( 10, 30 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matCalibration, line1, cv::Point( 10, 60 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matCalibration, line2, cv::Point( 10, 90 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matCalibration, line3, cv::Point( 10, 120 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colRedMd, 2 );

	// Show updated image
	cv::imshow( winCalibration, matCalibration );

	// Update and remove task
	calibrationPoints.clear();
	calibrationZPoints.clear();
}


void CalibrationClass::InitializeCalibration() {


	cv::resizeWindow( "Calibration Window", CONFIG_TOUCHSCREEN_WIDTH, CONFIG_TOUCHSCREEN_HEIGHT );
	// cv::setWindowProperty( "Calibration Window", cv::WindowPropertyFlags::WND_PROP_TOPMOST, 1.0 );
	cv::moveWindow( "Calibration Window", 3440, 0 );
	// cv::setWindowProperty( "Calibration Window", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );
}