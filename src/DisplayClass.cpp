// Call to class header
#include "DisplayClass.h"

// System data manager
#include "SystemDataManager.h"


// For decimal formatting
#include <iomanip>

/**
 * @brief DisplayClass constructor
 */
DisplayClass::DisplayClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {

	cv::namedWindow( winInterface, cv::WINDOW_AUTOSIZE );
	cv::moveWindow( winInterface, 3440 - CONFIG_DIS_WIDTH - 2, 0 );

	// Set font based on chosen resolution
	if ( CONFIG_TYPE == "LowResolution" ) {
		fontHeader	   = 0.65f;
		fontBody	   = 0.55f;
		key_fontHeader = 0.65f;
		key_fontBody   = 0.55f;
	} else if ( CONFIG_TYPE == "HighResolution" ) {
		fontHeader	   = 0.35f;
		fontBody	   = 0.3f;
		key_fontHeader = 0.45f;
		key_fontBody   = 0.4f;
	}
	std::cout << "DisplayClass: Display initialized.\n";
}



/**
 * @brief Updates the information on the display
 */
void DisplayClass::Update() {

	// Clear overlay frame
	// shared->matFrameOverlay = 0;

	// Copy video frame to overlay
	shared->matFrameUndistorted.copyTo( shared->matFrameOverlay( cv::Rect( 0, 0, shared->matFrameUndistorted.cols, shared->matFrameUndistorted.rows ) ) );


	/** Draw GUI elements **/

	// Draw detector crosshairs, changing colors based on if the target is present
	cv::circle( shared->matFrameOverlay, CONFIG_CAM_CENTER, CONFIG_DET_RADIUS, ( shared->FLAG_TARGET_FOUND ? CONFIG_colGreMd : CONFIG_colRedDk ), 1 );
	cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_HEIGHT ), ( shared->FLAG_TARGET_FOUND ? CONFIG_colGreMd : CONFIG_colRedDk ), 1 );
	cv::line( shared->matFrameOverlay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_WIDTH, CONFIG_CAM_PRINCIPAL_Y ), ( shared->FLAG_TARGET_FOUND ? CONFIG_colGreMd : CONFIG_colRedDk ), 1 );

	// Draw motor axis
	cv::line( shared->matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS35 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN35 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );
	cv::line( shared->matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS145 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN145 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );
	cv::line( shared->matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS270 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN270 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );


	// Draw relevant information if target found
	if ( shared->FLAG_TARGET_FOUND ) {

		//Draw vector to center of target
		cv::line( shared->matFrameOverlay, CONFIG_CAM_CENTER, CONFIG_CAM_CENTER - shared->targetScreenPosition, CONFIG_colCyaMd, 2 );

		// Draw border on target
		cv::polylines( shared->matFrameOverlay, shared->targetCorners, true, CONFIG_colCyaMd, 2 );

		// Draw 3D axes
		cv::drawFrameAxes( shared->matFrameUndistorted, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, shared->targetRotationVector, shared->targetTranslationVector, CONFIG_MARKER_WIDTH, 2 );

		// Draw velocity
		// cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X - shared->arucoTags[shared->arucoActiveID].errorVel3D.x * 1, CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].errorVel3D.y * -1 ),
		// 		  CONFIG_colMagLt, 2 );
	}

	// Draw green outline and crosshairs if marker found
	// if ( shared->FLAG_TAG_FOUND ) {

	// Draw circle
	// cv::circle( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_DET_RADIUS, CONFIG_colGreMd, 2 );
	// cv::rectangle( matDisplay, cv::Point2i( 1, 1 ), cv::Point2i( frame.cols - 2, frame.rows - 3 ), CONFIG_colGreMd, 3 );
	// cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_HEIGHT ), CONFIG_colGreMd, 1 );
	// cv::line( shared->matFrameOverlay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_WIDTH, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_colGreMd, 1 );

	// Draw vector to center of marker
	// if ( shared->arucoTags[shared->arucoActiveID].errorMagnitude2D > CONFIG_DET_RADIUS ) {
	// 	cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ),
	// 			  cv::Point2i( CONFIG_CAM_PRINCIPAL_X + shared->arucoTags[shared->arucoActiveID].errorMagnitude2D * cos( shared->arucoTags[shared->arucoActiveID].errorTheta ),
	// 						   CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].errorMagnitude2D * sin( shared->arucoTags[shared->arucoActiveID].errorTheta ) ),
	// 			  CONFIG_colCyaDk, 1 );
	// }
	// cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X - shared->arucoTags[shared->arucoActiveID].error2D.x, CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].error2D.y ), CONFIG_colCyaMd, 2 );

	// Draw rectangle if in calibration mode
	// if ( shared->TASK_NAME == "CALIBRATE" ) {
	// 	uint8_t side = 40;
	// 	cv::rectangle( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X - side, CONFIG_CAM_PRINCIPAL_Y - side ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X + side, CONFIG_CAM_PRINCIPAL_Y + side ), CONFIG_colBluMd, 2 );
	// }

	// if ( shared->calibrationComplete ) {
	// 	cv::Point2i offsetCenter = cv::Point2i( CONFIG_CAM_PRINCIPAL_X + shared->calibrationOffsetMM.x * MM2PX, CONFIG_CAM_PRINCIPAL_Y + shared->calibrationOffsetMM.y * MM2PX );
	// 	cv::circle( shared->matFrameOverlay, offsetCenter, 10, CONFIG_colGreMd, 2 );
	// 	cv::line( shared->matFrameOverlay, offsetCenter - cv::Point2i( 20, 0 ), offsetCenter + cv::Point2i( 20, 0 ), CONFIG_colGreLt, 2 );
	// 	cv::line( shared->matFrameOverlay, offsetCenter - cv::Point2i( 0, 20 ), offsetCenter + cv::Point2i( 0, 20 ), CONFIG_colGreLt, 2 );
	// }

	// Draw velocity on marker
	// cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X - shared->arucoTags[shared->arucoActiveID].error2D.x, CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].error2D.y ),
	// 		  cv::Point2i( CONFIG_CAM_PRINCIPAL_X - shared->arucoTags[shared->arucoActiveID].error2D.x + shared->arucoTags[shared->arucoActiveID].errorVel3D.x * 1,
	// 					   CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].error2D.y + shared->arucoTags[shared->arucoActiveID].errorVel3D.y * -1 ),
	// 		  CONFIG_colMagMd, 2 );

	// Draw velocity on center
	// cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X - shared->arucoTags[shared->arucoActiveID].errorVel3D.x * 1, CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].errorVel3D.y * -1 ),
	// 		  CONFIG_colMagLt, 2 );

	// Draw border on active tag
	// if ( shared->FLAG_TAG_FOUND && shared->arucoTags[shared->arucoActiveID].present ) {
	// 	cv::polylines( shared->matFrameOverlay, shared->arucoActiveCorners, true, CONFIG_colCyaMd, 2 );
	// } else {
	// 	cv::polylines( shared->matFrameOverlay, shared->arucoActiveCorners, true, CONFIG_colCyaDk, 2 );
	// }


	// //
	// }
	// else {
	// 	// cv::rectangle( matDisplay, cv::Point2i( 1, 1 ), cv::Point2i( frame.cols - 2, frame.rows - 3 ), CONFIG_colRedDk, 3 );
	// 	cv::circle( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_DET_RADIUS, CONFIG_colRedDk, 1 );
	// 	cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_HEIGHT ), CONFIG_colRedDk, 1 );
	// 	cv::line( shared->matFrameOverlay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_WIDTH, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_colRedDk, 1 );
	// }


	// // Draw motor axis
	// cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS35 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN35 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );
	// cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS145 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN145 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );
	// cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS270 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN270 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );


	// Add text
	AddText();

	// Show interface
	ShowInterface();

	// Check if other windows have been requested
	CheckOptions();
}



