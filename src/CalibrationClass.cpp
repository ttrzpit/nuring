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

void CalibrationClass::Close() {

	// Load window first time
	if ( shared->calibrationLoaded ) {
		cv::imshow( winCalibration, 0 );
	}
}



void CalibrationClass::Initialize() {

	// Load window first time
	if ( !shared->calibrationLoaded ) {

		// Configure interface
		cv::namedWindow( winCalibration, cv::WINDOW_AUTOSIZE );
		cv::moveWindow( winCalibration, 3440, 0 );
		shared->calibrationLoaded = true;
		shared->Task.repetitionNumber	  = 0;
	}

	// Clear window
	matCalibration = CONFIG_colWhite;

	// Draw aruco target
	matAruco.copyTo( matCalibration( cv::Rect( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ) - ( matAruco.cols / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) - ( matAruco.rows / 2 ), matAruco.cols, matAruco.rows ) ) );

	// Add instructional text
	std::string line0 = "Please touch the screen while keeping the center of the camera aligned with the dot below. ";
	std::string line1 = "Completed: " + std::to_string( shared->Task.repetitionNumber ) + " / 5";
	cv::putText( matCalibration, line0, cv::Point( 10, 40 ), cv::FONT_HERSHEY_SIMPLEX, 1.2, CONFIG_colBlack, 2 );
	cv::putText( matCalibration, line1, cv::Point( 10, 80 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );


	// Show calibration screen
	cv::imshow( winCalibration, matCalibration );
}



void CalibrationClass::Update() {

	// Check if touch detected
	if ( shared->touchDetected ) {

		calibrationPoints.push_back( shared->touchPosition - cv::Point3i( CONFIG_TOUCHSCREEN_WIDTH / 2, CONFIG_TOUCHSCREEN_HEIGHT / 2, 0 ) );
		calibrationZPoints.push_back( shared->Target.positionFilteredNewMM.z );

		// Debug output
		std::cout << "Rep: " << shared->Task.repetitionNumber << " X: " << shared->touchPosition.x << " Y: " << shared->touchPosition.y << " Z: " << shared->touchPosition.z << "\n";

		// Increment
		shared->Task.repetitionNumber++;

		// Reset touch flag
		shared->touchDetected = false;

		// Re-initialize
		Initialize();
	}
}


void CalibrationClass::End() {

	if ( !outputCreated ) {

		std::cout << shared->Task.repetitionNumber << "!!!/n";
		// Update console
		shared->displayString = "CalibrationClass: Calibration complete!";

		// Clear matc
		matCalibration = CONFIG_colWhite;

		// Copy to target window
		cv::rectangle( matCalibration, cv::Point2i( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ) - ( matAruco.cols / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) - ( matAruco.rows / 2 ) ),
					   cv::Point2i( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ) + ( matAruco.cols / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) + ( matAruco.rows / 2 ) ), CONFIG_colGreMd, 4 );
		matAruco.copyTo( matCalibration( cv::Rect( ( CONFIG_TOUCHSCREEN_WIDTH / 2 ) - ( matAruco.cols / 2 ), ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) - ( matAruco.rows / 2 ), matAruco.cols, matAruco.rows ) ) );

		// Only attempt if enough data points were collected
		if ( calibrationPoints.size() == 5 ) {

			// Iterate over points
			for ( int i = 0; i < calibrationPoints.size(); i++ ) {

				// Extract point
				cv::Point3i touchPoint = calibrationPoints[i];

				// Draw marker at touched point
				cv::circle( matCalibration, cv::Point2i( touchPoint.x, touchPoint.y ), 10, CONFIG_colBluLt, -1 );

				// Add to running average
				// shared->calibrationOffsetPX = shared->calibrationOffsetPX + calibrationPoints[i];
				runningAverage += cv::Point3f( touchPoint );
			}

			// Calc average
			cv::Point3f avg = runningAverage / static_cast<float>( calibrationPoints.size() );

			// Calculate average
			shared->calibrationOffsetPX	  = avg;
			shared->calibrationScreenPX	  = cv::Point2i( avg.x + CONFIG_TOUCHSCREEN_CENTER.x, avg.y + CONFIG_TOUCHSCREEN_CENTER.y );
			shared->calibrationOffsetMM	  = cv::Point3i( shared->calibrationOffsetPX.x * PX2MM, shared->calibrationOffsetPX.y * PX2MM, 0 );
			shared->calibrationOffsetMM.y = shared->calibrationOffsetMM.y - 25;
			shared->calibrationOffsetPX.y = shared->calibrationOffsetMM.y * MM2PX;
			shared->calibrationOffsetMM.z = int( std::accumulate( calibrationZPoints.begin(), calibrationZPoints.end(), 0 ) / 5.0f );
			shared->calibrationOffsetPX.z = shared->calibrationOffsetMM.z * MM2PX;

			// Draw lines to touch point
			cv::line( matCalibration, CONFIG_TOUCHSCREEN_CENTER, cv::Point2i( shared->calibrationScreenPX.x, shared->calibrationScreenPX.y ), CONFIG_colVioLt, 2 );
			cv::line( matCalibration, CONFIG_TOUCHSCREEN_CENTER, cv::Point2i( shared->calibrationScreenPX.x, ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), CONFIG_colRedMd, 2 );
			cv::line( matCalibration, cv::Point2i( shared->calibrationScreenPX.x, ( CONFIG_TOUCHSCREEN_HEIGHT / 2 ) ), cv::Point2i( shared->calibrationScreenPX.x, shared->calibrationOffsetPX.y ), CONFIG_colGreMd, 2 );

			// Draw circle at offset
			cv::circle( matCalibration, cv::Point2i( shared->calibrationScreenPX.x, shared->calibrationScreenPX.y ), 20, CONFIG_colBluMd, 2 );

			// Debug
			// std::cout << "Avg: " << std::to_string( shared->calibrationOffsetMM.x ) << " , " << std::to_string( shared->calibrationOffsetMM.y ) << std::to_string( shared->calibrationOffsetMM.z ) << "\n";

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

			if ( !shared->calibrationComplete ) {
				// Save image
				std::string imageFilename = "/home/tom/Code/nuring/logging/" + shared->loggingFilename + ".png";
				shared->displayString	  = "Saving file " + imageFilename;
				cv::imwrite( imageFilename, matCalibration );
				std::cout << "CalibrationClass:  Image saved at " << imageFilename << "\n";

				shared->calibrationComplete = true;
			}


			// Save data to logging variables
			shared->loggingVariable1 = std::to_string( shared->calibrationOffsetMM.x );
			shared->loggingVariable2 = std::to_string( shared->calibrationOffsetMM.y );
			shared->loggingVariable3 = std::to_string( shared->calibrationOffsetMM.z );



			// Update flag
			outputCreated = true;

			// Run again to wait for touch and close
			// Update();


			// shared->TASK_RUNNING = false;
			// shared->TASK_NAME	 = "";
		}
	}
}
