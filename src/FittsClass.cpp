// Call to class header
#include "FittsClass.h"

// System data manager
#include "SystemDataManager.h"

// Constructor
FittsClass::FittsClass( SystemDataManager& ctx, TimingClass& timerHandle, LoggingClass& loggerHandle )
	: dataHandle( ctx )
	, shared( ctx.getData() )
	, timer( timerHandle )
	, logger( loggerHandle ) {

	// Generate random seed
	std::srand( time( NULL ) );

	// Clear background frame
	matBackground = CONFIG_colWhite;
}

void FittsClass::Initialize() {
	// Configure interface
	cv::namedWindow( "Fitts Testing Interface", cv::WINDOW_FULLSCREEN );
	cv::setWindowProperty( "Fitts Testing Interface", cv::WindowPropertyFlags::WND_PROP_TOPMOST, 1.0 );
	cv::moveWindow( "Fitts Testing Interface", 3440, 0 );
	cv::setWindowProperty( "Fitts Testing Interface", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );
}



/**
 * @brief Start the Fitts Law trial
 * @param axis Which axis to run test on ('x', 'y', 'z' (both) )
 */
void FittsClass::StartAngleTest() {

	// Update flag
	testComplete = false;

	int minTheta = -45;
	int maxTheta = 45;

	// Generate theta
	shared->angleDesired = minTheta + ( rand() % ( maxTheta - minTheta + 1 ) );

	// Output position for debugging
	std::cout << "Rand angle: " << shared->angleDesired << "\n";

	// Start timer
	timer.TaskTimerStart();
}

/** 
 * @brief Check for update commands
 */
void FittsClass::UpdateAngle() {



	// Check if fitts test is running
	if ( shared->Task.isRunning == true ) {

		// Check if touchscreen pressed
		if ( shared->Touchscreen.isTouched == 1 ) {
			EndAngleTest();
			shared->Task.isRunning = false;
		}
	}
}



/**
 * @brief End the current test
 */
void FittsClass::EndAngleTest() {

	// End the task timer
	timer.TaskTimerEnd();

	// Update console
	shared->Display.statusString = "FittsClass: Angle test complete.";


	// Update
	testComplete = true;

	// // Save image
	// if ( shared->FLAG_LOGGING_ENABLED ) {
	// 	std::string imageFilename = "/home/tom/Code/nuring/logging/" + shared->loggingFilename + ".png";
	// 	shared->displayString	  = "Saving file " + imageFilename;
	// 	cv::imwrite( imageFilename, matBackground );
	// 	std::cout << "FittsClass:  Image saved at " << imageFilename << "\n";
	// }

	// Update and remove task
	shared->Task.name	   = "";
	shared->Task.isRunning = false;
	// shared->Task.isComplete					 = true;
	shared->Logging.isLoggingActivelyRunning = false;
}



/**
 * @brief Start the Fitts Law trial
 * @param axis Which axis to run test on ('x', 'y', 'z' (both) )
 */
void FittsClass::StartTest( char axis ) {


	// Update flag
	testComplete			= false;
	shared->fittsActiveAxis = axis;

	// Update test number
	// shared->TASK_REP_NUMBER++;

	// Clear the screen
	matBackground = CONFIG_colWhite;


	// Generate random marker position based on desired axis
	if ( axis == 'x' ) {
		GenerateRandomX();
	} else if ( axis == 'y' ) {
		GenerateRandomY();
	} else if ( axis == 'v' ) {
		GenerateTestVelocity();
	} else if ( axis == 'f' ) {
		GenerateFixedXY();
	} else if ( axis == 't' ) {
		GenerateIncrementingXY();
	} else if ( axis == 'z' ) {
		GenerateRandomXY();
	} else {
		std::cout << "FittsClass:  Incorrect axis request.\n";
	}

	// Output position for debugging
	std::cout << "Rand: " << shared->fittsMarkerPosition.x << " , " << shared->fittsMarkerPosition.y << "\n";

	// Copy to target window
	matAruco01.copyTo( matBackground( cv::Rect( shared->fittsMarkerPosition.x, shared->fittsMarkerPosition.y, matAruco01.cols, matAruco01.rows ) ) );

	// Draw compensated marker
	if ( shared->calibrationComplete ) {
		cv::circle( matBackground, cv::Point2i( shared->fittsMarkerPosition.x + matAruco01.cols / 2, shared->fittsMarkerPosition.y + matAruco01.cols / 2 + ( 25 * MM2PX ) ), 5 * MM2PX, CONFIG_colGreMd, -1 );
	}

	// Show target field
	cv::imshow( "Fitts Testing Interface", matBackground );

	// Start timer
	timer.TaskTimerStart();
}