/**
 * Display the interface on the screen
 */
void DisplayClass::ShowInterface() {

	// Show image
	cv::imshow( winInterface, shared->matFrameOverlay );
	// cv::imshow( "Raw", shared->matFrameUndistorted );

	// Output confirmation
}



/**
 * Add text to the overlay
 */
void DisplayClass::AddText() {

	// Local common variables
	bool TARGET = shared->FLAG_TARGET_FOUND;


	// // Section border
	// DrawCell( "", "A1", 40, 7, 0, CONFIG_colWhite, CONFIG_colBlack, false );

	// // Active Marker Block
	// DrawCell( "Active Marker", "A1", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "1", "A2", 1, 1, fontBody, ( shared->arucoTags[1].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 1 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	// DrawCell( "2", "B2", 1, 1, fontBody, ( shared->arucoTags[2].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 2 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	// DrawCell( "3", "C2", 1, 1, fontBody, ( shared->arucoTags[3].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 3 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	// DrawCell( "4", "D2", 1, 1, fontBody, ( shared->arucoTags[4].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 4 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	// DrawCell( "5", "E2", 1, 1, fontBody, ( shared->arucoTags[5].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 5 ? CONFIG_colGreDk : CONFIG_colBlack ), true );

	// // Position [mm] Block
	// DrawCell( "Pos [mm]", "B3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "x", "A4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "y", "A5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "z", "A6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "R", "A7", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].error3D.x ) ), "B4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].error3D.y ) ), "B5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].error3D.z ) ), "B6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorMagnitude3D ) ), "B7", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// // Velocity [mm/s] Block
	// DrawCell( "Vel [mm/s]", "D3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorVel3D.x ) ), "D4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorVel3D.y ) ), "D5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorVel3D.z ) ), "D6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// // Controller
	// // Heading
	// DrawCell( "Controller", "F1", 18, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// // DrawCell( "F", "F2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// // DrawCell( "=", "G2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "Kp", "H2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "(", "I2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "ThD", "J2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "-", "K2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "ThM", "L2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( ")", "M2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "+", "N2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "Kd", "O2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "(", "P2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "ThVD", "Q2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "-", "R2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "ThVM", "S2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( ")", "T2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "=", "U2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "xxx", "V2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// // X Values
	// DrawCell( "Fx", "F3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "=", "G3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->controllerKp.x ) ), "H3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "(", "I3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->controllerThetaDesired.x * RAD2DEG ) ), "J3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "-", "K3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].thetaMeasured.x * RAD2DEG ) ), "L3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( ")", "M3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "+", "N3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->controllerKd.x ) ), "O3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "(", "P3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->controllerThetaDotDesired.x * RAD2DEG ) ), "Q3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "-", "R3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].thetaDotMeasured.x * RAD2DEG ) ), "S3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( ")", "T3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "=", "U3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->controllerForceXY.x * RAD2DEG ) ), "V3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// // Y Values
	// DrawCell( "Fy", "F4", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "=", "G4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->controllerKp.y ) ), "H4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "(", "I4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->controllerThetaDesired.y * RAD2DEG ) ), "J4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "-", "K4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].thetaMeasured.y * RAD2DEG ) ), "L4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( ")", "M4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "+", "N4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->controllerKd.y ) ), "O4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "(", "P4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->controllerThetaDesired.y * RAD2DEG ) ), "Q4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "-", "R4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].thetaDotMeasured.y * RAD2DEG ) ), "S4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( ")", "T4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "=", "U4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->controllerForceXY.y * RAD2DEG ) ), "V4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// // Motor
	// DrawCell( std::to_string( int( shared->controllerPercentage.x * 100 ) ) + "%", "F5", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "A", "G5", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "B", "F6", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );

	// DrawCell( "C", "F7", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );



	// DrawCell( "ThD", "H2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "ThM", "I2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );



	// DrawCell( "Fx", "F2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( ( shared->controllerKp.x ) ), "G2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "Fy", "F3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );

	// DrawCell( "Kp", "F2", 1, 2, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "x", "G2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "y", "G3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( ( shared->controllerKp.x ) ), "H2", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( ( shared->controllerKp.y ) ), "H3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "Kd", "J2", 1, 2, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "x", "K2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "y", "K3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( ( shared->controllerKd.x ) ), "L2", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( ( shared->controllerKd.y ) ), "L3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// Impedance
	//


	//
	// // Controller block
	// DrawCell( "Controller Settings", "K3", 8, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "K [N/mm]", "K4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "B [N*s/mm]", "K5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// // DrawCell( ( []( float value ) {
	// // 			  std::ostringstream out;
	// // 			  out << std::fixed << std::setprecision( 3 ) << value;
	// // 			  return out.str();
	// // 		  } )( shared->controllerK ),
	// // 		  "M4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// // DrawCell( ( []( float value ) {
	// // 			  std::ostringstream out;
	// // 			  out << std::fixed << std::setprecision( 3 ) << value;
	// // 			  return out.str();
	// // 		  } )( shared->controllerB ),
	// // 		  "M5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "Fx [N]", "O4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "Fy [N]", "O5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( ( []( float value ) {
	// 			  std::ostringstream out;
	// 			  out << std::fixed << std::setprecision( 3 ) << value;
	// 			  return out.str();
	// 		  } )( shared->controllerFx ),
	// 		  "Q4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( ( []( float value ) {
	// 			  std::ostringstream out;
	// 			  out << std::fixed << std::setprecision( 3 ) << value;
	// 			  return out.str();
	// 		  } )( shared->controllerFy ),
	// 		  "Q5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// // Mouse output block // AB1
	// DrawCell( "Mouse", "X1", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "x", "X2", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "y", "Y2", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( shared->touchPosition.x + 3440 ), "X3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( shared->touchPosition.y + 32 ), "Y3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( ( shared->touchDetected == 1 ? "Button 1" : "-" ), "X4", 2, 2, fontBody, CONFIG_colWhite, ( shared->touchDetected ? CONFIG_colGreDk : CONFIG_colBlack ), true );


	// // Calibration Blocks
	// DrawCell( "Calibration Status", "Z1", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( ( shared->calibrationComplete ? "Complete" : "Incomplete" ), "Z2", 3, 1, fontHeader, CONFIG_colWhite, ( shared->calibrationComplete ? CONFIG_colGreDk : CONFIG_colRedBk ), true );
	// DrawCell( " ", "Z3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "[px]", "AA3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "[mm]", "AB3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "x", "Z4", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "y", "Z5", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "z", "Z6", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "|R|", "Z7", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.x * MM2PX ) ), "AA4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.y * MM2PX ) ), "AA5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.z * MM2PX ) ), "AA6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( cv::norm( shared->calibrationOffsetMM ) * MM2PX ) ), "AA7", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.x ) ), "AB4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.y ) ), "AB5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.z ) ), "AB6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( cv::norm( shared->calibrationOffsetMM ) ) ), "AB7", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// // Amplifier / frequency blocks
	// DrawCell( "Amplifiers", "AC1", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( ( shared->FLAG_AMPLIFIERS_READY ? "Online" : "Offline" ), "AC2", 2, 2, fontBody, CONFIG_colWhite, ( shared->FLAG_AMPLIFIERS_READY ? CONFIG_colGreDk : CONFIG_colRedBk ), true );
	// DrawCell( "Frequency", "AC4", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( shared->timingFrequency ) + " Hz", "AC5", 2, 2, fontBody * 1.5, CONFIG_colWhite, CONFIG_colBlack, true );


	// // Serial blocks
	// DrawCell( "Serial0 (Outbound)", "AE1", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( ( shared->FLAG_SERIAL0_ENABLED ? "Online" : "Offline" ), "AH1", 3, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_SERIAL0_ENABLED ? CONFIG_colGreDk : CONFIG_colRedBk ), true );
	// DrawCell( shared->serialPacket0.substr( 0, shared->serialPacket0.size() - 1 ), "AE2", 6, 2, fontHeader * 1.6, CONFIG_colWhite, ( shared->serialPacket0 == "DX\n" ) ? CONFIG_colBlack : CONFIG_colGreDk, true );
	// DrawCell( "Serial1 (Inbound) ", "AE4", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( ( shared->FLAG_SERIAL1_ENABLED ? "Online" : "Offline" ), "AH4", 3, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_SERIAL0_ENABLED ? CONFIG_colGreDk : CONFIG_colRedBk ), true );
	// DrawCell( shared->serialPacket1.substr( 0, shared->serialPacket1.size() - 1 ), "AE5", 6, 2, fontHeader * 1.6, CONFIG_colWhite, ( shared->serialPacket0 == "DX\n" ) ? CONFIG_colBlack : CONFIG_colGreDk, true );


	// // Logging Block
	// DrawCell( "Logging", "AC7", 2, 1, fontHeader, CONFIG_colWhite, ( shared->FLAG_LOGGING_STARTED ) ? CONFIG_colGreDk : CONFIG_colGraBk, true );
	// // DrawCell( ( shared->FLAG_LOGGING_ON ? "Logging" : "Offline" ), "AM2", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// // DrawCell( "Filename", "AG6", 8, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( ( ( shared->FLAG_LOGGING_ENABLED && shared->FLAG_LOGGING_STARTED ) ? shared->loggingFilename : "DISABLED" ), "AE7", 6, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// // Motor Output Block
	// cv::Point2i center( 1440 + 80, 1222 + 66 );
	// DrawCell( "Motor Output", "AK1", 4, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// // cv::rectangle( shared->matFrameOverlay, cv::Rect2i( cv::Point2i( 1440, 1222 ), cv::Point2i( 1600, 1354 ) ), CONFIG_colVioMd, 1 );
	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS35 * 60, center.y - SIN35 * 60 ), CONFIG_colGraBk, 2 );
	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS145 * 60, center.y - SIN145 * 60 ), CONFIG_colGraBk, 2 );
	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * 60 ), CONFIG_colGraBk, 2 );

	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS35 * ( shared->controllerPercentage.x * 60.0f ), center.y - SIN35 * ( shared->controllerPercentage.x * 60.0f ) ), CONFIG_colRedMd, 2 );
	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS145 * ( shared->controllerPercentage.y * 60.0f ), center.y - SIN145 * ( shared->controllerPercentage.y * 60.0f ) ), CONFIG_colRedMd, 2 );
	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * ( shared->controllerPercentage.z * 60.0f ) ), CONFIG_colRedMd, 2 );

	// /// Lines connecting motor pairs
	// cv::line( shared->matFrameOverlay, cv::Point2i( center.x + COS35 * ( shared->controllerPercentage.x * 60.0f ), center.y - SIN35 * ( shared->controllerPercentage.x * 60.0f ) ), cv::Point2i( center.x, center.y - SIN270 * ( shared->controllerPercentage.z * 60.0f ) ), CONFIG_colRedMd, 1 );
	// cv::line( shared->matFrameOverlay, cv::Point2i( center.x + COS145 * ( shared->controllerPercentage.y * 60.0f ), center.y - SIN145 * ( shared->controllerPercentage.y * 60.0f ) ), cv::Point2i( center.x, center.y - SIN270 * ( shared->controllerPercentage.z * 60.0f ) ), CONFIG_colRedMd, 1 );
	// cv::line( shared->matFrameOverlay, cv::Point2i( center.x + COS35 * ( shared->controllerPercentage.x * 60.0f ), center.y - SIN35 * ( shared->controllerPercentage.x * 60.0f ) ),
	// 		  cv::Point2i( center.x + COS145 * ( shared->controllerPercentage.y * 60.0f ), center.y - SIN145 * ( shared->controllerPercentage.y * 60.0f ) ), CONFIG_colRedMd, 1 );

	// // Motor output circles
	// cv::circle( shared->matFrameOverlay, center, 60, ( shared->FLAG_CONTROLLER_ACTIVE ? CONFIG_colGreDk : CONFIG_colGraDk ), 1 );
	// cv::circle( shared->matFrameOverlay, center, ( shared->FLAG_CONTROLLER_ACTIVE ? 6 : 2 ), ( shared->FLAG_CONTROLLER_ACTIVE ? CONFIG_colGreDk : CONFIG_colGraDk ), -1 );

	// // // Trial blocks
	// // Participant Block
	// DrawCell( "Participant ID", "AC1", 4, 2, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( shared->TASK_USER_ID ), "AC3", 4, 2, fontHeader, CONFIG_colWhite, ( shared->TASK_USER_ID == 100 ? CONFIG_colRedDk : CONFIG_colGreDk ), true );

	// // Task command
	// DrawCell( "Task Command", "AG6", 4, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( shared->TASK_COMMAND ), "AG7", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// cv::Scalar backColor = shared->TASK_NUMBER == 0 ? CONFIG_colGraBk : CONFIG_colBluDk;

	// // Task block
	// DrawCell( "Current Task", "K1", 5, 2, fontHeader * 1.5, CONFIG_colWhite, backColor, true );
	// std::string taskName;
	// if ( shared->TASK_NUMBER == 1 ) {
	// 	taskName = "1: Discrimination Task";
	// } else if ( shared->TASK_NUMBER == 2 ) {
	// 	taskName = "2: Fitts-Law Task";
	// } else if ( shared->TASK_NUMBER == 3 ) {
	// 	taskName = "3: TBD";
	// } else {
	// 	taskName = "0: None";
	// }
	// DrawCell( taskName, "P1", 13, 2, fontBody * 1.5, CONFIG_colWhite, backColor, true );

	// // Task timer
	// DrawCell( "Elapsed Time", "K3", 5, 1, fontBody, CONFIG_colWhite, backColor, true );
	// DrawCell( std::to_string( shared->timingTimestamp ), "K4", 5, 1, fontBody, CONFIG_colWhite, backColor, true );

	// // Status Block
	// DrawCell( "System:", "K6", 2, 2, fontHeader * 1.5, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( shared->displayString, "M6", 11, 2, fontBody * 1.5, CONFIG_colWhite, CONFIG_colBlack, false );
}



