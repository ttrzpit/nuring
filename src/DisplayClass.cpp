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

	cv::namedWindow( "NURing Interface", cv::WINDOW_AUTOSIZE );
	cv::moveWindow( "NURing Interface", 3440 - CONFIG_DIS_WIDTH - 2, 0 );

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
	shared->matFrameOverlay = 0;

	// Add video frame
	shared->matFrameUndistorted.copyTo( shared->matFrameOverlay( cv::Rect( 0, 0, shared->matFrameUndistorted.cols, shared->matFrameUndistorted.rows ) ) );

	// Draw green outline and crosshairs if marker found
	if ( shared->FLAG_TAG_FOUND ) {
		cv::circle( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_DET_RADIUS, CONFIG_colGreMd, 2 );
		// cv::rectangle( matDisplay, cv::Point2i( 1, 1 ), cv::Point2i( frame.cols - 2, frame.rows - 3 ), CONFIG_colGreMd, 3 );
		cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_HEIGHT ), CONFIG_colGreMd, 1 );
		cv::line( shared->matFrameOverlay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_WIDTH, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_colGreMd, 1 );

		// Draw vector to center of marker
		if ( shared->arucoTags[shared->arucoActiveID].errorMagnitude2D > CONFIG_DET_RADIUS ) {
			cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ),
					  cv::Point2i( CONFIG_CAM_PRINCIPAL_X + shared->arucoTags[shared->arucoActiveID].errorMagnitude2D * cos( shared->arucoTags[shared->arucoActiveID].errorTheta ),
								   CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].errorMagnitude2D * sin( shared->arucoTags[shared->arucoActiveID].errorTheta ) ),
					  CONFIG_colCyaDk, 1 );
		}
		cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X - shared->arucoTags[shared->arucoActiveID].error2D.x, CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].error2D.y ), CONFIG_colCyaMd, 2 );

		// Draw velocity on marker
		// cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X - shared->arucoTags[shared->arucoActiveID].error2D.x, CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].error2D.y ),
		// 		  cv::Point2i( CONFIG_CAM_PRINCIPAL_X - shared->arucoTags[shared->arucoActiveID].error2D.x + shared->arucoTags[shared->arucoActiveID].errorVel3D.x * 1,
		// 					   CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].error2D.y + shared->arucoTags[shared->arucoActiveID].errorVel3D.y * -1 ),
		// 		  CONFIG_colMagMd, 2 );

		// Draw velocity on center
		cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X - shared->arucoTags[shared->arucoActiveID].errorVel3D.x * 1, CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].errorVel3D.y * -1 ),
				  CONFIG_colMagLt, 2 );


		// Draw border on active tag
		if ( shared->FLAG_TAG_FOUND && shared->arucoTags[shared->arucoActiveID].present ) {
			cv::polylines( shared->matFrameOverlay, shared->arucoActiveCorners, true, CONFIG_colCyaMd, 2 );
		} else {
			cv::polylines( shared->matFrameOverlay, shared->arucoActiveCorners, true, CONFIG_colCyaDk, 2 );
		}


		//
	} else {
		// cv::rectangle( matDisplay, cv::Point2i( 1, 1 ), cv::Point2i( frame.cols - 2, frame.rows - 3 ), CONFIG_colRedDk, 3 );
		cv::circle( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_DET_RADIUS, CONFIG_colRedDk, 1 );
		cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_HEIGHT ), CONFIG_colRedDk, 1 );
		cv::line( shared->matFrameOverlay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_WIDTH, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_colRedDk, 1 );
	}

	// Add text
	AddText();

	// Show interface
	ShowInterface();


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
			cv::destroyWindow( "3D Visualizer" );
		}
	}
}



/**
 * Display the interface on the screen
 */
void DisplayClass::ShowInterface() {

	// Show image
	cv::imshow( "NURing Interface", shared->matFrameOverlay );
	// cv::imshow( "Raw", shared->matFrameUndistorted );

	// Output confirmation
}



/**
 * Add text to the overlay
 */