void FittsClass::GenerateRandomX() {

	// Calculate marker boundaries
	unsigned short minX = 0 + CONFIG_TOUCHSCREEN_EXCLUSION_ZONE;
	unsigned short maxX = CONFIG_TOUCHSCREEN_WIDTH_PX - CONFIG_TOUCHSCREEN_EXCLUSION_ZONE - 72;

	// Calculate marker position
	shared->fittsMarkerPosition.x = minX + ( rand() % ( maxX - minX + 1 ) ) + ( matAruco01.rows / 2 );
	shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) + ( matAruco01.rows / 2 );

	// Generate vertical bar
	cv::line( matBackground, cv::Point2i( shared->fittsMarkerPosition.x + ( matAruco01.rows / 2 ), 0 ), cv::Point2i( shared->fittsMarkerPosition.x + ( matAruco01.rows / 2 ), CONFIG_TOUCHSCREEN_HEIGHT_PX ), CONFIG_colGreLt, matAruco01.cols );
}



void FittsClass::GenerateRandomY() {

	// Calculate marker boundaries
	unsigned short minY = 0 + 72;
	unsigned short maxY = CONFIG_TOUCHSCREEN_HEIGHT_PX - CONFIG_TOUCHSCREEN_EXCLUSION_ZONE - 93;

	// Calculate marker position
	shared->fittsMarkerPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) + ( matAruco01.rows / 2 );
	shared->fittsMarkerPosition.y = minY + ( rand() % ( maxY - minY + 1 ) ) + ( matAruco01.rows / 2 );

	// Generate vertical bar
	cv::line( matBackground, cv::Point2i( 0, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH_PX, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), CONFIG_colGreLt, matAruco01.cols );
}


void FittsClass::GenerateRandomXY() {

	shared->Telemetry.isTargetFound = false;
	// Calculate marker boundaries
	unsigned short minX = 0 + CONFIG_TOUCHSCREEN_EXCLUSION_ZONE;
	unsigned short maxX = CONFIG_TOUCHSCREEN_WIDTH_PX - CONFIG_TOUCHSCREEN_EXCLUSION_ZONE - 72;
	unsigned short minY = 0 + 72;
	unsigned short maxY = CONFIG_TOUCHSCREEN_HEIGHT_PX - CONFIG_TOUCHSCREEN_EXCLUSION_ZONE - 93;


	shared->fittsMarkerPosition.x = minX + ( rand() % ( maxX - minX + 1 ) ) - matAruco01.cols / 2;
	shared->fittsMarkerPosition.y = minY + ( rand() % ( maxY - minY + 1 ) ) - ( matAruco01.rows / 2 );
}


void FittsClass::GenerateFixedXY() {

	// Calculate marker boundaries
	unsigned short minX = 0 + CONFIG_TOUCHSCREEN_EXCLUSION_ZONE;
	unsigned short maxX = CONFIG_TOUCHSCREEN_WIDTH_PX - CONFIG_TOUCHSCREEN_EXCLUSION_ZONE - 72;
	unsigned short minY = 0 + 72;
	unsigned short maxY = CONFIG_TOUCHSCREEN_HEIGHT_PX - CONFIG_TOUCHSCREEN_EXCLUSION_ZONE - 93;

	shared->fittsMarkerPosition = shared->Telemetry.screenPositionPX;


	// shared->fittsMarkerPosition.x = minX + ( rand() % ( maxX - minX + 1 ) ) - matAruco01.cols / 2;
	// shared->fittsMarkerPosition.y = minY + ( rand() % ( maxY - minY + 1 ) ) - ( matAruco01.rows / 2 );
}