/**
 * @brief Add text in a cell 
 * @param str Text to add [string]
 * @param cell0 Starting cell (e.g., "A1") [string]
 * @param width Number of columns across
 * @param height Number of columns down
 * @param sz Font size
 * @param textColor Color of body text
 * @param fillColor Color of background fill
 * @param centered Flag to center text
 */
void DisplayClass::DrawCell( std::string str, std::string cell0, short width, short height, float sz, cv::Scalar textColor, cv::Scalar fillColor, bool centered ) {

	// Check if using double letters (e.g., AA, AB)
	if ( std::isalpha( cell0[0] ) && !std::isalpha( cell0[1] ) ) {	  // Only one letter
		c0 = ( cell0[0] - 'A' ) * WIDTH;
		r0 = ( CONFIG_CAM_HEIGHT + 1 ) + ( ( std::stoi( cell0.substr( 1 ) ) - 1 ) * HEIGHT - 1 );
		rH = height * HEIGHT;
		cW = width * WIDTH;
	} else if ( std::isalpha( cell0[0] ) && std::isalpha( cell0[1] ) ) {	// Two letters)
		c0 = ( 26 + ( cell0[1] - 'A' ) ) * WIDTH;
		r0 = ( CONFIG_CAM_HEIGHT + 1 ) + ( ( std::stoi( cell0.substr( 2 ) ) - 1 ) * HEIGHT - 1 );
		rH = height * HEIGHT;
		cW = width * WIDTH;
	}


	// Draw cell frame
	cv::rectangle( shared->matFrameOverlay, cv::Rect( c0, r0, cW, rH ), fillColor, -1 );
	cv::rectangle( shared->matFrameOverlay, cv::Rect( c0, r0 - 1, cW + 1, rH + 1 ), CONFIG_colWhite, 1 );

	// Calculate text dimensions
	if ( sz >= fontHeader ) {
		textSize = cv::getTextSize( str, cv::FONT_HERSHEY_SIMPLEX, sz, 1, 0 );
	} else {
		textSize = cv::getTextSize( str, cv::FONT_HERSHEY_SIMPLEX, sz, 1, 0 );
	}

	// Calculate position for center
	if ( centered ) {
		textX = c0 + ( cW - textSize.width ) / 2;
		textY = r0 + ( rH + textSize.height ) / 2 - 1;	  //+ ( cH - textSize.height ) / 2;
	} else {
		textX = c0 + 10;
		textY = r0 + ( rH + textSize.height ) / 2 - 1;	  //+ ( cH - textSize.height ) / 2;
	}

	// Place text
	if ( sz >= fontHeader ) {
		cv::putText( shared->matFrameOverlay, str, cv::Point( textX, textY ), cv::FONT_HERSHEY_SIMPLEX, sz, textColor, 1 );
	} else {
		cv::putText( shared->matFrameOverlay, str, cv::Point( textX, textY ), cv::FONT_HERSHEY_SIMPLEX, sz, textColor, 1 );
	}
}



