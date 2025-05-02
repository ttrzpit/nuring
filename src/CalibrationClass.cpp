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
void CalibrationClass::CalibrateDevice() {

	// Clear window
	matCalibration = CONFIG_colWhite;

	// Configure interface
	cv::namedWindow( "Calibration Window", cv::WINDOW_FULLSCREEN );
	cv::setWindowProperty( "Calibration Window", cv::WindowPropertyFlags::WND_PROP_TOPMOST, 1.0 );
	cv::moveWindow( "Calibration Window", 3440, 0 );
	cv::setWindowProperty( "Calibration Window", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );

	// Draw aruco target
	matAruco.copyTo( matCalibration( cv::Rect( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ) - ( matAruco.cols / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) - ( matAruco.rows / 2 ), matAruco.cols, matAruco.rows ) ) );

	// Draw target on touchscreen
	// cv::circle( matCalibration, cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH / 2, CONFIG_TOUCHSCREEN_HEIGHT / 2 ), 2.5 * MM2PX, CONFIG_colGreDk, -1 );

	// Add instructional text
	std::string line0 = "Please touch the screen while keeping the center of the camera aligned with the dot below. ";
	std::string line1 = "Completed: " + std::to_string( shared->TASK_NUMBER ) + " / 5";
	cv::putText( matCalibration, line0, cv::Point( 10, 40 ), cv::FONT_HERSHEY_SIMPLEX, 1.2, CONFIG_colBlack, 2 );
	cv::putText( matCalibration, line1, cv::Point( 10, 80 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );


	// Show calibration screen
	cv::imshow( "Calibration Window", matCalibration );
}


/**
 * @brief Calibration update
 */
void CalibrationClass::Update() {



	// Check if calibration complete
	if ( shared->TASK_RUNNING == true ) {

		// Check if touchscreen pressed
		if ( shared->touchPosition.z == 1 ) {
			shared->TASK_NUMBER++;
			calibrationPoints.push_back( shared->touchPosition - cv::Point3i( CONFIG_TOUCHSCREEN_WIDTH / 2, CONFIG_TOUCHSCREEN_HEIGHT / 2, 0 ) );

			// Check if calibration complete
			if ( shared->TASK_NUMBER <= 4 ) {
				shared->touchPosition.z = 0;
				CalibrateDevice();
			} else if ( shared->TASK_NUMBER == 5 ) {
				shared->touchPosition.z		= 0;
				shared->calibrationComplete = true;
				FinishCalibration();
			} else {
				cv::destroyWindow( "Calibration Window" );
				shared->TASK_RUNNING = false;
				shared->TASK_NAME	 = "";
				// shared->TASK_RUNNING = false;
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
		shared->calibrationOffset = shared->calibrationOffset + calibrationPoints[i];

		// Debug
		std::cout << i << ": " << calibrationPoints[i].x << " , " << calibrationPoints[i].y << "\n";
	}

	// Calculate average
	shared->calibrationOffset = ( shared->calibrationOffset / 5.0 );



	// Draw lines to touch point
	cv::line( matCalibration, cv::Point2i( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), cv::Point2i( shared->calibrationOffset.x + ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), shared->calibrationOffset.y + ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), CONFIG_colVioLt, 2 );
	cv::line( matCalibration, cv::Point2i( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), cv::Point2i( shared->calibrationOffset.x + ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), CONFIG_colRedMd, 2 );
	cv::line( matCalibration, cv::Point2i( shared->calibrationOffset.x + ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), cv::Point2i( shared->calibrationOffset.x + ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), shared->calibrationOffset.y + ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ),
			  CONFIG_colGreMd, 2 );

	// Draw circle at offset
	cv::circle( matCalibration, cv::Point2i( shared->calibrationOffset.x + ( CONFIG_TOUCHSCREEN_WIDTH / 2 ), shared->calibrationOffset.y + ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), 20, CONFIG_colBluMd, 2 );

	std::cout << "Avg: " << std::to_string( shared->calibrationOffset.x ) << " , " << std::to_string( shared->calibrationOffset.y ) << "\n";

	// Completion text
	std::string line0 = "Calibration complete!";
	std::string line1 = "Averaged Error[px] X|Y|R: " + std::to_string( shared->calibrationOffset.x ) + "px | " + std::to_string( shared->calibrationOffset.y ) + "px | " + std::to_string( int( cv::norm( shared->calibrationOffset ) ) ) + " px";
	std::string line2 = "Averaged Error[mm] X|Y|R: " + std::to_string( int( shared->calibrationOffset.x * PX2MM ) ) + "mm | " + std::to_string( int( shared->calibrationOffset.y * PX2MM ) ) + "mm | " + std::to_string( int( cv::norm( shared->calibrationOffset ) * PX2MM ) ) + "mm";
	std::string line3 = "Calibration values saved! Touch screen again to close calibration.";
	cv::putText( matCalibration, line0, cv::Point( 10, 30 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matCalibration, line1, cv::Point( 10, 60 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matCalibration, line2, cv::Point( 10, 90 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matCalibration, line3, cv::Point( 10, 120 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colRedMd, 2 );

	// Show updated image
	cv::imshow( "Calibration Window", matCalibration );

	// Update and remove task
	calibrationPoints.clear();
}