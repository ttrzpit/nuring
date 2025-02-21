// Call to class header
#include "DisplayClass.h"

// System data manager
#include "SystemDataManager.h"



/**
 * @brief DisplayClass constructor
 */
DisplayClass::DisplayClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	std::cout << "Display: Thread " << cv::getThreadNum() << " of " << cv::getNumThreads() << "\n";
	cv::namedWindow( "NURing Interface", cv::WINDOW_AUTOSIZE );
	cv::moveWindow( "NURing Interface", 3440 - CONFIG_DIS_WIDTH, 0 );

	// Set font based on chosen resolution
	if ( CONFIG_TYPE == "LowResolution" ) {
		fontHeader = 0.6f;
		fontBody   = 0.5f;
	} else if ( CONFIG_TYPE == "HighResolution" ) {
		fontHeader = 0.5f;
		fontBody   = 0.4f;
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
			cv::line( shared->matFrameOverlay,
					  cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ),
					  cv::Point2i( CONFIG_CAM_PRINCIPAL_X + shared->arucoTags[shared->arucoActiveID].errorMagnitude2D * cos( shared->arucoTags[shared->arucoActiveID].errorTheta ),
								   CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].errorMagnitude2D * sin( shared->arucoTags[shared->arucoActiveID].errorTheta ) ),
					  CONFIG_colCyaDk,
					  1 );
		}
		cv::line( shared->matFrameOverlay,
				  cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ),
				  cv::Point2i( CONFIG_CAM_PRINCIPAL_X + shared->arucoTags[shared->arucoActiveID].errorMagnitudeNorm2D * ( CONFIG_DET_RADIUS / 100 ) * cos( shared->arucoTags[shared->arucoActiveID].errorTheta ),
							   CONFIG_CAM_PRINCIPAL_Y - shared->arucoTags[shared->arucoActiveID].errorMagnitudeNorm2D * ( CONFIG_DET_RADIUS / 100 ) * sin( shared->arucoTags[shared->arucoActiveID].errorTheta ) ),
				  CONFIG_colCyaMd,
				  2 );

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
	DrawCell( "", "A1", 40, 8, 0, CONFIG_colWhite, CONFIG_colBlack, false );

	// Active Marker Block
	DrawCell( "Active Marker", "A1", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "1", "A2", 1, 1, fontBody, ( shared->arucoTags[1].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 1 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "2", "B2", 1, 1, fontBody, ( shared->arucoTags[2].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 2 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "3", "C2", 1, 1, fontBody, ( shared->arucoTags[3].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 3 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "4", "D2", 1, 1, fontBody, ( shared->arucoTags[4].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 4 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "5", "E2", 1, 1, fontBody, ( shared->arucoTags[5].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 5 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "6", "A3", 1, 1, fontBody, ( shared->arucoTags[6].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 6 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "7", "B3", 1, 1, fontBody, ( shared->arucoTags[7].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 7 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "8", "C3", 1, 1, fontBody, ( shared->arucoTags[8].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 8 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "9", "D3", 1, 1, fontBody, ( shared->arucoTags[9].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 9 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "10", "E3", 1, 1, fontBody, ( shared->arucoTags[10].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->arucoActiveID == 10 ? CONFIG_colGreDk : CONFIG_colBlack ), true );

	// Position [mm] Block
	// DrawCell( "", "A4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Position [mm]", "A4", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "x", "A5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "y", "A6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "z", "A7", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "R", "A8", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( shared->arucoTags[shared->arucoActiveID].error3D.x ), "B5", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( shared->arucoTags[shared->arucoActiveID].error3D.y ), "B6", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( shared->arucoTags[shared->arucoActiveID].error3D.z ), "B7", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( shared->arucoTags[shared->arucoActiveID].errorMagnitude3D ), "B8", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Position [px] Block
	DrawCell( "Position [px]", "F4", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "x", "F5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "y", "F6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "P", "F7", 1, 2, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "|R|", "G7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Theta", "I7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( shared->arucoTags[shared->arucoActiveID].error2D.x ), "G5", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( shared->arucoTags[shared->arucoActiveID].error2D.y ), "G6", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( Capture.Markers[Capture.activeMarker].errorMagnitude2D ), "G8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( shared->arucoTags[shared->arucoActiveID].errorMagnitudeNorm2D ), "G8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorTheta * RAD2DEG ) ), "I8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// Mouse output block
	DrawCell( "Mouse", "AF5", 4, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( shared->touchPosition.x ), "AF6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( shared->touchPosition.y ), "AH6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( ( shared->touchPosition.z == 1 ? "Button1" : "" ), "AF7", 4, 2, fontBody, CONFIG_colWhite, ( shared->touchPosition.z ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	// std::cout << mouseXY.z << "\n";

	// Frequency Block
	DrawCell( "Frequency", "AJ1", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( shared->timingFrequency ), "AJ2", 5, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Motor Output Block
	DrawCell( "Amplifiers", "AJ3", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( shared->FLAG_AMPLIFIERS_READY ? "Enabled" : "Off" ), "AJ4", 5, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_AMPLIFIERS_READY ? CONFIG_colGreDk : CONFIG_colBlack ), true );

	// Serial Block
	DrawCell( "Serial Status", "AJ5", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( shared->FLAG_SERIAL_OPEN ? "Online" : "Offline" ), "AJ6", 5, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_SERIAL_OPEN ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( ( shared->FLAG_SERIAL_OPEN ? shared->serialPacket.substr( 0, shared->serialPacket.size() - 1 ) : "---" ), "AJ7", 5, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// // Status Block
	// DrawCell( "Messages:", "A8", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->displayString, "K8", 21, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
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
		textSize = cv::getTextSize( str, cv::FONT_HERSHEY_DUPLEX, sz, 1, 0 );
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
		cv::putText( shared->matFrameOverlay, str, cv::Point( textX, textY ), cv::FONT_HERSHEY_DUPLEX, sz, textColor, 1 );
	} else {
		cv::putText( shared->matFrameOverlay, str, cv::Point( textX, textY ), cv::FONT_HERSHEY_SIMPLEX, sz, textColor, 1 );
	}
}
