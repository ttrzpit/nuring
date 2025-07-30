
#include "TasksClass.h"



TasksClass::TasksClass( SystemDataManager& ctx, TimingClass& timerHandle, LoggingClass& loggerHandle )
	: dataHandle( ctx )
	, shared( ctx.getData() )
	, Timer( timerHandle )
	, Logger( loggerHandle ) {
	// Stuff
}

/* 
 * 
 * 
 *  
 * 
 * 
 * 
 * 
 * 
 * 
 *  =========================================================================================
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

		// Disable logging
		shared->Logging.isRunning = false;

		// Start calibration
		CalibrationStart();

	} else {

		// Calibration complete, wait for touch to close
		if ( isFinishing ) {

			if ( shared->Touchscreen.isTouched ) {

				// Set marker
				shared->Target.activeID = 1;

				// isComplete			   = true;
				isFinishing			   = false;
				shared->Task.isRunning = false;
				shared->Task.state	   = taskEnum::IDLE;

				std::cout << "TASK COMPLETE!";
				cv::imshow( winTaskBackground, 0 );
			}

		} else {

			// Update
			CalibrationUpdate();
		}
	}
}

void TasksClass::CalibrationStart() {

	// Set marker
	shared->Target.activeID = 8;

	// Start calibration
	shared->Task.isRunning = true;
	isFinishing			   = false;
	std::cout << "Starting calibration!\n";

	// Clear calibration values
	shared->Calibration.calibratedOffetMM = cv::Point3i( 0, 0, 0 );

	// Initialize interface
	InitializeInterface( taskEnum::CALIBRATE );

	// Show screen
	cv::imshow( winTaskBackground, matTaskBackground );
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
	shared->Calibration.calibratedOffetMM.z = shared->Target.positionFilteredNewMM.z;

	// Update status string
	shared->Display.statusString = "TasksClass: Calibration complete!";

	// Update flags
	isFinishing = true;

	// Show screen
	cv::imshow( winTaskBackground, matTaskBackground );
}



/* 
 * 
 * 
 *  
 * 
 * 
 * 
 * 
 * 
 * 
 *  =========================================================================================
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


void TasksClass::Fitts() {

	// Initialize if task is not running
	if ( !shared->Task.isRunning ) {

		// Start fitts task
		FittsStart();

	}
	// If task is already running, check if finishing or just updating
	else {

		// Check if task finishing
		if ( isFinishing ) {

			isFinishing					 = false;
			shared->Task.isRunning		 = false;
			shared->Task.state			 = taskEnum::IDLE;
			shared->Display.statusString = "Fitts task complete!";

			// Finish task
			FittsFinish();

		}
		// Otherwise, just update
		else {

			// Update
			FittsUpdate();
		}
	}
}

void TasksClass::FittsStart() {

	// Initialize runtime variables
	shared->Task.isRunning		 = true;
	isFinishing					 = false;
	shared->Display.statusString = "Starting Fitts task...";

	// Generate random seed
	std::srand( time( NULL ) );

	// Initialize interface
	InitializeInterface( taskEnum::FITTS );

	// Show screen
	cv::imshow( winTaskBackground, matTaskBackground );

	// Create timestamp and start task timer
	shared->Logging.dataAndTime = Timer.GetFullDateAndTime( false );
	Timer.TaskTimerStart();

	// Initialize logging variables
	FittsLoggingStart();
}


/**
 * @brief Update fitts task
 * 
 */
void TasksClass::FittsUpdate() {

	// Add entry
	FittsLoggingUpdate();

	// Check if touchscreen touched
	if ( shared->Touchscreen.isTouched == 1 ) {
		FittsFinish();
	}
}



/**
 * @brief Finish fitts task
 * 
 */