/**
 * @brief Add text in a cell 
 * @param str Text to add [string]
 * @param cell0 Starting cell (e.g., "A1") [string]
 * @param width Number of columns across
 * @param height Number of columns down
 * @param sz Font size
 * @param textColor Color of body text
 * @param fillColor Color of background fill
 * @param centered Flag to center text
 */
void DisplayClass::DrawKeyCell( std::string str, std::string cell0, short width, short height, float sz, cv::Scalar textColor, cv::Scalar fillColor, bool centered ) {

	// Check if using double letters (e.g., AA, AB)
	if ( std::isalpha( cell0[0] ) && !std::isalpha( cell0[1] ) ) {	  // Only one letter
		key_c0 = ( cell0[0] - 'A' ) * key_WIDTH;
		key_r0 = ( 1 ) + ( ( std::stoi( cell0.substr( 1 ) ) - 1 ) * key_HEIGHT - 1 );
		key_rH = height * key_HEIGHT;
		key_cW = width * key_WIDTH;
	} else if ( std::isalpha( cell0[0] ) && std::isalpha( cell0[1] ) ) {	// Two letters)
		key_c0 = ( 26 + ( cell0[1] - 'A' ) ) * key_WIDTH;
		key_r0 = ( 1 ) + ( ( std::stoi( cell0.substr( 2 ) ) - 1 ) * key_HEIGHT - 1 );
		key_rH = height * key_HEIGHT;
		key_cW = width * key_WIDTH;
	}


	// Draw cell frame
	cv::rectangle( matShortcuts, cv::Rect( key_c0, key_r0, key_cW, key_rH ), fillColor, -1 );
	cv::rectangle( matShortcuts, cv::Rect( key_c0, key_r0, key_cW + 1, key_rH + 1 ), CONFIG_colWhite, 1 );

	// Calculate text dimensions
	if ( sz >= key_fontHeader ) {
		key_textSize = cv::getTextSize( str, cv::FONT_HERSHEY_DUPLEX, sz, 1, 0 );
	} else {
		key_textSize = cv::getTextSize( str, cv::FONT_HERSHEY_SIMPLEX, sz, 1, 0 );
	}

	// Calculate position for center
	if ( centered ) {
		key_textX = key_c0 + ( key_cW - key_textSize.width ) / 2;
		key_textY = key_r0 + ( key_rH + key_textSize.height ) / 2 - 1;	  //+ ( cH - textSize.height ) / 2;
	} else {
		key_textX = key_c0 + 10;
		key_textY = key_r0 + ( key_rH + key_textSize.height ) / 2 - 1;	  //+ ( cH - textSize.height ) / 2;
	}

	// Place text
	if ( sz >= key_fontHeader ) {
		cv::putText( matShortcuts, str, cv::Point( key_textX, key_textY ), cv::FONT_HERSHEY_DUPLEX, sz, textColor, 1 );
	} else {
		cv::putText( matShortcuts, str, cv::Point( key_textX, key_textY ), cv::FONT_HERSHEY_SIMPLEX, sz, textColor, 1 );
	}

	// std::cout << key_c0 << "," << key_r0 << "," << key_rH << "," << key_cW << "\n";
}