void DisplayClass::AddText() {

	// Section border
	DrawCell( "", "A1", 40, 7, 0, CONFIG_colWhite, CONFIG_colBlack, false );

	// Active Marker Block
	DrawCell( "Active Marker", "A1", 10, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "1", "A2", 1, 1, fontBody, ( shared->arucoTags[1].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 1 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "2", "B2", 1, 1, fontBody, ( shared->arucoTags[2].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 2 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "3", "C2", 1, 1, fontBody, ( shared->arucoTags[3].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 3 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "4", "D2", 1, 1, fontBody, ( shared->arucoTags[4].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 4 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "5", "E2", 1, 1, fontBody, ( shared->arucoTags[5].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 5 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "6", "F2", 1, 1, fontBody, ( shared->arucoTags[6].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 6 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "7", "G2", 1, 1, fontBody, ( shared->arucoTags[7].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 7 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "8", "H2", 1, 1, fontBody, ( shared->arucoTags[8].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 8 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "9", "I2", 1, 1, fontBody, ( shared->arucoTags[9].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 9 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "10", "J2", 1, 1, fontBody, ( shared->arucoTags[10].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 10 ? CONFIG_colGreDk : CONFIG_colBlack ), true );

	// Position [mm] Block
	DrawCell( "Pos [mm]", "B3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "x", "A4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "y", "A5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "z", "A6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "R", "A7", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].error3D.x ) ), "B4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].error3D.y ) ), "B5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].error3D.z ) ), "B6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorMagnitude3D ) ), "B7", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Velocity [mm/s] Block
	DrawCell( "Vel [mm/s]", "D3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorVel3D.x ) ), "D4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorVel3D.y ) ), "D5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorVel3D.z ) ), "D6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Position [px] Block
	DrawCell( "Position [px]", "F3", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "x", "F4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "y", "F5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Error", "F6", 1, 2, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "|e| [mm]", "G6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Theta [deg]", "I6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].error2D.x * -1 ) ), "G4", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].error2D.y ) ), "G5", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( Capture.Markers[Capture.activeMarker].errorMagnitude2D ), "G8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorMagnitude2D ) ), "G7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorTheta * RAD2DEG ) ), "I7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Controller block
	DrawCell( "Controller Settings", "K3", 8, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "K [N/mm]", "K4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "B [N*s/mm]", "K5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( []( float value ) {
				  std::ostringstream out;
				  out << std::fixed << std::setprecision( 3 ) << value;
				  return out.str();
			  } )( shared->controllerK ),
			  "M4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( ( []( float value ) {
				  std::ostringstream out;
				  out << std::fixed << std::setprecision( 3 ) << value;
				  return out.str();
			  } )( shared->controllerB ),
			  "M5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Fx [N]", "O4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Fy [N]", "O5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( []( float value ) {
				  std::ostringstream out;
				  out << std::fixed << std::setprecision( 3 ) << value;
				  return out.str();
			  } )( shared->controllerFx ),
			  "Q4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( ( []( float value ) {
				  std::ostringstream out;
				  out << std::fixed << std::setprecision( 3 ) << value;
				  return out.str();
			  } )( shared->controllerFy ),
			  "Q5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// Frequency Block
	// DrawCell( "Frequency", "AG1", 4, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( shared->timingFrequency ), "AG2", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// // Mouse output block
	// DrawCell( "Mouse", "AG3", 4, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( shared->touchPosition.x ), "AG4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( shared->touchPosition.y ), "AI4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( ( shared->touchPosition.z == 1 ? "Button1" : "" ), "AG5", 4, 1, fontBody, CONFIG_colWhite, ( shared->touchPosition.z ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	// // std::cout << mouseXY.z << "\n";

	// Status blocks
	DrawCell( "Frequency", "AG1", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( shared->timingFrequency ), "AG2", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Amplifiers", "AI1", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( shared->FLAG_AMPLIFIERS_READY ? "Online" : "Offline" ), "AI2", 2, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_AMPLIFIERS_READY ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "Serial Status", "AK1", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( shared->FLAG_SERIAL_OPEN ? "Online" : "Offline" ), "AK2", 2, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_SERIAL_OPEN ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "Logging", "AM1", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( shared->FLAG_LOGGING_ON ? "Logging" : "Offline" ), "AM2", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Serial packet blocks
	DrawCell( "Outgoing Packet", "AG3", 8, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( ( shared->FLAG_SERIAL_OPEN && shared->FLAG_AMPLIFIERS_READY ) ? shared->serialPacket.substr( 0, shared->serialPacket.size() - 1 ) : "Amplifiers or serial disabled" ), "AG4", 8, 2, fontHeader * 2, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Filename", "AG6", 8, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[FILENAME]", "AG7", 8, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// // Trial blocks
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
	DrawCell( "System:", "K6", 2, 2, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->displayString, "M6", 20, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
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

	std::cout << key_c0 << "," << key_r0 << "," << key_rH << "," << key_cW << "\n";
}



void DisplayClass::ShowShortcuts() {

	// Create window
	cv::namedWindow( "Keyboard Shortcuts", cv::WINDOW_AUTOSIZE );
	cv::moveWindow( "Keyboard Shortcuts", 3440 - CONFIG_DIS_WIDTH - CONFIG_DIS_KEY_WIDTH - 4, 0 );

	// System
	DrawKeyCell( "System:", "A1", 6, 1, key_fontHeader * 1.3, CONFIG_colWhite, CONFIG_colGraBk, false );
	DrawKeyCell( "ESC", "A2", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Shutdown software", "B2", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "e", "A3", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Enable amplifier (toggle)", "B3", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "s", "A4", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Send serial data (toggle)", "B4", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "x", "A5", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "E-Stop (kill amp & serial)", "B5", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );


	// Controller
	DrawKeyCell( "Controller:", "A7", 6, 1, key_fontHeader * 1.3, CONFIG_colWhite, CONFIG_colGraBk, false );
	DrawKeyCell( "1-5", "A8", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Change active marker to 1-5", "B8", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "[  ]", "A9", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Decrease | increase spring K", "B9", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( ";   '", "A10", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Decrease | increase damping B", "B10", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );

	// Visualizer
	DrawKeyCell( "Visualizer:", "A12", 6, 1, key_fontHeader * 1.3, CONFIG_colWhite, CONFIG_colGraBk, false );
	DrawKeyCell( "v", "A13", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Enable visualizer (toggle)", "B13", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "c", "A14", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Clear visualizer ", "B14", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );



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
	cv::namedWindow( "3D Visualizer", cv::WINDOW_AUTOSIZE );
	cv::moveWindow( "3D Visualizer", 3440 - CONFIG_DIS_WIDTH - CONFIG_DIS_VIZ_WIDTH - 6, 0 );
	cv::imshow( "3D Visualizer", matVisualizer );
}


void DisplayClass::UpdateVisualizer() {


	// Check if trail should be cleared
	if ( shared->vizClear ) {
		trailPoints.clear();
		shared->vizClear = false;
	}

	// Clear canvas
	matVisualizer = CONFIG_colWhite;


	// Draw cube
	for ( const auto& edge : edges ) {
		cv::line( matVisualizer, ProjectedCorners[edge.first], ProjectedCorners[edge.second], CONFIG_colGraLt, 1 );
	}

	// Draw target square
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( 0, -30, 30 ) ), ProjectIsometric( cv::Point3f( 0, 30, 30 ) ), CONFIG_colGraMd, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( 0, -30, -30 ) ), ProjectIsometric( cv::Point3f( 0, 30, -30 ) ), CONFIG_colGraMd, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( 0, 30, -30 ) ), ProjectIsometric( cv::Point3f( 0, 30, 30 ) ), CONFIG_colGraMd, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( 0, -30, -30 ) ), ProjectIsometric( cv::Point3f( 0, -30, 30 ) ), CONFIG_colGraMd, 1 );



	// X axis
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( 0, -vizLimXY, 0 ) ), ProjectIsometric( cv::Point3f( 0, vizLimXY, 0 ) ), CONFIG_colRedLt, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( 0, vizLimXY, 0 ) ), ProjectIsometric( cv::Point3f( vizLimZ, vizLimXY, 0 ) ), CONFIG_colRedLt, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( 0, -vizLimXY, 0 ) ), ProjectIsometric( cv::Point3f( 1000, -vizLimXY, 0 ) ), CONFIG_colRedLt, 1 );

	// Y axis
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( 0, 0, -vizLimXY ) ), ProjectIsometric( cv::Point3f( 0, 0, vizLimXY ) ), CONFIG_colGreLt, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( 0, 0, vizLimXY ) ), ProjectIsometric( cv::Point3f( 1000, 0, vizLimXY ) ), CONFIG_colGreLt, 1 );


	// Draw shadows
	// cv::Point3f p = shared->arucoTags[shared->arucoActiveID].error3D;
	cv::Point3f p = cv::Point3f( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z - zOffset );

	// Create color gradient
	float zClamped = std::clamp( shared->arucoTags[shared->arucoActiveID].error3D.z - zOffset, 0.0f, 1000.0f );
	// float intensity = 128.0f * ( zClamped / 1000.0f );

	// Add current point to trail
	if ( trailCounter++ >= trailInterval ) {
		trailPoints.push_back( p );
		// trailColor.push_back( ( int )intensity );
		trailCounter = 0;

		// Limit trail length
		if ( trailPoints.size() > trailLimit ) {
			trailPoints.erase( trailPoints.begin() );
			// trailColor.erase( trailColor.begin() );
		}
	}



	// Moving axis
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( p.z - zOffset, -p.y, vizLimXY ) ), ProjectIsometric( cv::Point3f( p.z - zOffset, -p.y, p.x ) ), CONFIG_colGreMd, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( p.z - zOffset, vizLimXY, p.x ) ), ProjectIsometric( cv::Point3f( p.z - zOffset, -p.y, p.x ) ), CONFIG_colRedMd, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3f( 0, -p.y, p.x ) ), ProjectIsometric( cv::Point3f( p.z - zOffset, -p.y, p.x ) ), CONFIG_colBluMd, 1 );

	int ptSize = ( ( 1000 - p.z ) / 800 ) * 10;

	// Draw 3D dot
	cv::Point2i projected = ProjectIsometric( cv::Point3f( shared->arucoTags[shared->arucoActiveID].error3D.z - zOffset, shared->arucoTags[shared->arucoActiveID].error3D.y * -1, shared->arucoTags[shared->arucoActiveID].error3D.x ) );



	// Draw trail points
	for ( int i = 0; i < trailPoints.size(); i++ ) {

		float ratio		= static_cast<float>( i ) / static_cast<float>( trailPoints.size() );
		int	  intensity = static_cast<int>( 191 * ( 1.0f - ratio ) );

		// // Z trail
		cv::Point2i shadowXYZ = ProjectIsometric( cv::Point3i( 0, -trailPoints.at( i ).y, trailPoints.at( i ).x ) );
		cv::circle( matVisualizer, shadowXYZ, 2, cv::Scalar( 255, 64 + intensity, 64 + intensity ), -1 );

		// Y trail
		cv::Point2i shadowXY = ProjectIsometric( cv::Point3i( trailPoints.at( i ).z, -trailPoints.at( i ).y, vizLimXY ) );
		cv::circle( matVisualizer, shadowXY, 2, cv::Scalar( 64 + intensity, 255, 64 + intensity ), -1 );

		// X trail
		cv::Point2f shadowXZ = ProjectIsometric( cv::Point3f( trailPoints.at( i ).z, vizLimXY, trailPoints.at( i ).x ) );	 // Swap x<->z
		cv::circle( matVisualizer, shadowXZ, 2, cv::Scalar( 64 + intensity, 64 + intensity, 255 ), -1 );
	}

	// Line from marker to target
	cv::line( matVisualizer, projected, ProjectIsometric( cv::Point3f( 0, 0, 0 ) ), CONFIG_colCyaMd, 2 );

	// Draw marker
	cv::circle( matVisualizer, projected, 10, CONFIG_colWhite, -1 );
	cv::circle( matVisualizer, projected, 10, CONFIG_colBlack, 1 );

	// Shadow on XY plane (bottom)
	cv::Point2f shadowXY = ProjectIsometric( cv::Point3f( 0, -p.y, p.x ) );	   // Swap x<->z
	cv::circle( matVisualizer, shadowXY, ptSize, CONFIG_colBluLt, 1 );		   // Red-ish

	// Shadow on YZ plane (left wall at x = -500)
	cv::Point2f shadowYZ = ProjectIsometric( cv::Point3f( p.z, -p.y, vizLimXY ) );	  // Swap x<->z
	cv::circle( matVisualizer, shadowYZ, 5, CONFIG_colGreDk, 1 );					  // Y shadow

	// Shadow on XZ plane (back wall at y = 500)
	cv::Point2f shadowXZ = ProjectIsometric( cv::Point3f( p.z, vizLimXY, p.x ) );	 // Swap x<->z
	cv::circle( matVisualizer, shadowXZ, 5, CONFIG_colRedDk, 1 );					 // X shadow

	// Show
	cv::imshow( "3D Visualizer", matVisualizer );
}


// cv::Point2i DisplayClass::ProjectPoint( const cv::Point3f& p3d ) {
// 	if ( p3d.z == 0 )
// 		return cameraCenter;	// Avoid divide by zero
// 	int x2d = ( p3d.x * focalLength ) / ( float )p3d.z + cameraCenter.x;
// 	int y2d = ( p3d.y * focalLength ) / ( float )p3d.z + cameraCenter.y;
// 	return ( cv::Point2i( x2d, y2d ) );
// }


// Apply isometric camera transform and perspective
cv::Point2i DisplayClass::ProjectIsometric( const cv::Point3f& p3d ) {
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
