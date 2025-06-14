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
		fontTitle	   = 0.6f;
		fontHeader	   = 0.45f;
		fontBody	   = 0.45f;
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
	shared->Capture.matFrameUndistorted.copyTo( shared->Display.matFrameOverlay( cv::Rect( 0, 0, shared->Capture.matFrameUndistorted.cols, shared->Capture.matFrameUndistorted.rows ) ) );


	/** Draw GUI elements **/

	// Draw detector crosshairs, changing colors based on if the target is present
	cv::circle( shared->Display.matFrameOverlay, CONFIG_CAM_CENTER, CONFIG_DET_RADIUS, ( shared->Telemetry.isTargetFound ? CONFIG_colGreMd : CONFIG_colRedDk ), 1 );
	cv::circle( shared->Display.matFrameOverlay, CONFIG_CAM_CENTER, shared->Controller.integrationRadius, ( shared->Telemetry.isTargetFound ? CONFIG_colYelDk : CONFIG_colRedDk ), 1 );
	cv::line( shared->Display.matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_HEIGHT ), ( shared->Telemetry.isTargetFound ? CONFIG_colGreMd : CONFIG_colRedDk ), 1 );
	cv::line( shared->Display.matFrameOverlay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_WIDTH, CONFIG_CAM_PRINCIPAL_Y ), ( shared->Telemetry.isTargetFound ? CONFIG_colGreMd : CONFIG_colRedDk ), 1 );

	// Draw motor axis
	cv::line( shared->Display.matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS35 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN35 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );
	cv::line( shared->Display.matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS145 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN145 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );
	cv::line( shared->Display.matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS270 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN270 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );

	// Draw information for target marker if present
	if ( shared->Telemetry.isTargetFound ) {

		//Draw vector to center of target
		cv::line( shared->Display.matFrameOverlay, CONFIG_CAM_CENTER, shared->Telemetry.screenPositionPX, CONFIG_colCyaMd, 2 );

		// Draw border and axis elements of target marker
		cv::polylines( shared->Display.matFrameOverlay, shared->Telemetry.cornersPX, true, CONFIG_colGreMd, 2 );
		// cv::drawFrameAxes( shared->matFrameUndistorted, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, shared->targetMarkerRotationVector, shared->targetMarkerTranslationVector, CONFIG_LARGE_MARKER_WIDTH, 15 );

		// Draw velocity
		cv::line( shared->Display.matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_CENTER.x - shared->Telemetry.velocityFilteredNewMM.x * MM2PX / 2.0f, CONFIG_CAM_CENTER.y + shared->Telemetry.velocityFilteredNewMM.y * MM2PX / 2.0f ), CONFIG_colMagLt, 2 );
	}


	// Draw calibrated marker
	if ( shared->calibrationComplete ) {
		// int newX = shared->calibrationOffsetPX.x - CONFIG_TOUCHSCREEN_CENTER.x + CONFIG_CAM_PRINCIPAL_X;
		// int newY = shared->calibrationOffsetPX.y - CONFIG_TOUCHSCREEN_CENTER.y + CONFIG_CAM_PRINCIPAL_Y - 20 * MM2PX;
		// cv::circle( shared->Display.matFrameOverlay, cv::Point2i( newX, newY ), 10 * MM2PX, CONFIG_colGreLt, 2 );
		// cv::circle( shared->Display.matFrameOverlay, shared->, 10 * MM2PX, CONFIG_colGreLt, 2 );
	}

	// Break components for PID

	uint8_t		rad = 80;
	cv::Point2i centerProp( 1500, 100 );
	cv::Point2i centerInt( 1500, 300 );
	cv::Point2i centerDeriv( 1500, 500 );
	cv::Point2i propA = cv::Point2i( centerProp.x + shared->Controller.percentageProportional.x * COS35 * rad, centerProp.y - shared->Controller.percentageProportional.x * SIN35 * rad );
	cv::Point2i propB = cv::Point2i( centerProp.x + shared->Controller.percentageProportional.y * COS145 * rad, centerProp.y - shared->Controller.percentageProportional.y * SIN145 * rad );
	cv::Point2i propC = cv::Point2i( centerProp.x + shared->Controller.percentageProportional.z * COS270 * rad, centerProp.y - shared->Controller.percentageProportional.z * SIN270 * rad );
	cv::Point2i intA  = cv::Point2i( centerInt.x + shared->Controller.percentageIntegral.x * COS35 * rad, centerInt.y - shared->Controller.percentageIntegral.x * SIN35 * rad );
	cv::Point2i intB  = cv::Point2i( centerInt.x + shared->Controller.percentageIntegral.y * COS145 * rad, centerInt.y - shared->Controller.percentageIntegral.y * SIN145 * rad );
	cv::Point2i intC  = cv::Point2i( centerInt.x + shared->Controller.percentageIntegral.z * COS270 * rad, centerInt.y - shared->Controller.percentageIntegral.z * SIN270 * rad );
	cv::Point2i derA  = cv::Point2i( centerDeriv.x + shared->Controller.percentageDerivative.x * COS35 * rad, centerDeriv.y - shared->Controller.percentageDerivative.x * SIN35 * rad );
	cv::Point2i derB  = cv::Point2i( centerDeriv.x + shared->Controller.percentageDerivative.y * COS145 * rad, centerDeriv.y - shared->Controller.percentageDerivative.y * SIN145 * rad );
	cv::Point2i derC  = cv::Point2i( centerDeriv.x + shared->Controller.percentageDerivative.z * COS270 * rad, centerDeriv.y - shared->Controller.percentageDerivative.z * SIN270 * rad );

	// Base Prop
	cv::circle( shared->Display.matFrameOverlay, centerProp, rad, CONFIG_colGraBk, -1 );
	cv::line( shared->Display.matFrameOverlay, centerProp, cv::Point2i( centerProp.x + rad * COS35, centerProp.y - rad * SIN35 ), CONFIG_colGraMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerProp, cv::Point2i( centerProp.x + rad * COS145, centerProp.y - rad * SIN145 ), CONFIG_colGraMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerProp, cv::Point2i( centerProp.x + rad * COS270, centerProp.y - rad * SIN270 ), CONFIG_colGraMd, 2 );

	// Components Prop
	cv::line( shared->Display.matFrameOverlay, centerProp, propA, CONFIG_colCyaMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerProp, propB, CONFIG_colCyaMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerProp, propC, CONFIG_colCyaMd, 2 );
	cv::circle( shared->Display.matFrameOverlay, propA, 5, CONFIG_colCyaMd, -1 );
	cv::circle( shared->Display.matFrameOverlay, propB, 5, CONFIG_colCyaMd, -1 );
	cv::circle( shared->Display.matFrameOverlay, propC, 5, CONFIG_colCyaMd, -1 );

	// Outline Prop
	cv::circle( shared->Display.matFrameOverlay, centerProp, rad, CONFIG_colGraMd, 2 );


	// Base Int
	cv::circle( shared->Display.matFrameOverlay, centerInt, rad, CONFIG_colGraBk, -1 );
	cv::line( shared->Display.matFrameOverlay, centerInt, cv::Point2i( centerInt.x + rad * COS35, centerInt.y - rad * SIN35 ), CONFIG_colGraMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerInt, cv::Point2i( centerInt.x + rad * COS145, centerInt.y - rad * SIN145 ), CONFIG_colGraMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerInt, cv::Point2i( centerInt.x + rad * COS270, centerInt.y - rad * SIN270 ), CONFIG_colGraMd, 2 );

	// Components Int
	cv::line( shared->Display.matFrameOverlay, centerInt, intA, CONFIG_colYelMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerInt, intB, CONFIG_colYelMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerInt, intC, CONFIG_colYelMd, 2 );
	cv::circle( shared->Display.matFrameOverlay, intA, 5, CONFIG_colYelMd, -1 );
	cv::circle( shared->Display.matFrameOverlay, intB, 5, CONFIG_colYelMd, -1 );
	cv::circle( shared->Display.matFrameOverlay, intC, 5, CONFIG_colYelMd, -1 );

	// Outline Int
	cv::circle( shared->Display.matFrameOverlay, centerInt, rad, CONFIG_colGraMd, 2 );


	// Base Int
	cv::circle( shared->Display.matFrameOverlay, centerDeriv, rad, CONFIG_colGraBk, -1 );
	cv::line( shared->Display.matFrameOverlay, centerDeriv, cv::Point2i( centerDeriv.x + rad * COS35, centerDeriv.y - rad * SIN35 ), CONFIG_colGraMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerDeriv, cv::Point2i( centerDeriv.x + rad * COS145, centerDeriv.y - rad * SIN145 ), CONFIG_colGraMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerDeriv, cv::Point2i( centerDeriv.x + rad * COS270, centerDeriv.y - rad * SIN270 ), CONFIG_colGraMd, 2 );

	// Components Int
	cv::line( shared->Display.matFrameOverlay, centerDeriv, derA, CONFIG_colRedMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerDeriv, derB, CONFIG_colRedMd, 2 );
	cv::line( shared->Display.matFrameOverlay, centerDeriv, derC, CONFIG_colRedMd, 2 );
	cv::circle( shared->Display.matFrameOverlay, derA, 5, CONFIG_colRedMd, -1 );
	cv::circle( shared->Display.matFrameOverlay, derB, 5, CONFIG_colRedMd, -1 );
	cv::circle( shared->Display.matFrameOverlay, derC, 5, CONFIG_colRedMd, -1 );

	// Outline Int
	cv::circle( shared->Display.matFrameOverlay, centerDeriv, rad, CONFIG_colGraMd, 2 );

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

	// Catch mouse
	// cv::setMouseCallback( winInterface, DisplayClass::onMouse, this );


	// Show image
	cv::imshow( winInterface, shared->Display.matFrameOverlay );
	// cv::imshow( "Raw", shared->matFrameUndistorted );

	// Output confirmation
}