void DisplayClass::ShowShortcuts() {

	// Create window
	cv::namedWindow( winShortcuts, cv::WINDOW_AUTOSIZE );
	cv::moveWindow( winShortcuts, 3440 - CONFIG_DIS_WIDTH - CONFIG_DIS_KEY_WIDTH - 4, 0 );

	// System
	DrawKeyCell( "System:", "A1", 6, 1, key_fontHeader * 1.3, CONFIG_colWhite, CONFIG_colGraDk, false );
	DrawKeyCell( "Shutdown software", "A2", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "ESC", "F2", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Enable amplifier (toggle)", "A3", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "e", "F3", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Send serial data (toggle)", "A4", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "s", "F4", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "E-Stop (kill amp & serial)", "A5", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "x", "F5", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Controller
	DrawKeyCell( "Controller:", "A6", 6, 1, key_fontHeader * 1.3, CONFIG_colWhite, CONFIG_colGraDk, false );
	DrawKeyCell( "Change active marker to 1-5", "A7", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "1-5", "F7", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Disable active markers", "A8", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "'", "F8", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Decrease | increase spring K", "A9", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "[  ]", "F9", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Decrease | increase damping B", "A10", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( ";   '", "F10", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Visualization Tools
	DrawKeyCell( "Visualization Tools:", "A11", 6, 1, key_fontHeader * 1.3, CONFIG_colWhite, CONFIG_colGraDk, false );
	DrawKeyCell( "Enable visualizer (toggle)", "A12", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "v", "F12", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Clear visualizer ", "A13", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "z", "F13", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// Tasks
	DrawKeyCell( "Tasks:", "A14", 6, 1, key_fontHeader * 1.3, CONFIG_colWhite, CONFIG_colGraDk, false );
	DrawKeyCell( "Calibrate Offset", "A15", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "c", "F15", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Target Test (Fitts)", "A16", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "f", "F16", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// Display window
	cv::imshow( "Keyboard Shortcuts", matShortcuts );
}



void DisplayClass::ShowVisualizer() {

	// Pre-calculate
	ProjectedCorners.clear();	 // <-- Important: clear it each frame!

	// Project corners
	for ( const auto& pt : cubeCorners ) {
		ProjectedCorners.push_back( ProjectIsometric( cv::Point3f( pt.z, pt.y, pt.x ) ) );	  // Swap x <-> z
	}

	// Create window
	cv::namedWindow( winVisualizer, cv::WINDOW_AUTOSIZE );
	cv::moveWindow( winVisualizer, 3440 - CONFIG_DIS_WIDTH - CONFIG_DIS_VIZ_WIDTH - 6 - CONFIG_DIS_KEY_WIDTH, 0 );
	cv::imshow( winVisualizer, matVisualizer );
}


void DisplayClass::UpdateVisualizer() {


	// // Check if trail should be cleared
	// if ( shared->vizClear ) {
	// 	trailPoints.clear();
	// 	shared->vizClear = false;
	// }

	// // Clear canvas
	// matVisualizer = CONFIG_colWhite;


	// // Draw cube
	// for ( const auto& edge : edges ) {
	// 	cv::line( matVisualizer, ProjectedCorners[edge.first], ProjectedCorners[edge.second], CONFIG_colGraLt, 1 );
	// }

	// uint8_t markerSize = 20;
	// // Draw target square
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, -markerSize, markerSize ) ), ProjectIsometric( cv::Point3i( 0, markerSize, markerSize ) ), CONFIG_colGraLt, 1 );
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, -markerSize, -markerSize ) ), ProjectIsometric( cv::Point3i( 0, markerSize, -markerSize ) ), CONFIG_colGraLt, 1 );
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, markerSize, -markerSize ) ), ProjectIsometric( cv::Point3i( 0, markerSize, markerSize ) ), CONFIG_colGraLt, 1 );
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, -markerSize, -markerSize ) ), ProjectIsometric( cv::Point3i( 0, -markerSize, markerSize ) ), CONFIG_colGraLt, 1 );

	// // X axis
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, -vizLimXY, 0 ) ), ProjectIsometric( cv::Point3i( 0, vizLimXY, 0 ) ), CONFIG_colRedLt, 1 );
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, vizLimXY, 0 ) ), ProjectIsometric( cv::Point3i( vizLimZ, vizLimXY, 0 ) ), CONFIG_colRedLt, 1 );
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, -vizLimXY, 0 ) ), ProjectIsometric( cv::Point3i( 1000, -vizLimXY, 0 ) ), CONFIG_colRedLt, 1 );

	// // Y axis
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, 0, -vizLimXY ) ), ProjectIsometric( cv::Point3i( 0, 0, vizLimXY ) ), CONFIG_colGreLt, 1 );
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, 0, vizLimXY ) ), ProjectIsometric( cv::Point3i( 1000, 0, vizLimXY ) ), CONFIG_colGreLt, 1 );

	// // Calculate camera + marker positions
	// cv::Point3i p3D		= cv::Point3i( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z - zOffset );
	// cv::Point3i p3DInv	= cv::Point3i( shared->arucoTags[shared->arucoActiveID].error3D.z - zOffset, shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.x * -1 );
	// int			ptSizeX = int( float( ( 1000.0 - ( p3D.x + vizLimXY ) ) / 1000.0 ) * 10.0 );
	// int			ptSizeY = int( float( ( 1000.0 - ( -1 * p3D.y + vizLimXY ) ) / 1000.0 ) * 10.0 );
	// int			ptSizeZ = int( float( ( 1000.0 - p3D.z ) / 1000.0 ) * 10.0 );

	// // Line from marker to target
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, 0, 0 ) ), ProjectIsometric( p3DInv ), CONFIG_colCyaMd, 2 );

	// // Moving local axis
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( p3DInv.x, p3D.y, vizLimXY ) ), ProjectIsometric( p3DInv ), CONFIG_colGreMd, 1 );
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( p3DInv.x, vizLimXY, p3DInv.z ) ), ProjectIsometric( p3DInv ), CONFIG_colRedMd, 1 );
	// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, p3DInv.y, p3DInv.z ) ), ProjectIsometric( p3DInv ), CONFIG_colBluMd, 1 );



	// // Create color gradient
	// float zClamped = std::clamp( shared->arucoTags[shared->arucoActiveID].error3D.z - zOffset, 0.0f, 1000.0f );
	// // float intensity = 128.0f * ( zClamped / 1000.0f );

	// // Add current point to trail
	// if ( trailCounter++ >= trailInterval ) {
	// 	trailPoints.push_back( p3DInv );
	// 	// trailColor.push_back( ( int )intensity );
	// 	trailCounter = 0;

	// 	// Limit trail length
	// 	if ( trailPoints.size() > trailLimit ) {
	// 		trailPoints.erase( trailPoints.begin() );
	// 		// trailColor.erase( trailColor.begin() );
	// 	}
	// }

	// // Draw trail points
	// for ( int i = 0; i < trailPoints.size(); i++ ) {

	// 	float ratio		= static_cast<float>( i ) / static_cast<float>( trailPoints.size() );
	// 	int	  intensity = static_cast<int>( 191 * ( 1.0f - ratio ) );

	// 	// Z trail
	// 	cv::circle( matVisualizer, ProjectIsometric( cv::Point3i( 0, trailPoints.at( i ).y, trailPoints.at( i ).z ) ), 2, cv::Scalar( 255, 64 + intensity, 64 + intensity ), -1 );

	// 	// Y trail
	// 	cv::circle( matVisualizer, ProjectIsometric( cv::Point3i( trailPoints.at( i ).x, trailPoints.at( i ).y, vizLimXY ) ), 2, cv::Scalar( 64 + intensity, 255, 64 + intensity ), -1 );

	// 	// X trail
	// 	cv::circle( matVisualizer, ProjectIsometric( cv::Point3f( trailPoints.at( i ).x, vizLimXY, trailPoints.at( i ).z ) ), 2, cv::Scalar( 64 + intensity, 64 + intensity, 255 ), -1 );
	// }

	// // Shadow on wall
	// cv::circle( matVisualizer, ProjectIsometric( cv::Point3i( p3DInv.x, p3D.y, vizLimXY ) ), ptSizeX, CONFIG_colGreLt, -1 );	   // Shadow on YZ axis (right)
	// cv::circle( matVisualizer, ProjectIsometric( cv::Point3i( 0, p3D.y, p3DInv.z ) ), ptSizeZ, CONFIG_colBluWt, -1 );			   // Shadow on XY plane (bottom)
	// cv::circle( matVisualizer, ProjectIsometric( cv::Point3i( p3DInv.x, vizLimXY, p3DInv.z ) ), ptSizeY, CONFIG_colRedLt, -1 );	   // Shadow on XZ (screen)

	// // // Draw marker (last to place over other elements)
	// cv::circle( matVisualizer, ProjectIsometric( p3DInv ), 10, CONFIG_colWhite, -1 );
	// cv::circle( matVisualizer, ProjectIsometric( p3DInv ), 10, CONFIG_colBlack, 1 );



	// // Show
	// cv::imshow( "3D Visualizer", matVisualizer );
}



