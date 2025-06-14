
#include "TasksClass.h"



TasksClass::TasksClass( SystemDataManager& ctx, TimingClass& timerHandle, LoggingClass& loggerHandle )
	: dataHandle( ctx )
	, shared( ctx.getData() )
	, timer( timerHandle )
	, logger( loggerHandle ) {
	// Stuff
}

/* =========================================================================================
 *  ========================================================================================= 
 * 
 *   CCCCCCC    AAAA    LL       IIIIII  BBBBBBB   RRRRRR     AAAA   TTTTTTTT  EEEEEEE
 *  CC         AA  AA   LL         II    BB   BBB  RR   RR   AA  AA     TT     EE
 *  CC         AA  AA   LL         II    BB   BBB  RR   RR   AA  AA     TT     EE
 *  CC        AAAAAAAA  LL         II    BBBBBBB   RRRRRR   AAAAAAAA    TT     EEEEE
 *  CC        AA    AA  LL         II    BB   BBB  RR  RR   AA    AA    TT     EE
 *  CC        AA    AA  LL         II    BB   BBB  RR   RR  AA    AA    TT     EE
 *   CCCCCCC  AA    AA  LLLLLLL  IIIIII  BBBBBBB   RR   RR  AA    AA    TT     EEEEEEE
 * 
 *  ========================================================================================= 
 *  ========================================================================================= 
 */



void TasksClass::Calibration() {

	// If the task isn't running yet, start it first
	if ( !shared->Task.isRunning ) {

		// Set marker
		shared->Telemetry.activeID = 8;

		// Start calibration
		shared->Task.isRunning = true;
		isFinishing			   = false;
		std::cout << "Starting calibration!\n";
		CalibrationStart();

	} else {

		// Calibration complete, wait for touch to close
		if ( isFinishing ) {

			if ( shared->Touchscreen.isTouched ) {

				// Set marker
				shared->Telemetry.activeID = 1;

				// isComplete			   = true;
				isFinishing			   = false;
				shared->Task.isRunning = false;
				shared->Task.state	   = taskEnum::IDLE;

				std::cout << "TASK COMPLETE!";
				cv::imshow( winCalibration, 0 );
			}

		} else {

			// Update
			CalibrationUpdate();
		}
	}
}

void TasksClass::CalibrationStart() {

	// Clear calibration values
	shared->Calibration.calibratedOffetMM = cv::Point3i( 0, 0, 0 );

	// Configure interface
	cv::namedWindow( winCalibration, cv::WINDOW_FULLSCREEN );
	cv::setWindowProperty( winCalibration, cv::WindowPropertyFlags::WND_PROP_TOPMOST, 1.0 );
	cv::moveWindow( winCalibration, 3440, 0 );
	cv::setWindowProperty( winCalibration, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );

	// Copy marker to mat
	matTaskBackground = CONFIG_colWhite;
	matAruco08.copyTo( matTaskBackground( cv::Rect( posX, posY, matAruco08.cols, matAruco08.rows ) ) );

	// Add instructional text
	std::string line0 = "Please touch the screen while keeping the center of the camera aligned with the dot below. ";
	cv::putText( matTaskBackground, line0, cv::Point( 10, 40 ), cv::FONT_HERSHEY_SIMPLEX, 1.2, CONFIG_colBlack, 2 );

	// Show screen
	cv::imshow( winCalibration, matTaskBackground );
}



void TasksClass::CalibrationUpdate() {

	// Check if touch detected
	if ( shared->Touchscreen.isTouched ) {

		// Update screen offset with position
		shared->Calibration.calibratedOffetPX = cv::Point3i( shared->Touchscreen.positionTouched );

		// Update flags
		shared->Calibration.isCalibrated = true;
		shared->Touchscreen.isTouched	 = false;

		// End calibration
		CalibrationFinish();
	}
}


void TasksClass::CalibrationFinish() {

	// Calculate screen touchpoint
	cv::Point2i touchPointPX = cv::Point2i( shared->Calibration.calibratedOffetPX.x, shared->Calibration.calibratedOffetPX.y );

	// Draw elements
	cv::line( matTaskBackground, CONFIG_TOUCHSCREEN_CENTER, cv::Point2i( CONFIG_TOUCHSCREEN_CENTER.x, touchPointPX.y ), CONFIG_colMagMd, 2 );						 // X
	cv::line( matTaskBackground, cv::Point2i( CONFIG_TOUCHSCREEN_CENTER.x, touchPointPX.y ), cv::Point2i( touchPointPX.x, touchPointPX.y ), CONFIG_colMagMd, 2 );	 // Y
	cv::line( matTaskBackground, CONFIG_TOUCHSCREEN_CENTER, touchPointPX, CONFIG_colMagMd, 2 );																		 // Mag
	cv::circle( matTaskBackground, touchPointPX, 20, CONFIG_colMagMd, -1 );

	// Store offset values
	shared->Calibration.calibratedOffetMM.x = ( CONFIG_TOUCHSCREEN_CENTER.x - touchPointPX.x ) * PX2MM;
	shared->Calibration.calibratedOffetMM.y = ( CONFIG_TOUCHSCREEN_CENTER.y - touchPointPX.y ) * PX2MM;
	shared->Calibration.calibratedOffetMM.z = shared->Telemetry.positionFilteredNewMM.z;

	// Update status string
	shared->Display.statusString = "TasksClass: Calibration complete!";

	// Update flags
	isFinishing = true;

	// Show screen
	cv::imshow( winCalibration, matTaskBackground );
}



/* =========================================================================================
 *  ========================================================================================= 
 * 
 *   FFFFFFF  IIIIII  TTTTTTT  TTTTTTT   SSSSS
 *   FF         II      TT        TT    SS
 *   FF         II      TT        TT    SS
 *   FFFFF      II      TT        TT     SSSSS
 *   FF         II      TT        TT         SS
 *   FF         II      TT        TT         SS
 *   FF       IIIIII    TT        TT     SSSSS
 * 
 *  ========================================================================================= 
 *  ========================================================================================= 
 */


void Fitts() { }

void FittsStart() { }