void FittsClass::GenerateIncrementingXY() {
	std::cout << "Rep: " << shared->Task.repetitionNumber << "\n";

	if ( shared->Task.repetitionNumber == 1 ) {

		// Dead center
		shared->fittsMarkerPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco01.rows / 2 );
		shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows );

		// Generate vertical bar
		cv::line( matBackground, cv::Point2i( 0, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH_PX, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), CONFIG_colGreLt, matAruco01.cols );

	} else if ( shared->Task.repetitionNumber == 2 ) {

		// Dead center
		shared->fittsMarkerPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco01.rows / 2 ) + ( 240 * MM2PX );
		shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows );

		// Generate vertical bar
		cv::line( matBackground, cv::Point2i( 0, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH_PX, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), CONFIG_colGreLt, matAruco01.cols );

	} else if ( shared->Task.repetitionNumber == 3 ) {

		// Dead center
		shared->fittsMarkerPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco01.rows / 2 ) + ( 100 * MM2PX );
		shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows ) + ( 100 * MM2PX );

		// Generate vertical bar
		cv::line( matBackground, cv::Point2i( 0, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH_PX, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), CONFIG_colGreLt, matAruco01.cols );
	} else if ( shared->Task.repetitionNumber == 4 ) {

		// Dead center
		shared->fittsMarkerPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco01.rows / 2 );
		shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows ) + ( 100 * MM2PX );

		// Generate vertical bar
		cv::line( matBackground, cv::Point2i( 0, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH_PX, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), CONFIG_colGreLt, matAruco01.cols );
	} else if ( shared->Task.repetitionNumber == 5 ) {

		// Dead center
		shared->fittsMarkerPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco01.rows / 2 ) - ( 100 * MM2PX );
		shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows ) + ( 100 * MM2PX );

		// Generate vertical bar
		cv::line( matBackground, cv::Point2i( 0, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH_PX, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), CONFIG_colGreLt, matAruco01.cols );
	} else if ( shared->Task.repetitionNumber == 6 ) {

		// Dead center
		shared->fittsMarkerPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco01.rows / 2 ) - ( 240 * MM2PX );
		shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows );

		// Generate vertical bar
		cv::line( matBackground, cv::Point2i( 0, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH_PX, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), CONFIG_colGreLt, matAruco01.cols );
	} else if ( shared->Task.repetitionNumber == 7 ) {

		// Dead center
		shared->fittsMarkerPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco01.rows / 2 ) - ( 100 * MM2PX );
		shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows ) - ( 100 * MM2PX );

		// Generate vertical bar
		cv::line( matBackground, cv::Point2i( 0, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH_PX, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), CONFIG_colGreLt, matAruco01.cols );
	} else if ( shared->Task.repetitionNumber == 8 ) {

		// Dead center
		shared->fittsMarkerPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco01.rows / 2 );
		shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows ) - ( 100 * MM2PX );

		// Generate vertical bar
		cv::line( matBackground, cv::Point2i( 0, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH_PX, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), CONFIG_colGreLt, matAruco01.cols );
	} else if ( shared->Task.repetitionNumber == 9 ) {

		// Dead center
		shared->fittsMarkerPosition.x = ( CONFIG_TOUCHSCREEN_WIDTH_PX / 2 ) - ( matAruco01.rows / 2 ) + ( 100 * MM2PX );
		shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows ) - ( 100 * MM2PX );

		// Generate vertical bar
		cv::line( matBackground, cv::Point2i( 0, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), cv::Point2i( CONFIG_TOUCHSCREEN_WIDTH_PX, shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) ), CONFIG_colGreLt, matAruco01.cols );
	} else {

		shared->Task.repetitionNumber = 0;
	}
}


void FittsClass::GenerateTestVelocity() {

	fittsPosition.x += ( velocity * 0.01 * MM2PX );
	shared->fittsMarkerPosition.x = fittsPosition.x;
	shared->fittsMarkerPosition.y = ( CONFIG_TOUCHSCREEN_HEIGHT_PX / 2 ) - ( matAruco01.rows );
	// std::cout << shared->timingTimestamp << "," << fittsPosition.x * PX2MM << "," << shared->targetMarkerPosition3dRaw.x << "," << shared->targetMarkerPosition3dNew.x << "\n";
	std::cout << "t:" << shared->Timing.elapsedRunningTime << " \t v:" << shared->Telemetry.velocityFilteredNewMM.x << "\n";


	// Clear the screen
	matBackground = CONFIG_colWhite;

	// Copy to target window
	matAruco01.copyTo( matBackground( cv::Rect( shared->fittsMarkerPosition.x, shared->fittsMarkerPosition.y, matAruco01.cols, matAruco01.rows ) ) );

	// Show target field
	cv::imshow( "Fitts Testing Interface", matBackground );
}