// cv::Point2i DisplayClass::ProjectPoint( const cv::Point3f& p3d ) {
// 	if ( p3d.z == 0 )
// 		return cameraCenter;	// Avoid divide by zero
// 	int x2d = ( p3d.x * focalLength ) / ( float )p3d.z + cameraCenter.x;
// 	int y2d = ( p3d.y * focalLength ) / ( float )p3d.z + cameraCenter.y;
// 	return ( cv::Point2i( x2d, y2d ) );
// }


// Apply isometric camera transform and perspective
cv::Point2i DisplayClass::ProjectIsometric( const cv::Point3i& p3d ) {
	// Step 1: rotate the 3D point around X and Y axes


	// Rotation matrices
	float cx = cos( elevation ), sx = sin( elevation );
	float cy = cos( azimuth ), sy = sin( azimuth );

	// Rotate around Y axis (azimuth)
	float x1 = p3d.x * cy + p3d.z * sy;
	float y1 = p3d.y;
	float z1 = -p3d.x * sy + p3d.z * cy;

	// Then around X axis (elevation)
	float x2 = x1;
	float y2 = y1 * cx - z1 * sx;
	float z2 = y1 * sx + z1 * cx;

	// Step 2: perspective projection (same as before)
	float x_proj = ( x2 * focalLength ) / ( z2 + 2500.0f ) + ( ( CONFIG_DIS_VIZ_WIDTH / 2 ) - 300 );
	float y_proj = ( y2 * focalLength ) / ( z2 + 2500.0f ) + ( ( CONFIG_DIS_VIZ_HEIGHT / 2 ) - 150 );

	return cv::Point2f( x_proj, y_proj );
}