/**
 * Add text to the overlay
 */
void DisplayClass::AddText() {

	// Section border
	DrawCell( "", "A1", 40, 10, 0, CONFIG_colWhite, CONFIG_colBlack, false );

	// Telemetry
	// Position
	DrawCell( "TELEMETRY", "A1", 8, 1, fontTitle, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( std::to_string( shared->Telemetry.activeID ), "A2", 2, 2, fontBody * 3, CONFIG_colWhite, ( shared->Telemetry.isTargetFound ? CONFIG_colGreBk : CONFIG_colBlack ), true );
	DrawCell( "Position", "C2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[mm]", "C3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "x", "A4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "y", "A5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "z", "A6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Rxy", "A7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Rxyz", "A8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->Telemetry.positionFilteredNewMM.x ) ), "C4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->Telemetry.positionFilteredNewMM.y ) ), "C5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->Telemetry.positionFilteredNewMM.z ) ), "C6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->GetNorm2D( cv::Point2f( shared->Telemetry.positionFilteredNewMM.x, shared->Telemetry.positionFilteredNewMM.y ) ) ) ), "C7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->GetNorm3D( shared->Telemetry.positionFilteredNewMM ) ) ), "C8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Velocity
	DrawCell( "Velocity", "E2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[mm/s]", "E3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->Telemetry.velocityFilteredNewMM.x ) ), "E4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->Telemetry.velocityFilteredNewMM.y ) ), "E5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->Telemetry.velocityFilteredNewMM.z ) ), "E6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->GetNorm2D( cv::Point2f( shared->Telemetry.velocityFilteredNewMM.x, shared->Telemetry.velocityFilteredNewMM.y ) ) ) ), "E7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->GetNorm3D( shared->Telemetry.velocityFilteredNewMM ) ) ), "E8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Integrated Error
	DrawCell( "Integr.", "G2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[mm]", "G3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Telemetry.positionIntegratedMM.x, 1, 0 ), "G4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Telemetry.positionIntegratedMM.y, 1, 0 ), "G5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Telemetry.positionIntegratedMM.z, 1, 0 ), "G6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "xx", "G7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "xx", "G8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// Handler for gains
	auto& in = shared->Input;

	// ( ( in.selectGainTarget == selectGainTargetEnum::ABD ) && ( in.selectGain == selectGainEnum::KP ) ? CONFIG_colYelDk : CONFIG_colBlack )

	// Controller
	DrawCell( "CONTROLLER", "I1", 17, 1, fontTitle, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( "Freq", "I2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->Timing.measuredFrequency ) ), "I3", 2, 1, fontBody, CONFIG_colWhite, ( shared->Timing.measuredFrequency > 60 ? CONFIG_colGreBk : CONFIG_colRedBk ), true );
	DrawCell( "ABD", "I4", 2, 1, fontHeader, CONFIG_colWhite, ( in.selectGainTarget == selectGainTargetEnum::ABD ? CONFIG_colYelBk : CONFIG_colGraBk ), true );
	DrawCell( "ADD", "I5", 2, 1, fontHeader, CONFIG_colWhite, ( in.selectGainTarget == selectGainTargetEnum::ADD ? CONFIG_colYelBk : CONFIG_colGraBk ), true );
	DrawCell( "FLEX", "I6", 2, 1, fontHeader, CONFIG_colWhite, ( in.selectGainTarget == selectGainTargetEnum::FLEX ? CONFIG_colYelBk : CONFIG_colGraBk ), true );
	DrawCell( "EXT", "I7", 2, 1, fontHeader, CONFIG_colWhite, ( in.selectGainTarget == selectGainTargetEnum::EXT ? CONFIG_colYelBk : CONFIG_colGraBk ), true );
	DrawCell( "Kp", "K2", 2, 1, fontHeader, CONFIG_colWhite, ( in.selectGain == selectGainEnum::KP ? CONFIG_colYelBk : CONFIG_colGraBk ), true );
	DrawCell( "[u]", "K3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKp.abd, 1, 1 ), "K4", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::ABD ) && ( in.selectGain == selectGainEnum::KP ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKp.add, 1, 1 ), "K5", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::ADD ) && ( in.selectGain == selectGainEnum::KP ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKp.flx, 1, 1 ), "K6", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::FLEX ) && ( in.selectGain == selectGainEnum::KP ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKp.ext, 1, 1 ), "K7", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::EXT ) && ( in.selectGain == selectGainEnum::KP ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( "Ki", "M2", 2, 1, fontHeader, CONFIG_colWhite, ( in.selectGain == selectGainEnum::KI ? CONFIG_colYelBk : CONFIG_colGraBk ), true );
	DrawCell( "[u]", "M3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKi.abd, 1, 1 ), "M4", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::ABD ) && ( in.selectGain == selectGainEnum::KI ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKi.add, 1, 1 ), "M5", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::ADD ) && ( in.selectGain == selectGainEnum::KI ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKi.flx, 1, 1 ), "M6", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::FLEX ) && ( in.selectGain == selectGainEnum::KI ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKi.ext, 1, 1 ), "M7", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::EXT ) && ( in.selectGain == selectGainEnum::KI ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( "Kd", "O2", 2, 1, fontHeader, CONFIG_colWhite, ( in.selectGain == selectGainEnum::KD ? CONFIG_colYelBk : CONFIG_colGraBk ), true );
	DrawCell( "[u]", "O3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKd.abd, 1, 2 ), "O4", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::ABD ) && ( in.selectGain == selectGainEnum::KD ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKd.add, 1, 2 ), "O5", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::ADD ) && ( in.selectGain == selectGainEnum::KD ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKd.flx, 1, 2 ), "O6", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::FLEX ) && ( in.selectGain == selectGainEnum::KD ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.gainKd.ext, 1, 2 ), "O7", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::EXT ) && ( in.selectGain == selectGainEnum::KD ) ? CONFIG_colYelDk : CONFIG_colGraBk ), true );
	DrawCell( "x", "Q4", 1, 2, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "y", "Q6", 1, 2, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Kp*E", "R2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[u]", "R3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Controller.proportionalTerm.x, 1, 1 ), "R4", 2, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Controller.proportionalTerm.y, 1, 1 ), "R6", 2, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Ki*IE", "T2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[u]", "T3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Controller.integralTerm.x, 1, 1 ), "T4", 2, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Controller.integralTerm.y, 1, 1 ), "T6", 2, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Kp*dE", "V2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[u]", "V3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Controller.derivativeTerm.x, 1, 1 ), "V4", 2, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Controller.derivativeTerm.y, 1, 1 ), "V6", 2, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Total", "X2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[u]", "X3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Controller.combinedPIDTerms.x, 1, 1 ), "X4", 2, 2, fontBody * 1.5f, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Controller.combinedPIDTerms.y, 1, 1 ), "X6", 2, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// System Flags
	DrawCell( shared->Amplifier.isLimitSet ? "Limit Set" : "No Limits", "I8", 2, 1, fontHeader, CONFIG_colWhite, shared->Amplifier.isLimitSet ? CONFIG_colGreBk : CONFIG_colRedBk, true );
	DrawCell( "Calib:", "K8", 2, 1, fontHeader, CONFIG_colWhite, shared->Calibration.isCalibrated ? CONFIG_colGreBk : CONFIG_colRedBk, true );
	DrawCell( std::to_string( shared->Calibration.calibratedOffetMM.x ), "M8", 1, 1, fontHeader * 0.65f, CONFIG_colWhite, shared->Calibration.isCalibrated ? CONFIG_colGreBk : CONFIG_colRedBk, true );
	DrawCell( std::to_string( shared->Calibration.calibratedOffetMM.y ), "N8", 1, 1, fontHeader * 0.65f, CONFIG_colWhite, shared->Calibration.isCalibrated ? CONFIG_colGreBk : CONFIG_colRedBk, true );
	DrawCell( std::to_string( shared->Calibration.calibratedOffetMM.z ), "O8", 1, 1, fontHeader * 0.65f, CONFIG_colWhite, shared->Calibration.isCalibrated ? CONFIG_colGreBk : CONFIG_colRedBk, true );
	DrawCell( "Teensy In", "P8", 3, 1, fontHeader, CONFIG_colWhite, shared->Serial.isSerialReceiveOpen ? CONFIG_colGreBk : CONFIG_colRedBk, true );
	DrawCell( "Teensy Out", "S8", 3, 1, fontHeader, CONFIG_colWhite, shared->Serial.isSerialSendOpen ? CONFIG_colGreBk : CONFIG_colRedBk, true );

	if ( shared->Amplifier.isAmplifierActive ) {
		if ( shared->Amplifier.isTensionOnly ) {

			DrawCell( "Tension Only ", "V8", 4, 1, fontHeader, CONFIG_colWhite, shared->Amplifier.isAmplifierActive ? CONFIG_colGreBk : CONFIG_colRedBk, true );
		} else {
			DrawCell( "Driving ", "V8", 4, 1, fontHeader, CONFIG_colWhite, shared->Amplifier.isAmplifierActive ? CONFIG_colGreBk : CONFIG_colRedBk, true );
		}
	} else {
		DrawCell( "Amplifier Off ", "V8", 4, 1, fontHeader, CONFIG_colWhite, shared->Amplifier.isAmplifierActive ? CONFIG_colGreBk : CONFIG_colRedBk, true );
	}



	// Controller
	DrawCell( "AMPLIFIER", "Z1", 15, 1, fontTitle, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( "Motor", "Z3", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "A", "AC2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "B", "AE2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "C", "AG2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Tension", "Z3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[%]", "AB3", 1, 1, fontHeader * 0.8f, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Controller.commandedTensionABC.x * 100.0, 3, 1 ), "AC3", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::AMPA ) ? CONFIG_colYelDk : CONFIG_colBlack ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.commandedTensionABC.y * 100.0, 3, 1 ), "AE3", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::AMPB ) ? CONFIG_colYelDk : CONFIG_colBlack ), true );
	DrawCell( shared->FormatDecimal( shared->Controller.commandedTensionABC.z * 100.0, 3, 1 ), "AG3", 2, 1, fontBody, CONFIG_colWhite, ( ( in.selectGainTarget == selectGainTargetEnum::AMPC ) ? CONFIG_colYelDk : CONFIG_colBlack ), true );
	DrawCell( "Drive", "Z4", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[%]", "AB4", 1, 1, fontHeader * 0.6f, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( ( shared->Controller.commandedPercentageABC.x - shared->Controller.commandedTensionABC.x ) * 100.0, 3, 1 ), "AC4", 2, 1, fontBody, ( shared->Amplifier.isTensionOnly ? CONFIG_colGraDk : CONFIG_colWhite ), CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( ( shared->Controller.commandedPercentageABC.y - shared->Controller.commandedTensionABC.y ) * 100.0, 3, 1 ), "AE4", 2, 1, fontBody, ( shared->Amplifier.isTensionOnly ? CONFIG_colGraDk : CONFIG_colWhite ), CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( ( shared->Controller.commandedPercentageABC.z - shared->Controller.commandedTensionABC.z ) * 100.0, 3, 1 ), "AG4", 2, 1, fontBody, ( shared->Amplifier.isTensionOnly ? CONFIG_colGraDk : CONFIG_colWhite ), CONFIG_colBlack, true );
	DrawCell( "Total", "Z5", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[%]", "AB5", 1, 1, fontHeader * 0.6f, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Controller.commandedPercentageABC.x * 100.0, 3, 1 ), "AC5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Controller.commandedPercentageABC.y * 100.0, 3, 1 ), "AE5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Controller.commandedPercentageABC.z * 100.0, 3, 1 ), "AG5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Max", "Z6", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[%]", "AB6", 1, 1, fontHeader * 0.6f, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Controller.commandedPercentageLimit.x * 100.0f, 3, 1 ), "AC6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Controller.commandedPercentageLimit.y * 100.0f, 3, 1 ), "AE6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Controller.commandedPercentageLimit.z * 100.0f, 3, 1 ), "AG6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "PWM", "Z7", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[DC]", "AB7", 1, 1, fontHeader * 0.6f, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( shared->Controller.commandedPwmABC.x ), "AC7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( shared->Controller.commandedPwmABC.y ), "AE7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( shared->Controller.commandedPwmABC.z ), "AG7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Current", "Z8", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[A]", "AB8", 1, 1, fontHeader * 0.6f, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Amplifier.currentMeasuredAmpsA, 1, 2 ), "AC8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Amplifier.currentMeasuredAmpsB, 1, 2 ), "AE8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->Amplifier.currentMeasuredAmpsC, 1, 2 ), "AG8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Angle", "Z9", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[deg]", "AB9", 1, 1, fontHeader * 0.6f, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Amplifier.encoderMeasuredDegA, 2, 2 ), "AC9", 2, 1, fontBody, CONFIG_colWhite, ( ( shared->Amplifier.isLimitSet && shared->Amplifier.isOverLimitA ) ? CONFIG_colRedDk : CONFIG_colGreBk ), true );
	DrawCell( shared->FormatDecimal( shared->Amplifier.encoderMeasuredDegB, 2, 2 ), "AE9", 2, 1, fontBody, CONFIG_colWhite, ( ( shared->Amplifier.isLimitSet && shared->Amplifier.isOverLimitB ) ? CONFIG_colRedDk : CONFIG_colGreBk ), true );
	DrawCell( shared->FormatDecimal( shared->Amplifier.encoderMeasuredDegC, 2, 2 ), "AG9", 2, 1, fontBody, CONFIG_colWhite, ( ( shared->Amplifier.isLimitSet && shared->Amplifier.isOverLimitC ) ? CONFIG_colRedDk : CONFIG_colGreBk ), true );
	DrawCell( "Limit", "Z10", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[deg]", "AB10", 1, 1, fontHeader * 0.6f, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Amplifier.encoderLimitDegA, 2, 2 ), "AC10", 2, 1, fontBody, CONFIG_colWhite, ( shared->Amplifier.isMeasuringEncoderLimit ? CONFIG_colYelDk : CONFIG_colBlack ), true );
	DrawCell( shared->FormatDecimal( shared->Amplifier.encoderLimitDegB, 2, 2 ), "AE10", 2, 1, fontBody, CONFIG_colWhite, ( shared->Amplifier.isMeasuringEncoderLimit ? CONFIG_colYelDk : CONFIG_colBlack ), true );
	DrawCell( shared->FormatDecimal( shared->Amplifier.encoderLimitDegC, 2, 2 ), "AG10", 2, 1, fontBody, CONFIG_colWhite, ( shared->Amplifier.isMeasuringEncoderLimit ? CONFIG_colYelDk : CONFIG_colBlack ), true );



	// Serial I/O
	DrawCell( "PC Out", "A9", 2, 1, fontHeader, CONFIG_colWhite, shared->Serial.isSerialSending ? CONFIG_colGreBk : CONFIG_colRedBk, true );
	DrawCell( "PC In", "A10", 2, 1, fontHeader, CONFIG_colWhite, shared->Serial.isSerialReceiving ? CONFIG_colGreBk : CONFIG_colRedBk, true );
	DrawCell( shared->Serial.isSerialSending ? shared->Serial.packetOut : "Not sending", "C9", 23, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawCell( shared->Serial.isSerialReceiving ? shared->Serial.packetIn : "Not receiving", "C10", 23, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	// Serial delay
	DrawCell( "Lag", "X9", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( shared->Serial.packetDelay ), "X10", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "[ms]", "Y10", 1, 1, fontBody * 0.6f, CONFIG_colWhite, CONFIG_colGraBk, true );

	// Motor Output Block
	DrawCell( "", "AI2", 6, 7, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	cv::Point2i center( 1184, 1100 + 117 );
	float		motorR = 80.0f;

	limA = shared->Controller.commandedPercentageLimit.x * motorR;
	limB = shared->Controller.commandedPercentageLimit.y * motorR;
	limC = shared->Controller.commandedPercentageLimit.z * motorR;


	// Motor limits
	if ( shared->Controller.isLimitSet ) {
		cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x + COS35 * limA, center.y - SIN35 * limA ), CONFIG_colGraDk, 10 );
		cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x + COS145 * limB, center.y - SIN145 * limB ), CONFIG_colGraDk, 10 );
		cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * limC ), CONFIG_colGraDk, 10 );
	}

	// Motor segments
	cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x + COS35 * motorR, center.y - SIN35 * motorR ), CONFIG_colGraLt, 1 );
	cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x + COS145 * motorR, center.y - SIN145 * motorR ), CONFIG_colGraLt, 1 );
	cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * motorR ), CONFIG_colGraLt, 1 );


	// /// Lines connecting motor pairs
	// cv::line( shared->Display.matFrameOverlay, cv::Point2i( center.x + COS35 * ( shared->Controller.commandedPercentage.x * 60.0f ), center.y - SIN35 * ( shared->Controller.commandedPercentage.x * 60.0f ) ), cv::Point2i( center.x, center.y - SIN270 * ( shared->Controller.commandedPercentage.z * 60.0f ) ),
	// 		  CONFIG_colRedMd, 1 );
	// cv::line( shared->Display.matFrameOverlay, cv::Point2i( center.x + COS145 * ( shared->Controller.commandedPercentage.y * 60.0f ), center.y - SIN145 * ( shared->Controller.commandedPercentage.y * 60.0f ) ), cv::Point2i( center.x, center.y - SIN270 * ( shared->Controller.commandedPercentage.z * 60.0f ) ),
	// 		  CONFIG_colRedMd, 1 );
	// cv::line( shared->Display.matFrameOverlay, cv::Point2i( center.x + COS35 * ( shared->Controller.commandedPercentage.x * 60.0f ), center.y - SIN35 * ( shared->Controller.commandedPercentage.x * 60.0f ) ),
	// 		  cv::Point2i( center.x + COS145 * ( shared->Controller.commandedPercentage.y * 60.0f ), center.y - SIN145 * ( shared->Controller.commandedPercentage.y * 60.0f ) ), CONFIG_colRedMd, 1 );
	// Active lines
	cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x + std::clamp( float( COS35 * ( shared->Controller.commandedPercentageABC.x * motorR ) ), 0.0f, motorR ), center.y - std::clamp( float( SIN35 * ( shared->Controller.commandedPercentageABC.x * motorR ) ), 0.0f, motorR ) ),
			  CONFIG_colGreDk, 10 );
	cv::line( shared->Display.matFrameOverlay, center,
			  cv::Point2i( center.x + std::clamp( float( COS145 * ( shared->Controller.commandedPercentageABC.y * motorR ) ), -60.0f, motorR ), center.y - std::clamp( float( SIN145 * ( shared->Controller.commandedPercentageABC.y * motorR ) ), 0.0f, motorR ) ), CONFIG_colGreDk, 10 );
	cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * ( shared->Controller.commandedPercentageABC.z * motorR ) ), CONFIG_colGreDk, 10 );

	// Teensy response lines
	if ( shared->Amplifier.isAmplifierActive ) {
		cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x + COS35 * ( shared->Amplifier.measuredPwmPercentA * motorR ), center.y - SIN35 * ( shared->Amplifier.measuredPwmPercentA * motorR ) ), CONFIG_colBluMd, 4 );
		cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x + COS145 * ( shared->Amplifier.measuredPwmPercentB * motorR ), center.y - SIN145 * ( shared->Amplifier.measuredPwmPercentB * motorR ) ), CONFIG_colBluMd, 4 );
		cv::line( shared->Display.matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * ( shared->Amplifier.measuredPwmPercentC * motorR ) ), CONFIG_colBluMd, 4 );
	}

	// Axis labels
	cv::putText( shared->Display.matFrameOverlay, "A", cv::Point2i( center.x + 58, center.y - 25 ), cv::FONT_HERSHEY_SIMPLEX, 0.5f, CONFIG_colRedLt, 1 );
	cv::putText( shared->Display.matFrameOverlay, "B", cv::Point2i( center.x - 67, center.y - 25 ), cv::FONT_HERSHEY_SIMPLEX, 0.5f, CONFIG_colGreLt, 1 );
	cv::putText( shared->Display.matFrameOverlay, "C", cv::Point2i( center.x + 10, center.y + 70 ), cv::FONT_HERSHEY_SIMPLEX, 0.5f, CONFIG_colBluLt, 1 );

	// Motor output circles
	cv::circle( shared->Display.matFrameOverlay, center, motorR, ( shared->Amplifier.isAmplifierActive ? CONFIG_colGreDk : CONFIG_colGraDk ), 2 );
	cv::circle( shared->Display.matFrameOverlay, center, ( shared->Amplifier.isAmplifierActive ? 6 : 2 ), ( shared->Amplifier.isAmplifierActive ? CONFIG_colGreDk : CONFIG_colGraDk ), -1 );


	// Encoder output block
	DrawCell( "", "AI9", 2, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "", "AM9", 2, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	cv::Point2i encoderB( 1120, 1332 );
	cv::Point2i encoderC( 1184, 1332 );
	cv::Point2i encoderA( 1248, 1332 );
	uint8_t		radEnc = 14;
	// Angle lines
	cv::line( shared->Display.matFrameOverlay, encoderA, cv::Point2i( encoderA.x + cos( shared->Amplifier.encoderMeasuredDegA * DEG2RAD * -1 ) * radEnc, encoderA.y + sin( shared->Amplifier.encoderMeasuredDegA * DEG2RAD * -1 ) * radEnc ), CONFIG_colRedLt, 2 );
	cv::line( shared->Display.matFrameOverlay, encoderB, cv::Point2i( encoderB.x + cos( shared->Amplifier.encoderMeasuredDegB * DEG2RAD ) * radEnc, encoderB.y + sin( shared->Amplifier.encoderMeasuredDegB * DEG2RAD ) * radEnc ), CONFIG_colGreLt, 2 );
	cv::line( shared->Display.matFrameOverlay, encoderC, cv::Point2i( encoderC.x + cos( shared->Amplifier.encoderMeasuredDegC * DEG2RAD ) * radEnc, encoderC.y + sin( shared->Amplifier.encoderMeasuredDegC * DEG2RAD ) * radEnc ), CONFIG_colBluLt, 2 );
	cv::circle( shared->Display.matFrameOverlay, encoderA, 18, CONFIG_colGraDk, 2 );
	cv::circle( shared->Display.matFrameOverlay, encoderB, 18, CONFIG_colGraDk, 2 );
	cv::circle( shared->Display.matFrameOverlay, encoderC, 18, CONFIG_colGraDk, 2 );



	// Task
	DrawCell( "TASK MONITOR", "AO1", 10, 1, fontTitle, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( "Name", "AO2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->Task.isRunning ? shared->Task.name : "No task running", "AQ2", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "User ID", "AU2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( shared->Task.userID ), "AW2", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Time", "AO3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->Task.runningTime, 2, 3 ), "AQ3", 3, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	DrawCell( "Touchscreen", "AO8", 4, 1, fontHeader, CONFIG_colWhite, shared->Touchscreen.isTouched ? CONFIG_colGreBk : CONFIG_colGraBk, true );
	DrawCell( "x", "AS8", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( shared->Touchscreen.positionTouched.x ), "AT8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "y", "AV8", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( shared->Touchscreen.positionTouched.y ), "AW8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Status block
	DrawCell( shared->Display.statusString, "AO9", 10, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Section borders
	DrawCellBorder( "A1", 8, 8, 2, CONFIG_colWhite );	   // Telemetry
	DrawCellBorder( "I1", 17, 7, 2, CONFIG_colWhite );	   // Controller
	DrawCellBorder( "Z1", 15, 10, 2, CONFIG_colWhite );	   // Amplifier
	DrawCellBorder( "AO1", 10, 8, 2, CONFIG_colWhite );	   // Task
	DrawCellBorder( "A9", 25, 2, 2, CONFIG_colWhite );	   // System status text
	DrawCellBorder( "I8", 2, 1, 2, CONFIG_colWhite );	   // System flags: Limits
	DrawCellBorder( "P8", 6, 1, 2, CONFIG_colWhite );	   // System flags: Serial
	DrawCellBorder( "AO9", 10, 2, 2, CONFIG_colWhite );	   // Serial packets

	// Line on the right side to satisfy my OCD
	cv::line( shared->Display.matFrameOverlay, cv::Point2i( 1598, CONFIG_PANEL_HEIGHT ), cv::Point2i( 1598, 1360 ), CONFIG_colWhite, 1 );
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
		r0 = ( CONFIG_PANEL_HEIGHT + 1 ) + ( ( std::stoi( cell0.substr( 1 ) ) - 1 ) * HEIGHT - 1 );
		rH = height * HEIGHT;
		cW = std::floor( width * WIDTH );
	} else if ( std::isalpha( cell0[0] ) && std::isalpha( cell0[1] ) ) {	// Two letters)
		c0 = ( 26 + ( cell0[1] - 'A' ) ) * WIDTH;
		r0 = ( CONFIG_PANEL_HEIGHT + 1 ) + ( ( std::stoi( cell0.substr( 2 ) ) - 1 ) * HEIGHT - 1 );
		rH = height * HEIGHT;
		cW = std::floor( width * WIDTH );
	}


	// Draw cell frame
	cv::rectangle( shared->Display.matFrameOverlay, cv::Rect( c0, r0, cW, rH ), fillColor, -1 );
	cv::rectangle( shared->Display.matFrameOverlay, cv::Rect( c0, r0 - 1, cW + 1, rH + 1 ), CONFIG_colWhite, 1 );

	// Calculate text dimensions
	if ( sz == fontBody ) {
		textSize = cv::getTextSize( str, cv::FONT_HERSHEY_SIMPLEX, sz, 1, 0 );
	} else {
		textSize = cv::getTextSize( str, cv::FONT_HERSHEY_DUPLEX, sz, 1, 0 );
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
	if ( sz == fontBody ) {
		cv::putText( shared->Display.matFrameOverlay, str, cv::Point( textX, textY ), cv::FONT_HERSHEY_SIMPLEX, sz, textColor, 1 );
	} else {
		cv::putText( shared->Display.matFrameOverlay, str, cv::Point( textX, textY ), cv::FONT_HERSHEY_DUPLEX, sz, textColor, 1 );
	}
}



void DisplayClass::DrawCellBorder( std::string cell0, short width, short height, uint8_t thickness, cv::Scalar color ) {

	// Check if using double letters (e.g., AA, AB)
	if ( std::isalpha( cell0[0] ) && !std::isalpha( cell0[1] ) ) {	  // Only one letter
		c0 = ( cell0[0] - 'A' ) * WIDTH;
		r0 = ( CONFIG_PANEL_HEIGHT + 1 ) + ( ( std::stoi( cell0.substr( 1 ) ) - 1 ) * HEIGHT - 1 );
		rH = height * HEIGHT;
		cW = std::floor( width * WIDTH );
	} else if ( std::isalpha( cell0[0] ) && std::isalpha( cell0[1] ) ) {	// Two letters)
		c0 = ( 26 + ( cell0[1] - 'A' ) ) * WIDTH;
		r0 = ( CONFIG_PANEL_HEIGHT + 1 ) + ( ( std::stoi( cell0.substr( 2 ) ) - 1 ) * HEIGHT - 1 );
		rH = height * HEIGHT;
		cW = std::floor( width * WIDTH );
	}


	// Draw cell frame
	cv::rectangle( shared->Display.matFrameOverlay, cv::Rect( c0, r0 - 1, cW + 1, rH + 1 ), CONFIG_colWhite, thickness );
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


	DrawKeyCell( "Exit", "A1", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "SetActive1", "A2", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "SetActive2", "A3", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "SetActive3", "A4", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "SetActive4", "A5", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "SetActive5", "A6", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "SetActiveNone", "A7", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "AmplifierToggle", "A8", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "SerialToggle", "A9", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "DirSelect_Abduction", "A10", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "DirSelect_Adduction", "A11", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "DirSelect_Flexion", "A12", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "DirSelect_Extension", "A13", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "GainSelect_Proportional", "A14", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "GainSelect_Integral", "A15", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "GainSelect_Derivative", "A16", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "GainsZero", "A17", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "TenSelect_A", "A18", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "TenSelect_B", "A19", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "TenSelect_C", "A20", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "Increment", "A21", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "Decrement", "A22", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "EncoderZero", "A23", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "EncoderSet", "A24", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "EncoderSetLimit", "A25", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "FittsStart", "A26", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "FittsStop", "A27", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );



	DrawKeyCell( "Esc", "F1", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "1", "F2", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "2", "F3", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "3", "F4", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "4", "F5", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "5", "F6", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "`", "F7", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "a", "F8", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "s", "F9", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "n4", "F10", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "n6", "F11", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "n2", "F12", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "n8", "F13", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "p", "F14", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "i", "F15", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "d", "F16", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "n5", "F17", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "n=", "F18", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "n/", "F19", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "n*", "F20", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "n+", "F21", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "n-", "F22", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "z", "F23", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "x", "F24", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "c", "F25", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "f", "F26", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "g", "F27", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );



	// Instructions


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
	// for ( const auto& pt : cubeCorners ) {
	// 	ProjectedCorners.push_back( ProjectIsometric( cv::Point3f( pt.z, pt.y, pt.x ) ) );	  // Swap x <-> z
	// }

	// Create window
	cv::namedWindow( winVisualizer, cv::WINDOW_AUTOSIZE );
	cv::moveWindow( winVisualizer, 3440 - CONFIG_DIS_WIDTH - CONFIG_DIS_VIZ_WIDTH - 6 - CONFIG_DIS_KEY_WIDTH, 0 );
	cv::imshow( winVisualizer, matVisualizer );
}


// Working visualizer
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

	uint8_t markerSize = 20;
	// Draw target square
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, -markerSize, markerSize ) ), ProjectIsometric( cv::Point3i( 0, markerSize, markerSize ) ), CONFIG_colGraLt, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, -markerSize, -markerSize ) ), ProjectIsometric( cv::Point3i( 0, markerSize, -markerSize ) ), CONFIG_colGraLt, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, markerSize, -markerSize ) ), ProjectIsometric( cv::Point3i( 0, markerSize, markerSize ) ), CONFIG_colGraLt, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, -markerSize, -markerSize ) ), ProjectIsometric( cv::Point3i( 0, -markerSize, markerSize ) ), CONFIG_colGraLt, 1 );

	// X axis
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, -vizLimXY, 0 ) ), ProjectIsometric( cv::Point3i( 0, vizLimXY, 0 ) ), CONFIG_colRedLt, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, vizLimXY, 0 ) ), ProjectIsometric( cv::Point3i( vizLimZ, vizLimXY, 0 ) ), CONFIG_colRedLt, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, -vizLimXY, 0 ) ), ProjectIsometric( cv::Point3i( 1000, -vizLimXY, 0 ) ), CONFIG_colRedLt, 1 );

	// Y axis
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, 0, -vizLimXY ) ), ProjectIsometric( cv::Point3i( 0, 0, vizLimXY ) ), CONFIG_colGreLt, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, 0, vizLimXY ) ), ProjectIsometric( cv::Point3i( 1000, 0, vizLimXY ) ), CONFIG_colGreLt, 1 );

	// Calculate camera + marker positions
	cv::Point3i p3D		= cv::Point3i( shared->Telemetry.positionFilteredNewMM.x, shared->Telemetry.positionFilteredNewMM.y, shared->Telemetry.positionFilteredNewMM.z );
	cv::Point3i p3DInv	= cv::Point3i( shared->Telemetry.positionFilteredNewMM.z, -shared->Telemetry.positionFilteredNewMM.y, shared->Telemetry.positionFilteredNewMM.x );
	int			ptSizeX = int( float( ( 1000.0 - ( p3D.x + vizLimXY ) ) / 1000.0 ) * 10.0 );
	int			ptSizeY = int( float( ( 1000.0 - ( -1 * p3D.y + vizLimXY ) ) / 1000.0 ) * 10.0 );
	int			ptSizeZ = int( float( ( 1000.0 - p3D.z ) / 1000.0 ) * 10.0 );

	// Line from marker to target
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, 0, 0 ) ), ProjectIsometric( p3DInv ), CONFIG_colCyaMd, 2 );

	// Moving local axis
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( p3DInv.x, p3DInv.y, vizLimXY ) ), ProjectIsometric( p3DInv ), CONFIG_colGreMd, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( p3DInv.x, vizLimXY, p3DInv.z ) ), ProjectIsometric( p3DInv ), CONFIG_colRedMd, 1 );
	cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, p3DInv.y, p3DInv.z ) ), ProjectIsometric( p3DInv ), CONFIG_colBluMd, 1 );



	// Create color gradient
	float zClamped = std::clamp( shared->Telemetry.positionFilteredNewMM.z, 0.0f, 1000.0f );
	// float intensity = 128.0f * ( zClamped / 1000.0f );

	// Add current point to trail
	if ( trailCounter++ >= trailInterval ) {
		trailPoints.push_back( p3DInv );
		// trailColor.push_back( ( int )intensity );
		trailCounter = 0;

		// Limit trail length
		if ( trailPoints.size() > trailLimit ) {
			trailPoints.erase( trailPoints.begin() );
			// trailColor.erase( trailColor.begin() );
		}
	}

	// Draw trail points
	for ( int i = 0; i < trailPoints.size(); i++ ) {

		float ratio		= static_cast<float>( i ) / static_cast<float>( trailPoints.size() );
		int	  intensity = static_cast<int>( 191 * ( 1.0f - ratio ) );

		// Z trail
		cv::circle( matVisualizer, ProjectIsometric( cv::Point3i( 0, trailPoints.at( i ).y, trailPoints.at( i ).z ) ), 2, cv::Scalar( 255, 64 + intensity, 64 + intensity ), -1 );

		// Y trail
		cv::circle( matVisualizer, ProjectIsometric( cv::Point3i( trailPoints.at( i ).x, trailPoints.at( i ).y, vizLimXY ) ), 2, cv::Scalar( 64 + intensity, 255, 64 + intensity ), -1 );

		// X trail
		cv::circle( matVisualizer, ProjectIsometric( cv::Point3f( trailPoints.at( i ).x, vizLimXY, trailPoints.at( i ).z ) ), 2, cv::Scalar( 64 + intensity, 64 + intensity, 255 ), -1 );
	}

	// Shadow on wall
	cv::circle( matVisualizer, ProjectIsometric( cv::Point3i( p3DInv.x, p3DInv.y, vizLimXY ) ), ptSizeX, CONFIG_colGreLt, -1 );	   // Shadow on YZ axis (right)
	cv::circle( matVisualizer, ProjectIsometric( cv::Point3i( 0, p3DInv.y, p3DInv.z ) ), ptSizeZ, CONFIG_colBluWt, -1 );		   // Shadow on XY plane (bottom)
	cv::circle( matVisualizer, ProjectIsometric( cv::Point3i( p3DInv.x, vizLimXY, p3DInv.z ) ), ptSizeY, CONFIG_colRedLt, -1 );	   // Shadow on XZ (screen)

	// // Draw marker (last to place over other elements)
	cv::circle( matVisualizer, ProjectIsometric( p3DInv ), 10, CONFIG_colWhite, -1 );
	cv::circle( matVisualizer, ProjectIsometric( p3DInv ), 10, CONFIG_colBlack, 1 );


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