void FittsClass::EndTest() {

	// End the task timer
	timer.TaskTimerEnd();

	// Update console
	shared->Display.statusString = "FittsClass: Test complete.";

	// Save touchpoint
	touchPosition = cv::Point2i( shared->Touchscreen.positionTouched.x, shared->Touchscreen.positionTouched.y );

	// Draw marker
	cv::circle( matBackground, touchPosition, 20, CONFIG_colRedMd, 2 );

	// Draw lines from target to touch
	cv::Point2i tagCenter = cv::Point2i( shared->fittsMarkerPosition.x + ( matAruco01.cols / 2 ), shared->fittsMarkerPosition.y + ( matAruco01.rows / 2 ) );
	cv::Point2i tagTouch  = cv::Point( shared->Touchscreen.positionTouched.x, shared->Touchscreen.positionTouched.y );
	cv::line( matBackground, tagCenter, cv::Point( tagCenter.x, tagTouch.y ), CONFIG_colRedLt, 2 );
	cv::line( matBackground, cv::Point( tagCenter.x, tagTouch.y ), cv::Point( tagTouch.x, tagTouch.y ), CONFIG_colRedLt, 2 );
	cv::line( matBackground, tagCenter, tagTouch, CONFIG_colRedMd, 2 );

	// Calculate errors
	shared->fittsErrorPx = cv::Point3i( ( tagTouch.x - tagCenter.x ), ( tagCenter.y - tagTouch.y ), CalculateNorm( tagCenter, tagTouch ) );
	shared->fittsErrorMm = shared->fittsErrorPx * PX2MM;

	// For debug
	// std::cout << "ErrPx: " << shared->fittsErrorPx.x << " , " << shared->fittsErrorPx.y << " , " << shared->fittsErrorPx.z << "\n";
	// std::cout << "ErrMm: " << shared->fittsErrorMm.x << " , " << shared->fittsErrorMm.y << " , " << shared->fittsErrorMm.z << "\n";

	// Touch detected at
	// Add on-screen text
	std::string line0 = "Task completed!";
	std::string line1 = "Participant ID: " + std::to_string( shared->Task.userID );
	std::string line2 = "Task Time: " + std::to_string( shared->fittsCompletionTime ).substr( 0, 5 ) + "s";
	std::string line3 = "Error[px] X|Y|R: " + std::to_string( shared->fittsErrorPx.x ) + "px | " + std::to_string( shared->fittsErrorPx.y ) + "px | " + std::to_string( shared->fittsErrorPx.z ) + "px";
	std::string line4 = "Error[mm] X|Y|R: " + std::to_string( shared->fittsErrorMm.x ) + "mm | " + std::to_string( shared->fittsErrorMm.y ) + "mm | " + std::to_string( shared->fittsErrorMm.z ) + "mm";
	cv::putText( matBackground, line0, cv::Point( 10, 30 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matBackground, line1, cv::Point( 10, 60 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matBackground, line2, cv::Point( 10, 90 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matBackground, line3, cv::Point( 10, 120 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );
	cv::putText( matBackground, line4, cv::Point( 10, 150 ), cv::FONT_HERSHEY_SIMPLEX, 0.8, CONFIG_colBlack, 2 );



	// Update
	testComplete = true;

	// Show target field
	cv::imshow( "Fitts Testing Interface", matBackground );


	// Save image
	if ( shared->Logging.isLoggingEnabled ) {
		std::string imageFilename	 = "/home/tom/Code/nuring/logging/" + shared->loggingFilename + ".png";
		shared->Display.statusString = "Saving file " + imageFilename;
		cv::imwrite( imageFilename, matBackground );
		std::cout << "FittsClass:  Image saved at " << imageFilename << "\n";
	}

	// Update and remove task
	shared->Task.name	   = "";
	shared->Task.isRunning = false;
	// shared->Task.isComplete					 = true;
	shared->Logging.isLoggingActivelyRunning = false;
}



/** 
 * @brief Check for update commands
 */
void FittsClass::Update() {



	// Check if fitts test is running
	if ( shared->Task.isRunning == true ) {

		// Check if velocity is active
		if ( shared->fittsActiveAxis == 'v' ) {

			float timestep = 0.01f;
			currentStep	   = static_cast<int>( shared->Timing.elapsedRunningTime / timestep );

			if ( currentStep != lastTriggerStep ) {
				lastTriggerStep = currentStep;
				GenerateTestVelocity();
			}
		}

		// Check if touchscreen pressed
		if ( shared->Touchscreen.isTouched == 1 ) {
			EndTest();
			shared->Task.isRunning = false;
		}
	}
}



/**
 * @brief Calculate norm between two 2D points
 */
unsigned short FittsClass::CalculateNorm( cv::Point2i pt1, cv::Point2i pt2 ) {

	unsigned short norm = sqrt( ( ( pt1.x - pt2.x ) * ( pt1.x - pt2.x ) ) + ( ( pt1.y - pt2.y ) * ( pt1.y - pt2.y ) ) );
	return norm;
}