cv::Point2i DisplayClass::GetForwardDirectionFromPose( const cv::Vec3d rvec, const cv::Vec3d tvec, const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs, float axisLength ) {
	// Define 3D points: origin and Z-axis point
	std::vector<cv::Point3f> axisPoints;
	axisPoints.push_back( cv::Point3f( 0, 0, 0 ) );				// Origin
	axisPoints.push_back( cv::Point3f( 0, 0, axisLength ) );	// Forward along Z

	// Project to 2D
	std::vector<cv::Point2f> imagePoints;
	cv::projectPoints( axisPoints, rvec, tvec, cameraMatrix, distCoeffs, imagePoints );

	// Direction vector from origin to Z point
	cv::Point2i dir = imagePoints[1] - imagePoints[0];
	return dir;
}



// void DisplayClass::ShowAngle() {


// 	// Create window
// 	cv::namedWindow( winAngle, cv::WINDOW_AUTOSIZE );
// 	cv::moveWindow( winAngle, 3440 - CONFIG_DIS_WIDTH - CONFIG_DIS_VIZ_WIDTH - 6 - CONFIG_DIS_KEY_WIDTH, 0 );
// 	cv::imshow( winAngle, matAngles );
// }


// void DisplayClass::UpdateAngle() {

// 	matAngles = CONFIG_colWhite;