void DisplayClass::ShowAngle() {

	// Create window
	cv::namedWindow( winAngle, cv::WINDOW_AUTOSIZE );
	cv::moveWindow( winAngle, 3440 - CONFIG_DIS_WIDTH - CONFIG_DIS_VIZ_WIDTH - 6 - CONFIG_DIS_KEY_WIDTH, 0 );
	cv::imshow( winAngle, matAngles );
}


void DisplayClass::UpdateAngle() {

	matAngles = CONFIG_colWhite;

	// Centerpoint
	cv::Point2i midPt = cv::Point2i( CONFIG_DIS_ANGLE_WIDTH / 2, CONFIG_DIS_ANGLE_HEIGHT / 2 + 200 );

	// Draw middle line
	cv::line( matAngles, cv::Point2i( 0, midPt.y ), cv::Point2i( CONFIG_DIS_ANGLE_WIDTH, midPt.y ), CONFIG_colGraLt, 2 );
	// Draw circle
	cv::ellipse( matAngles, midPt, cv::Size( 400, 400 ), 0, 180, 360, CONFIG_colGraLt, 2 );

	// Radius
	int radius = 400;

	// Target line
	cv::Point2i endPtDesired( int( cos( ( shared->angleDesired + 90 ) * DEG2RAD ) * radius ), int( sin( ( shared->angleDesired + 270 ) * DEG2RAD ) * radius ) );
	cv::line( matAngles, midPt, midPt + endPtDesired, CONFIG_colGreWt, 10 );

	// Velocity Line
	// cv::Point2i endPtVel( int( cos( ( shared->angleFiltered + shared->angleVelocity + 90 ) * DEG2RAD ) * radius ), int( sin( ( shared->angleFiltered + shared->angleVelocity + 270 ) * DEG2RAD ) * radius ) );
	cv::Point2i endPtVel( int( shared->Telemetry.velocityFilteredNewMM.x * MM2PX ), int( shared->Telemetry.velocityFilteredNewMM.x * MM2PX ) );
	cv::line( matAngles, midPt, midPt + endPtVel, CONFIG_colBluWt, 2 );



	// Measured angle
	cv::Point2i endPt( int( cos( ( shared->angleFiltered + 90 ) * DEG2RAD ) * radius ), int( sin( ( shared->angleFiltered + 270 ) * DEG2RAD ) * radius ) );
	cv::line( matAngles, midPt, midPt + endPt, CONFIG_colBluLt, 3 );

	// Encoder center
	cv::circle( matAngles, midPt, 10, CONFIG_colGraDk, -1 );

	// Text
	cv::putText( matAngles, "Desired [deg]: ", cv::Point2i( 20, 40 ), cv::FONT_HERSHEY_DUPLEX, 1.0, CONFIG_colBlack, 1 );
	cv::putText( matAngles, shared->FormatDecimal( shared->angleDesired, 2, 2 ), cv::Point2i( 300, 40 ), cv::FONT_HERSHEY_DUPLEX, 1.0, CONFIG_colBlack, 1 );
	cv::putText( matAngles, "Measured [deg]: ", cv::Point2i( 20, 80 ), cv::FONT_HERSHEY_DUPLEX, 1.0, CONFIG_colBlack, 1 );
	cv::putText( matAngles, shared->FormatDecimal( shared->angleFiltered, 2, 2 ), cv::Point2i( 300, 80 ), cv::FONT_HERSHEY_DUPLEX, 1.0, CONFIG_colBlack, 1 );
	cv::putText( matAngles, "Error [deg]: ", cv::Point2i( 20, 120 ), cv::FONT_HERSHEY_DUPLEX, 1.0, CONFIG_colBlack, 1 );
	cv::putText( matAngles, shared->FormatDecimal( shared->angleFiltered - shared->angleDesired, 2, 2 ), cv::Point2i( 300, 120 ), cv::FONT_HERSHEY_DUPLEX, 1.0, CONFIG_colBlack, 1 );

	if ( shared->Touchscreen.isTouched == 1 ) {

		// Save image
		if ( shared->Logging.isLoggingEnabled ) {
			std::string imageFilename	 = "/home/tom/Code/nuring/logging/" + shared->loggingFilename + ".png";
			shared->Display.statusString = "Saving file " + imageFilename;
			cv::imwrite( imageFilename, matAngles );
			std::cout << "FittsClass:  Image saved at " << imageFilename << "\n";
		}
	}

	cv::imshow( winAngle, matAngles );
}


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

	// Launch angle if requested
	if ( shared->angleEnabled ) {

		// Check if window loaded
		if ( shared->angleLoaded ) {
			UpdateAngle();
		} else {
			ShowAngle();
			shared->angleLoaded = true;
		}
	} else {

		// // Check if window is loaded before destroying
		// if ( shared->angleLoaded ) {
		// 	cv::destroyWindow( winAngle );
		// 	shared->angleLoaded = false;
		// }
	}
}