void TasksClass::FittsFinish() {

	// Stop timer
	Timer.TaskTimerStop();

	// Get touchpoint position
	touchPosition  = cv::Point2i( shared->Touchscreen.positionTouched.x, shared->Touchscreen.positionTouched.y );
	targetPosition = cv::Point2i( shared->Task.targetPosition.x + ( matAruco01.cols / 2 ), shared->Task.targetPosition.y + ( matAruco01.rows / 2 ) );

	// Draw marker at touch point
	cv::circle( matTaskBackground, touchPosition, 10, CONFIG_colBluMd, -1 );

	// Draw lines from target to touch
	cv::line( matTaskBackground, targetPosition, cv::Point( targetPosition.x, touchPosition.y ), CONFIG_colRedLt, 2 );
	cv::line( matTaskBackground, cv::Point( targetPosition.x, touchPosition.y ), cv::Point( touchPosition.x, touchPosition.y ), CONFIG_colRedLt, 2 );
	cv::line( matTaskBackground, targetPosition, touchPosition, CONFIG_colRedMd, 2 );

	// Calculate errors
	errorPx = cv::Point2i( ( touchPosition.x - targetPosition.x ), ( targetPosition.y - touchPosition.y ) );
	errorMm = errorPx * PX2MM;

	// Add on-screen text
	std::string line0 = "Task completed!";
	std::string line1 = "Participant ID: " + std::to_string( shared->Task.userID );
	std::string line2 = "Task Time: " + std::to_string( shared->Task.elapsedTaskTime ).substr( 0, 5 ) + "s";
	std::string line3 = "Error[px] X|Y|R: " + std::to_string( errorPx.x ) + "px | " + std::to_string( errorPx.y ) + "px | " + shared->FormatDecimal( shared->GetNorm2D( errorPx ), 0, 1 ) + "px";
	std::string line4 = "Error[mm] X|Y|R: " + std::to_string( errorMm.x ) + "mm | " + std::to_string( errorMm.y ) + "mm | " + shared->FormatDecimal( shared->GetNorm2D( errorMm ), 0, 1 ) + "mm";
	cv::putText( matTaskBackground, line0, cv::Point( 10, 70 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matTaskBackground, line1, cv::Point( 10, 100 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matTaskBackground, line2, cv::Point( 10, 130 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matTaskBackground, line3, cv::Point( 10, 160 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matTaskBackground, line4, cv::Point( 10, 190 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );

	// Show updated task field
	cv::imshow( winTaskBackground, matTaskBackground );

	// Save data
	Logger.SavePng( matTaskBackground );
	Logger.SaveTxt();

	// Update flags
	shared->Task.isRunning = false;
	shared->Task.name	   = "";
	shared->Task.state	   = taskEnum::IDLE;
}



/**
 * @brief Generate position of target marker based on shared->Tasks.command
 * 
 */
void TasksClass::FittsGeneratePosition() {

	// Select generation type
	switch ( shared->Task.command ) {

		// Generate random marker position on X axis
		case 'x': {

			// Calculate marker boundaries
			minX = CONFIG_TOUCHSCREEN_EXCLUSION_ZONE;
			maxX = CONFIG_TOUCHSCREEN_WIDTH_PX - CONFIG_TOUCHSCREEN_EXCLUSION_ZONE - 72;

			// Calculate marker position
			shared->Task.targetPosition.x = minX + ( rand() % ( maxX - minX + 1 ) ) + ( matAruco01.rows / 2 );
			shared->Task.targetPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) + ( matAruco01.rows / 2 );

			// Return
			shared->Display.statusString = "TasksClass: Generating random X position...";
			break;
		}

		// Generate random marker position on Y axis
		case 'y': {

			// Calculate marker boundaries
			minY = 0 + 72;
			maxY = CONFIG_TOUCHSCREEN_HEIGHT_PX - CONFIG_TOUCHSCREEN_EXCLUSION_ZONE - 93;

			// Calculate marker position
			shared->Task.targetPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) + ( matAruco01.rows / 2 );
			shared->Task.targetPosition.y = minY + ( rand() % ( maxY - minY + 1 ) ) + ( matAruco01.rows / 2 );

			// Return
			shared->Display.statusString = "TasksClass: Generating random Y position...";
			break;
		}

		// Generate random marker position on XY plane
		case 'z': {

			// Calculate marker boundaries
			minX = 0 + CONFIG_TOUCHSCREEN_EXCLUSION_ZONE;
			maxX = CONFIG_TOUCHSCREEN_WIDTH_PX - CONFIG_TOUCHSCREEN_EXCLUSION_ZONE - 72;
			minY = 0 + 72;
			maxY = CONFIG_TOUCHSCREEN_HEIGHT_PX - CONFIG_TOUCHSCREEN_EXCLUSION_ZONE - 93;

			// Calculate marker position
			shared->Task.targetPosition.x = minX + ( rand() % ( maxX - minX + 1 ) ) - matAruco01.cols / 2;
			shared->Task.targetPosition.y = minY + ( rand() % ( maxY - minY + 1 ) ) - ( matAruco01.rows / 2 );

			// Return
			shared->Display.statusString = "TasksClass: Generating random XY position...";
			break;
		}

		// Generate marker moving at constant velocity
		case 'v': {

			shared->Display.statusString = "TasksClass: Generating constant velocity marker...";
			break;
		}

		// Generate marker at center
		case 'c': {

			// Dead center
			shared->Task.targetPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco01.rows / 2 );
			shared->Task.targetPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows );
			shared->Display.statusString  = "TasksClass: Generating marker at center...";
			// Return
			break;
		}

		// Default case for error
		default: {

			shared->Display.statusString = "TasksClass: Incorrect Fitts axis request!";
			break;
		}
	}

	// Copy target to generated position
	matAruco01.copyTo( matTaskBackground( cv::Rect( shared->Task.targetPosition.x, shared->Task.targetPosition.y, matAruco01.cols, matAruco01.rows ) ) );

	// Actual target to prevent occlusion
	targetPosition = cv::Point2i( shared->Task.targetPosition.x + ( matAruco01.cols / 2 ), shared->Task.targetPosition.y + ( matAruco01.rows / 2 ) );
	cv::circle( matTaskBackground, cv::Point2i( targetPosition.x, targetPosition.y + CONFIG_TARGET_OFFSET_Y_MM * MM2PX ), 5 * MM2PX, CONFIG_colGreMd, -1 );
}


/**
 * @brief Initialize and start logging
 * 
 */
void TasksClass::FittsLoggingStart() {

	// Customize header
	shared->Logging.header1 = "TouchDetected";
	shared->Logging.header2 = "OutgoingPacket";
	shared->Logging.header3 = "IncomingPacket";
	shared->Logging.header4 = "GainPropAb, GainPropAd, GainPropFl, GainPropEx";
	shared->Logging.header5 = "PropA, PropB, PropC";
	shared->Logging.header6 = "GainIntegAb, GainIntegAd, GainIntegFl, GainIntegEx";
	shared->Logging.header7 = "IntegA, IntegB, IntegC";
	shared->Logging.header8 = "GainDerivAb, GainDerivAd, GainDerivFl, GainDerivEx";
	shared->Logging.header9 = "DerivA, DerivB, DerivC";

	// Initialize and add initial entry
	Logger.Initialize();
	// Logger.AddEntry();
}


void TasksClass::FittsLoggingUpdate() {

	// Only update if task is running
	if ( shared->Task.isRunning && !shared->Target.isTargetReset ) {

		// Update logging values
		shared->Logging.variable1 = std::to_string( shared->Touchscreen.isTouched );
		shared->Logging.variable2 = shared->Serial.packetOut;
		shared->Logging.variable3 = shared->Serial.packetIn;
		shared->Logging.variable4
			= shared->FormatDecimal( shared->Controller.gainKp.abd, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.gainKp.add, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.gainKp.flx, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.gainKp.ext, 0, 2 );
		shared->Logging.variable5 = shared->FormatDecimal( shared->Controller.proportionalTerm.x, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.proportionalTerm.y, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.proportionalTerm.z, 0, 2 );
		shared->Logging.variable6
			= shared->FormatDecimal( shared->Controller.gainKi.abd, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.gainKi.add, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.gainKi.flx, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.gainKi.ext, 0, 2 );
		shared->Logging.variable7 = shared->FormatDecimal( shared->Controller.integralTerm.x, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.integralTerm.y, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.integralTerm.z, 0, 2 );
		shared->Logging.variable8
			= shared->FormatDecimal( shared->Controller.gainKd.abd, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.gainKd.add, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.gainKd.flx, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.gainKi.ext, 0, 2 );
		shared->Logging.variable9 = shared->FormatDecimal( shared->Controller.derivativeTerm.x, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.derivativeTerm.y, 0, 2 ) + "," + shared->FormatDecimal( shared->Controller.derivativeTerm.z, 0, 2 );

		// Save entry
		Logger.AddEntry();
	}
}


/*
 *
 *
 * 
 * 
 * 
 * 
 * 
 *  =========================================================================================
 *  ========================================================================================= 
 * 
 *   LL     IIIIII  MM   MM  IIIIII  TTTTTTT   SSSSS
 *   LL       II    MMM MMM    II      TT     SS
 *   LL       II    MM M MM    II      TT     SS
 *   LL       II    MM   MM    II      TT      SSSSS
 *   LL       II    MM   MM    II      TT          SS
 *   LL       II    MM   MM    II      TT          SS
 *   LLLLL  IIIIII  MM   MM  IIIIII    TT      SSSSS
 * 
 *  ========================================================================================= 
 *  ========================================================================================= 
 */

void TasksClass::Limits() {

	// If the task isn't running, start it
	if ( !shared->Task.isRunning ) {

		LimitsStart();
	} else {
		// Update
		LimitsUpdate();
	}
}



void TasksClass::LimitsStart() {

	// Turn off targets
	shared->Target.activeID = 99;

	// Set runtime flags
	shared->Task.isRunning		 = true;
	isFinishing					 = false;
	shared->Task.name			 = "LIMITS";
	shared->Display.statusString = "Starting limits task...";
}



void TasksClass::LimitsUpdate() {

	// // Send limit value to encoders
	// shared->Controller.commandedPercentageABC = shared->Amplifier.commandedLimits;
}



void TasksClass::LimitsFinish() {
	shared->Task.isRunning = false;
	shared->Task.name	   = "";
	shared->Task.state	   = taskEnum::IDLE;
}



/* 
 *
 * 
 *
 *  
 * 
 * 
 *  =========================================================================================
 *  ========================================================================================= 
 * 
 *   GGGGGGG  EEEEEEE  NNN   NN  EEEEEEE  RRRRRR     AAA    LL
 *  GG        EE       NNNN  NN  EE       RR   RR   AA AA   LL
 *  GG        EE       NN NN NN  EE       RR   RR   AA AA   LL
 *  GG  GGG   EEEEE    NN  NNNN  EEEEE    RRRRRR   AAAAAAA  LL
 *  GG   GG   EE       NN   NNN  EE       RR  RR   AA   AA  LL
 *  GG   GG   EE       NN   NNN  EE       RR   RR  AA   AA  LL
 *   GGGGGGG  EEEEEEE  NN   NNN  EEEEEEE  RR   RR  AA   AA  LLLLLLL
 * 
 *  ========================================================================================= 
 *  ========================================================================================= 
 */



void TasksClass::InitializeInterface( taskEnum task ) {

	// Configure interface
	cv::namedWindow( winTaskBackground, cv::WINDOW_FULLSCREEN );
	cv::setWindowProperty( winTaskBackground, cv::WindowPropertyFlags::WND_PROP_TOPMOST, 1.0 );
	cv::moveWindow( winTaskBackground, 3440, 0 );
	cv::setWindowProperty( winTaskBackground, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );

	// Copy marker to mat
	matTaskBackground = CONFIG_colWhite;

	switch ( task ) {

		case taskEnum::CALIBRATE: {

			// Copy tag
			matAruco08.copyTo( matTaskBackground( cv::Rect( posX, posY, matAruco08.cols, matAruco08.rows ) ) );

			// Add instructional text
			std::string line0 = "Please touch the screen while keeping the center of the camera aligned with the target marker below. ";
			cv::putText( matTaskBackground, line0, cv::Point( 10, 40 ), cv::FONT_HERSHEY_SIMPLEX, 1.2, CONFIG_colBlack, 2 );

			break;
		}


		case taskEnum::FITTS: {

			// Add instructional text
			std::string line0 = "Fitts task running...";
			cv::putText( matTaskBackground, line0, cv::Point( 10, 40 ), cv::FONT_HERSHEY_SIMPLEX, 1.2, CONFIG_colBlack, 2 );

			// Calculate marker position based on selection
			shared->Target.isTargetFound = false;

			// Generate position
			FittsGeneratePosition();

			break;
		}

		default: {

			shared->Display.statusString = "TasksClass: Error in InterfaceInitialize!";
			break;
		}
	}
}