// 	uint16_t midX = CONFIG_DIS_ANGLE_WIDTH / 2;


// 	// Draw middle line
// 	cv::line( matAngles, cv::Point2i( midX, 10 ), cv::Point2i( midX, CONFIG_FIELD_LENGTH_PX ), CONFIG_colGraLt, 2 );

// 	// Draw "screen" at top
// 	cv::line( matAngles, cv::Point2i( midX - ( CONFIG_FIELD_WIDTH_PX / 2 ), 10 ), cv::Point2i( midX + ( CONFIG_FIELD_WIDTH_PX / 2 ), 10 ), CONFIG_colGreLt, 10 );

// 	// Draw test platform axis
// 	cv::line( matAngles, cv::Point2i( midX - ( CONFIG_FIELD_WIDTH_PX / 2 ), CONFIG_FIELD_LENGTH_PX ), cv::Point2i( midX + ( CONFIG_FIELD_WIDTH_PX / 2 ), CONFIG_FIELD_LENGTH_PX ), CONFIG_colBluWt, 10 );

// 	// Draw encoder elements
// 	int16_t endX = std::clamp( int( cos( ( shared->angleTheta * 3.0f + 90.0f ) * DEG2RAD ) * 1000 ), -int( CONFIG_DIS_ANGLE_WIDTH ), int( CONFIG_DIS_ANGLE_WIDTH ) );
// 	int16_t endY = std::clamp( int( sin( ( shared->angleTheta * 3.0f + 90.0f ) * DEG2RAD ) * 2000 ), 0, int( CONFIG_DIS_ANGLE_HEIGHT ) );
// 	cv::circle( matAngles, cv::Point2i( midX, CONFIG_FIELD_LENGTH_PX ), 10, CONFIG_colBluLt, -1 );
// 	cv::line( matAngles, cv::Point2i( midX, CONFIG_FIELD_LENGTH_PX ), cv::Point2i( midX + endX, CONFIG_FIELD_LENGTH_PX - endY ), CONFIG_colBluMd, 2 );


// 	// Draw marker
// 	int16_t markerX = ( ( shared->fittsMarkerPosition.x ) / 1662.0f ) * 1340.0f;
// 	// std::cout << markerX;
// 	cv::circle( matAngles, cv::Point2i( markerX + CONFIG_MARKER_WIDTH, 50 ), 10, CONFIG_colGreMd, -1 );

// 	// // Draw "encoder"
// 	// int16_t endX = std::clamp( int( cos( ( shared->angleTheta + 90.0f ) * DEG2RAD ) * 1000 ), -int( CONFIG_DIS_VIZ_WIDTH ), int( CONFIG_DIS_VIZ_WIDTH ) );
// 	// int16_t endY = std::clamp( int( sin( ( shared->angleTheta + 90.0f ) * DEG2RAD ) * 2000 ), 0, int( CONFIG_DIS_VIZ_HEIGHT ) );
// 	// cv::circle( matAngles, cv::Point2i( CONFIG_DIS_VIZ_WIDTH / 2, CONFIG_DIS_VIZ_HEIGHT - 100 ), 20, CONFIG_colBluMd, 2 );

// 	cv::imshow( winAngle, matAngles );
// }


void DisplayClass::CheckOptions() {

	// Launch viz if requested
	if ( shared->vizEnabled ) {

		// Check if window loaded
		if ( shared->vizLoaded ) {
			// Update
			UpdateVisualizer();
		} else {
			ShowVisualizer();
			shared->vizLoaded = true;
		}
	} else {

		// Check if window loaded
		if ( shared->vizLoaded ) {
			shared->vizLoaded = false;
			cv::destroyWindow( winVisualizer );
		}
	}

	// // Launch angle if requested
	// if ( shared->angleEnabled ) {

	// 	// Check if window loaded
	// 	if ( shared->angleLoaded ) {
	// 		UpdateAngle();
	// 	} else {
	// 		ShowAngle();
	// 		shared->angleLoaded = true;
	// 	}
	// } else {

	// 	// Check if window is loaded before destroying
	// 	if ( shared->angleLoaded ) {
	// 		cv::destroyWindow( winAngle );
	// 		shared->angleLoaded = false;
	// 	}
	// }
}

void DrawMotorOutput() { }
