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

	// Configure interface
	cv::namedWindow( "Fitts Testing Interface", cv::WINDOW_FULLSCREEN );
	cv::setWindowProperty( "Fitts Testing Interface", cv::WindowPropertyFlags::WND_PROP_TOPMOST, 1.0 );
	cv::moveWindow( "Fitts Testing Interface", 3440, 0 );
	cv::setWindowProperty( "Fitts Testing Interface", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN );
}



/**
 * @brief Start the Fitts Law trial
 */
void FittsClass::StartTest() {

	// Update flag
	testComplete = false;

	// Update test number
	testNumber++;

	// Clear the screen
	matBackground = CONFIG_colWhite;

	// Calculate marker boundaries
	unsigned short minX = 0 + CONFIG_FITTS_SCREEN_EXCLUSION_ZONE;
	unsigned short maxX = CONFIG_FITTS_SCREEN_WIDTH - CONFIG_FITTS_SCREEN_EXCLUSION_ZONE - 72;
	unsigned short minY = 0 + 72;
	unsigned short maxY = CONFIG_FITTS_SCREEN_HEIGHT - CONFIG_FITTS_SCREEN_EXCLUSION_ZONE - 93;

	// Generate random marker position
	markerPosition.x = minX + ( rand() % ( maxX - minX + 1 ) );
	markerPosition.y = minY + ( rand() % ( maxY - minY + 1 ) );

	// Output position for debugging
	std::cout << "Rand: " << markerPosition.x << " , " << markerPosition.y << "\n";

	// Copy to target window
	matAruco01.copyTo( matBackground( cv::Rect( markerPosition.x, markerPosition.y, matAruco01.cols, matAruco01.rows ) ) );

	// Show target field
	cv::imshow( "Fitts Testing Interface", matBackground );

	// Start timer
	timer.TaskTimerStart();
}



/**
 * @brief End the current test
 */
void FittsClass::EndTest() {

	// End the task timer
	timer.TaskTimerEnd();

	// Update console
	shared->displayString = "FittsClass: Test complete.";

	// Save touchpoint
	touchPosition = cv::Point2i( shared->touchPosition.x, shared->touchPosition.y );

	// Draw marker
	cv::circle( matBackground, touchPosition, 20, CONFIG_colRedMd, 2 );

	// Draw lines from target to touch
	cv::Point2i tagCenter = cv::Point2i( markerPosition.x + ( matAruco01.cols / 2 ), markerPosition.y + ( matAruco01.rows / 2 ) );
	cv::Point2i tagTouch  = cv::Point( shared->touchPosition.x, shared->touchPosition.y );
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
	std::string line1 = "Participant ID: " + std::to_string( shared->TASK_USER_ID );
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
	std::string imageFilename = "/home/tom/Code/nuring/logging/" + shared->TASK_NAME + "_" + std::to_string( shared->TASK_USER_ID ) + "_" + std::to_string( testNumber ) + ".png";
	shared->displayString	  = "Saving file " + imageFilename;
	cv::imwrite( imageFilename, matBackground );
	std::cout << "FittsClass:  Image saved at " << imageFilename << "\n";
}



/** 
 * @brief Check for update commands
 */
void FittsClass::Update() {

	// Check commands
	switch ( shared->TASK_COMMAND ) {

	// Randomize
	case 'r':
		shared->arucoActiveID = 1;
		testComplete		  = false;
		StartTest();
		shared->displayString = "FittsClass: Randomizing tag position...";
		shared->TASK_COMMAND  = 0;

		break;
	case 'c':
		if ( !testComplete ) {
			EndTest();
			shared->TASK_COMMAND = 0;
		}
		break;
	default:
		//nothing
		break;
	}

	// Log data
	if ( !testComplete ) {
		// Save entry
		logger.AddEntry();
	} else if ( testComplete ) {
		// Save data
	}
}



/**
 * @brief Calculate norm between two 2D points
 */
unsigned short FittsClass::CalculateNorm( cv::Point2i pt1, cv::Point2i pt2 ) {

	unsigned short norm = sqrt( ( ( pt1.x - pt2.x ) * ( pt1.x - pt2.x ) ) + ( ( pt1.y - pt2.y ) * ( pt1.y - pt2.y ) ) );
	return norm;
}