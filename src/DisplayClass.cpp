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
		fontBody	   = 0.35f;
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
	cv::circle( shared->matFrameOverlay, CONFIG_CAM_CENTER, CONFIG_DET_RADIUS, ( shared->FLAG_TARGET_MARKER_FOUND ? CONFIG_colGreMd : CONFIG_colRedDk ), 1 );
	cv::line( shared->matFrameOverlay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_HEIGHT ), ( shared->FLAG_TARGET_MARKER_FOUND ? CONFIG_colGreMd : CONFIG_colRedDk ), 1 );
	cv::line( shared->matFrameOverlay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( CONFIG_CAM_WIDTH, CONFIG_CAM_PRINCIPAL_Y ), ( shared->FLAG_TARGET_MARKER_FOUND ? CONFIG_colGreMd : CONFIG_colRedDk ), 1 );

	// Draw motor axis
	cv::line( shared->matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS35 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN35 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );
	cv::line( shared->matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS145 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN145 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );
	cv::line( shared->matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_PRINCIPAL_X + COS270 * CONFIG_DET_RADIUS, CONFIG_CAM_PRINCIPAL_Y - SIN270 * CONFIG_DET_RADIUS ), CONFIG_colYelMd, 1 );



	// Draw information for finger marker if present
	if ( shared->FLAG_FINGER_MARKER_FOUND ) {

		// Draw border and axis elements of finger marker
		cv::polylines( shared->matFrameOverlay, shared->fingerMarkerCorners, true, CONFIG_colCyaMd, 2 );
		// cv::drawFrameAxes( shared->matFrameUndistorted, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, shared->fingerMarkerRotationVector, shared->fingerMarkerTranslationVector, CONFIG_MEDIUM_MARKER_WIDTH, 2 );
	}

	// Draw information for target marker if present
	if ( shared->FLAG_TARGET_MARKER_FOUND ) {

		//Draw vector to center of target
		cv::line( shared->matFrameOverlay, CONFIG_CAM_CENTER, shared->targetMarkerScreenPosition, CONFIG_colCyaMd, 2 );

		// Draw border and axis elements of target marker
		cv::polylines( shared->matFrameOverlay, shared->targetMarkerCorners, true, CONFIG_colGreMd, 2 );
		// cv::drawFrameAxes( shared->matFrameUndistorted, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, shared->targetMarkerRotationVector, shared->targetMarkerTranslationVector, CONFIG_LARGE_MARKER_WIDTH, 15 );

		// Draw velocity
		// cv::line( shared->matFrameOverlay, shared->targetMarkerScreenPosition, cv::Point2i( shared->targetMarkerScreenPosition.x + shared->targetMarkerVelocity3dNew.x * MM2PX, shared->targetMarkerScreenPosition.y - shared->targetMarkerVelocity3dNew.y * MM2PX ), CONFIG_colMagLt, 2 );
		cv::line( shared->matFrameOverlay, CONFIG_CAM_CENTER, cv::Point2i( CONFIG_CAM_CENTER.x - shared->targetMarkerVelocity3dNew.x * MM2PX / 2.0f, CONFIG_CAM_CENTER.y + shared->targetMarkerVelocity3dNew.y * MM2PX / 2.0f ), CONFIG_colMagLt, 2 );

		// Graph velocity on screen
		cv::line( shared->matFrameOverlay, cv::Point2i( shared->targetMarkerScreenPosition.x + shared->targetMarkerVelocity3dNew.x, CONFIG_CAM_CENTER.y + 100 ), cv::Point2i( shared->targetMarkerScreenPosition.x - shared->targetMarkerVelocity3dNew.x, CONFIG_CAM_CENTER.y + 100 ), CONFIG_colRedMd, 2 );
	}



	// Draw calibrated marker
	if ( shared->calibrationComplete ) {
		// int newX = shared->calibrationOffsetPX.x - CONFIG_TOUCHSCREEN_CENTER.x + CONFIG_CAM_PRINCIPAL_X;
		// int newY = shared->calibrationOffsetPX.y - CONFIG_TOUCHSCREEN_CENTER.y + CONFIG_CAM_PRINCIPAL_Y - 20 * MM2PX;
		// cv::circle( shared->matFrameOverlay, cv::Point2i( newX, newY ), 10 * MM2PX, CONFIG_colGreLt, 2 );
		// cv::circle( shared->matFrameOverlay, shared->, 10 * MM2PX, CONFIG_colGreLt, 2 );
	}

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

	// Section border
	DrawCell( "", "A1", 50, 7, 0, CONFIG_colWhite, CONFIG_colBlack, false );


	// Telemetry
	// Position
	DrawCell( "Telemetry", "A1", 9, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( std::to_string( shared->targetMarkerActiveID ), "A2", 1, 1, fontBody * 3, CONFIG_colWhite, ( shared->FLAG_TARGET_MARKER_FOUND ? CONFIG_colGreBk : CONFIG_colBlack ), true );
	DrawCell( "0", "A3", 1, 1, fontBody * 3, CONFIG_colWhite, ( shared->FLAG_FINGER_MARKER_FOUND ? CONFIG_colGreBk : CONFIG_colBlack ), true );
	DrawCell( "x", "A4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "y", "A5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "z", "A6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "R", "A7", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Position", "B2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[mm]", "B3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->targetMarkerPosition3dNew.x ) ), "B4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->targetMarkerPosition3dNew.y ) ), "B5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->targetMarkerPosition3dNew.z ) ), "B6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->GetNorm3D( shared->targetMarkerPosition3dNew ) ) ), "B7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// Velocity
	DrawCell( "Velocity", "D2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[mm/s]", "D3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->targetMarkerVelocity3dNew.x ) ), "D4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->targetMarkerVelocity3dNew.y ) ), "D5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->targetMarkerVelocity3dNew.z ) ), "D6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->GetNorm3D( shared->targetMarkerVelocity3dNew ) ) ), "D7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// Integral
	DrawCell( "Integral E", "F2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[mm]", "F3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->targetMarkerIntegralError.x, 3, 1 ), "F4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->targetMarkerIntegralError.y, 3, 1 ), "F5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// Angle
	DrawCell( "Theta E", "H2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[deg]", "H3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->targetMarkerAngleNew.x * RAD2DEG ) ), "H4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->targetMarkerAngleNew.y * RAD2DEG ) ), "H5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "Theta dE", "H2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "[deg/s]", "H3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( std::to_string( int( shared->targetMarkerAnglularVelocityNew.x * RAD2DEG ) ), "H4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( int( shared->targetMarkerAnglularVelocityNew.y * RAD2DEG ) ), "H5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// Frequency
	DrawCell( "Frequency [Hz]", "F6", 4, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( std::to_string( int( shared->timingFrequency ) ), "F7", 4, 1, fontBody * 1.5, CONFIG_colWhite, ( shared->timingFrequency > 60 ? CONFIG_colGreBk : CONFIG_colRedBk ), true );

	// Controller
	DrawCell( "Controller", "J1", 12, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( "Kp", "J2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[?]", "J3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->controllerKp.x, 1, 1 ), "J4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerKp.y, 1, 1 ), "J5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Ki", "K2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[?]", "K3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->controllerKi.x, 2, 1 ), "K4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerKi.y, 2, 1 ), "K5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Kd", "L2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[?]", "L3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->controllerKd.x, 2, 1 ), "L4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerKd.y, 2, 1 ), "L5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	DrawCell( "Kp*E", "M2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[?*mm]", "M3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->controllerProportinalTerm.x, 1, 1 ), "M4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerProportinalTerm.y, 1, 1 ), "M5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Ki*iE", "O2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[?*mm]", "O3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->controllerIntegralTerm.x, 1, 1 ), "O4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerIntegralTerm.y, 1, 1 ), "O5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Kd*dE", "Q2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[?*mm/s]", "Q3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->controllerDerivativeTerm.x, 1, 1 ), "Q4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerDerivativeTerm.y, 1, 1 ), "Q5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Total", "S2", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[?]", "S3", 3, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->controllerTotalTerm.x, 1, 1 ), "S4", 3, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerTotalTerm.y, 1, 1 ), "S5", 3, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// DrawCell( "Kd*dE", "R2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "[mN*m]", "R3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( shared->FormatDecimal( shared->controllerDerivativeTerm.x, 2, 2 ), "R4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( shared->FormatDecimal( shared->controllerDerivativeTerm.y, 2, 2 ), "R5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( "Total", "T2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( "[mN*m]", "T3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	// DrawCell( shared->FormatDecimal( shared->controllerTotalTerm.x, 2, 2 ), "T4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( shared->FormatDecimal( shared->controllerTotalTerm.y, 2, 2 ), "T5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Serial Interface
	DrawCell( "Serial Out", "J6", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( shared->FLAG_SERIAL0_ENABLED ? "Enabled" : "Disabled", "M6", 3, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_SERIAL0_ENABLED ? CONFIG_colGreBk : CONFIG_colRedBk ), true );
	DrawCell( shared->FLAG_SERIAL0_ENABLED ? shared->serialPacket0.substr( 0, shared->serialPacket0.length() - 1 ) : "---", "J7", 6, 1, fontBody * 1.5, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Serial In", "P6", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( shared->FLAG_SERIAL1_ENABLED ? "Enabled" : "Disabled", "S6", 3, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_SERIAL1_ENABLED ? CONFIG_colGreBk : CONFIG_colRedBk ), true );
	DrawCell( shared->FLAG_SERIAL1_ENABLED ? shared->serialPacket1.substr( 0, shared->serialPacket1.length() - 1 ) : "---", "P7", 6, 1, fontBody * 1.5, CONFIG_colWhite, CONFIG_colBlack, true );

	// Amplifier
	DrawCell( "Amplifier", "V1", 13, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( ( shared->FLAG_AMPLIFIERS_ENABLED && shared->FLAG_TEENSY_AMPLIFIER_ENABLED ) ? "ON" : "OFF", "V2", 1, 2, fontBody, CONFIG_colWhite, ( shared->FLAG_AMPLIFIERS_ENABLED ? CONFIG_colGreBk : CONFIG_colRedBk ), true );
	DrawCell( "A", "V4", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "B", "V5", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "C", "V6", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Tension", "W2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[%I]", "W3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->controllerTension.x, 2, 2 ), "W4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerTension.y, 2, 2 ), "W5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerTension.z, 2, 2 ), "W6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Torque", "Y2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[mN*m]", "Y3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->controllerTorqueABC.x, 2, 2 ), "Y4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerTorqueABC.y, 2, 2 ), "Y5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerTorqueABC.z, 2, 2 ), "Y6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "Current", "AA2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[A]", "AA3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( shared->FormatDecimal( shared->controllerCurrent.x, 2, 2 ), "AA4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerCurrent.y, 2, 2 ), "AA5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( shared->FormatDecimal( shared->controllerCurrent.z, 2, 2 ), "AA6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( "PWM", "AC2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "[duty]", "AC3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->controllerPWM.x ) ), "AC4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->controllerPWM.y ) ), "AC5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( shared->controllerPWM.z ) ), "AC6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Teensy
	DrawCell( "Teensy Status", "V7", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( shared->FLAG_SERIAL0_ENABLED && shared->FLAG_TEENSY_SERIAL_RESPONDING ) ? "Serial Connected" : "Serial Disconnected", "Y7", 3, 1, fontBody, CONFIG_colWhite, ( ( shared->FLAG_SERIAL0_ENABLED && shared->FLAG_TEENSY_SERIAL_RESPONDING ) ? CONFIG_colGreBk : CONFIG_colRedDk ), true );
	DrawCell( ( shared->FLAG_TEENSY_AMPLIFIER_ENABLED ? "Amplifiers On" : "Amplifiers Off" ), "AB7", 3, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_TEENSY_AMPLIFIER_ENABLED ? CONFIG_colGreBk : CONFIG_colRedDk ), true );

	// Motor Output Block
	cv::Point2i center( 960 + 64, 1221 + 66 );
	cv::rectangle( shared->matFrameOverlay, cv::Rect2i( cv::Point2i( 960, 1221 ), cv::Point2i( 1089, 1354 ) ), CONFIG_colWhite, 1 );
	// Gray background
	cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS35 * 60, center.y - SIN35 * 60 ), CONFIG_colGraBk, 2 );
	cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS145 * 60, center.y - SIN145 * 60 ), CONFIG_colGraBk, 2 );
	cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * 60 ), CONFIG_colGraBk, 2 );
	// Teensy lines
	if ( shared->FLAG_TEENSY_AMPLIFIER_ENABLED && shared->FLAG_TEENSY_SERIAL_RESPONDING ) {
		cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS35 * ( ( shared->teensyMeasuredAmplfierOutput.x / 10.0 ) * 60.0f ), center.y - SIN35 * ( ( shared->teensyMeasuredAmplfierOutput.x / 10.0 ) * 60.0f ) ), CONFIG_colRedLt, 8 );
		cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS145 * ( ( shared->teensyMeasuredAmplfierOutput.y / 10.0 ) * 60.0f ), center.y - SIN145 * ( ( shared->teensyMeasuredAmplfierOutput.y / 10.0 ) * 60.0f ) ), CONFIG_colGreLt, 8 );
		cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * ( ( shared->teensyMeasuredAmplfierOutput.z / 10.0 ) * 60.0f ) ), CONFIG_colBluLt, 8 );
	}
	/// Lines connecting motor pairs
	cv::line( shared->matFrameOverlay, cv::Point2i( center.x + COS35 * ( shared->controllerPercentage.x * 60.0f ), center.y - SIN35 * ( shared->controllerPercentage.x * 60.0f ) ), cv::Point2i( center.x, center.y - SIN270 * ( shared->controllerPercentage.z * 60.0f ) ), CONFIG_colRedMd, 1 );
	cv::line( shared->matFrameOverlay, cv::Point2i( center.x + COS145 * ( shared->controllerPercentage.y * 60.0f ), center.y - SIN145 * ( shared->controllerPercentage.y * 60.0f ) ), cv::Point2i( center.x, center.y - SIN270 * ( shared->controllerPercentage.z * 60.0f ) ), CONFIG_colRedMd, 1 );
	cv::line( shared->matFrameOverlay, cv::Point2i( center.x + COS35 * ( shared->controllerPercentage.x * 60.0f ), center.y - SIN35 * ( shared->controllerPercentage.x * 60.0f ) ),
			  cv::Point2i( center.x + COS145 * ( shared->controllerPercentage.y * 60.0f ), center.y - SIN145 * ( shared->controllerPercentage.y * 60.0f ) ), CONFIG_colRedMd, 1 );
	// Active lines
	cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + std::clamp( float( COS35 * ( shared->controllerPercentage.x * 60.0f ) ), 0.0f, 60.0f ), center.y - std::clamp( float( SIN35 * ( shared->controllerPercentage.x * 60.0f ) ), 0.0f, 60.0f ) ), CONFIG_colRedMd, 2 );
	cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + std::clamp( float( COS145 * ( shared->controllerPercentage.y * 60.0f ) ), -60.0f, 0.0f ), center.y - std::clamp( float( SIN145 * ( shared->controllerPercentage.y * 60.0f ) ), 0.0f, 60.0f ) ), CONFIG_colGreDk, 2 );
	cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * ( shared->controllerPercentage.z * 60.0f ) ), CONFIG_colBluMd, 2 );
	// Motor output circles
	cv::circle( shared->matFrameOverlay, center, 60, ( shared->FLAG_AMPLIFIERS_ENABLED ? CONFIG_colGreDk : CONFIG_colGraDk ), 1 );
	cv::circle( shared->matFrameOverlay, center, ( shared->FLAG_AMPLIFIERS_ENABLED ? 6 : 2 ), ( shared->FLAG_AMPLIFIERS_ENABLED ? CONFIG_colGreDk : CONFIG_colGraDk ), -1 );

	// Calibration Block
	DrawCell( "Calibration", "AI1", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( "X [mm]", "AI2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->calibrationOffsetMM.x ) ), "AK2", 1, 1, fontBody, CONFIG_colWhite, ( shared->calibrationComplete ? CONFIG_colGreBk : CONFIG_colRedDk ), true );
	DrawCell( "Y [mm]", "AI3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->calibrationOffsetMM.y ) ), "AK3", 1, 1, fontBody, CONFIG_colWhite, ( shared->calibrationComplete ? CONFIG_colGreBk : CONFIG_colRedDk ), true );
	DrawCell( "Z [mm]", "AI4", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( int( shared->calibrationOffsetMM.z ) ), "AK4", 1, 1, fontBody, CONFIG_colWhite, ( shared->calibrationComplete ? CONFIG_colGreBk : CONFIG_colRedDk ), true );


	// // Status block
	DrawCell( "System Status", "AI5", 16, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraDk, true );
	DrawCell( shared->displayString, "AI6", 16, 2, fontBody * 1.5, CONFIG_colWhite, CONFIG_colBlack, true );
}

/**
 * Add text to the overlay
 */
// void DisplayClass::AddText() {

// 	// Local common variables
// 	bool TARGET = shared->FLAG_TARGET_FOUND;


// 	// Section border
// 	DrawCell( "", "A1", 40, 7, 0, CONFIG_colWhite, CONFIG_colBlack, false );

// 	// Active Marker Block
// 	DrawCell( "Active Marker", "A1", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	DrawCell( "1", "A2", 1, 1, fontBody, ( ( shared->targetActiveID == 1 && shared->FLAG_TARGET_FOUND ) ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->targetActiveID == 1 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
// 	DrawCell( "2", "B2", 1, 1, fontBody, ( ( shared->targetActiveID == 2 && shared->FLAG_TARGET_FOUND ) ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->targetActiveID == 2 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
// 	DrawCell( "3", "C2", 1, 1, fontBody, ( ( shared->targetActiveID == 3 && shared->FLAG_TARGET_FOUND ) ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->targetActiveID == 3 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
// 	DrawCell( "4", "D2", 1, 1, fontBody, ( ( shared->targetActiveID == 4 && shared->FLAG_TARGET_FOUND ) ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->targetActiveID == 4 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
// 	DrawCell( "5", "E2", 1, 1, fontBody, ( ( shared->targetActiveID == 5 && shared->FLAG_TARGET_FOUND ) ? CONFIG_colWhite : CONFIG_colGraDk ), ( shared->targetActiveID == 5 ? CONFIG_colGreDk : CONFIG_colBlack ), true );

// 	// // Position [mm] Block
// 	DrawCell( "Pos [mm]", "B3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	DrawCell( "x", "A4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	DrawCell( "y", "A5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	DrawCell( "z", "A6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	DrawCell( "R", "A7", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	DrawCell( std::to_string( int( shared->targetPosition3dNew.x ) ), "B4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	DrawCell( std::to_string( int( shared->targetPosition3dNew.y ) ), "B5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	DrawCell( std::to_string( int( shared->targetPosition3dNew.z ) ), "B6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorMagnitude3D ) ), "B7", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

// 	// // Velocity [mm/s] Block
// 	// DrawCell( "Vel [mm/s]", "D3", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorVel3D.x ) ), "D4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorVel3D.y ) ), "D5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].errorVel3D.z ) ), "D6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

// 	// // Controller
// 	// // Heading
// 	// DrawCell( "Controller", "F1", 18, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// // DrawCell( "F", "F2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// // DrawCell( "=", "G2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "Kp", "H2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "(", "I2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "ThD", "J2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "-", "K2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "ThM", "L2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( ")", "M2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "+", "N2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "Kd", "O2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "(", "P2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "ThVD", "Q2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "-", "R2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "ThVM", "S2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( ")", "T2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "=", "U2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "xxx", "V2", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// // X Values
// 	// DrawCell( "Fx", "F3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "=", "G3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->controllerKp.x ) ), "H3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "(", "I3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->controllerThetaDesired.x * RAD2DEG ) ), "J3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "-", "K3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].thetaMeasured.x * RAD2DEG ) ), "L3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( ")", "M3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "+", "N3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->controllerKd.x ) ), "O3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "(", "P3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->controllerThetaDotDesired.x * RAD2DEG ) ), "Q3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "-", "R3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].thetaDotMeasured.x * RAD2DEG ) ), "S3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( ")", "T3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "=", "U3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->controllerForceXY.x * RAD2DEG ) ), "V3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// // Y Values
// 	// DrawCell( "Fy", "F4", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "=", "G4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->controllerKp.y ) ), "H4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "(", "I4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->controllerThetaDesired.y * RAD2DEG ) ), "J4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "-", "K4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].thetaMeasured.y * RAD2DEG ) ), "L4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( ")", "M4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "+", "N4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->controllerKd.y ) ), "O4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "(", "P4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->controllerThetaDesired.y * RAD2DEG ) ), "Q4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "-", "R4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->arucoTags[shared->arucoActiveID].thetaDotMeasured.y * RAD2DEG ) ), "S4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( ")", "T4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "=", "U4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->controllerForceXY.y * RAD2DEG ) ), "V4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// // Motor
// 	// DrawCell( std::to_string( int( shared->controllerPercentage.x * 100 ) ) + "%", "F5", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "A", "G5", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "B", "F6", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );

// 	// DrawCell( "C", "F7", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );



// 	// DrawCell( "ThD", "H2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "ThM", "I2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );



// 	// DrawCell( "Fx", "F2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( std::to_string( ( shared->controllerKp.x ) ), "G2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "Fy", "F3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );

// 	// DrawCell( "Kp", "F2", 1, 2, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "x", "G2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "y", "G3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( std::to_string( ( shared->controllerKp.x ) ), "H2", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( ( shared->controllerKp.y ) ), "H3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "Kd", "J2", 1, 2, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "x", "K2", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "y", "K3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( std::to_string( ( shared->controllerKd.x ) ), "L2", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( ( shared->controllerKd.y ) ), "L3", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// Impedance
// 	//


// 	//
// 	// // Controller block
// 	// DrawCell( "Controller Settings", "K3", 8, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "K [N/mm]", "K4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "B [N*s/mm]", "K5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// // DrawCell( ( []( float value ) {
// 	// // 			  std::ostringstream out;
// 	// // 			  out << std::fixed << std::setprecision( 3 ) << value;
// 	// // 			  return out.str();
// 	// // 		  } )( shared->controllerK ),
// 	// // 		  "M4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// // DrawCell( ( []( float value ) {
// 	// // 			  std::ostringstream out;
// 	// // 			  out << std::fixed << std::setprecision( 3 ) << value;
// 	// // 			  return out.str();
// 	// // 		  } )( shared->controllerB ),
// 	// // 		  "M5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( "Fx [N]", "O4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "Fy [N]", "O5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( ( []( float value ) {
// 	// 			  std::ostringstream out;
// 	// 			  out << std::fixed << std::setprecision( 3 ) << value;
// 	// 			  return out.str();
// 	// 		  } )( shared->controllerFx ),
// 	// 		  "Q4", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( ( []( float value ) {
// 	// 			  std::ostringstream out;
// 	// 			  out << std::fixed << std::setprecision( 3 ) << value;
// 	// 			  return out.str();
// 	// 		  } )( shared->controllerFy ),
// 	// 		  "Q5", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


// 	// // Mouse output block // AB1
// 	// DrawCell( "Mouse", "X1", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "x", "X2", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "y", "Y2", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( std::to_string( shared->touchPosition.x + 3440 ), "X3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( shared->touchPosition.y + 32 ), "Y3", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( ( shared->touchDetected == 1 ? "Button 1" : "-" ), "X4", 2, 2, fontBody, CONFIG_colWhite, ( shared->touchDetected ? CONFIG_colGreDk : CONFIG_colBlack ), true );


// 	// // Calibration Blocks
// 	// DrawCell( "Calibration Status", "Z1", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( ( shared->calibrationComplete ? "Complete" : "Incomplete" ), "Z2", 3, 1, fontHeader, CONFIG_colWhite, ( shared->calibrationComplete ? CONFIG_colGreDk : CONFIG_colRedBk ), true );
// 	// DrawCell( " ", "Z3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "[px]", "AA3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "[mm]", "AB3", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "x", "Z4", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "y", "Z5", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "z", "Z6", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( "|R|", "Z7", 1, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.x * MM2PX ) ), "AA4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.y * MM2PX ) ), "AA5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.z * MM2PX ) ), "AA6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( cv::norm( shared->calibrationOffsetMM ) * MM2PX ) ), "AA7", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.x ) ), "AB4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.y ) ), "AB5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( shared->calibrationOffsetMM.z ) ), "AB6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// DrawCell( std::to_string( int( cv::norm( shared->calibrationOffsetMM ) ) ), "AB7", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

// 	// // Amplifier / frequency blocks
// 	// DrawCell( "Amplifiers", "AC1", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( ( shared->FLAG_AMPLIFIERS_READY ? "Online" : "Offline" ), "AC2", 2, 2, fontBody, CONFIG_colWhite, ( shared->FLAG_AMPLIFIERS_READY ? CONFIG_colGreDk : CONFIG_colRedBk ), true );
// 	// DrawCell( "Frequency", "AC4", 2, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( std::to_string( shared->timingFrequency ) + " Hz", "AC5", 2, 2, fontBody * 1.5, CONFIG_colWhite, CONFIG_colBlack, true );


// 	// // Serial blocks
// 	// DrawCell( "Serial0 (Outbound)", "AE1", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( ( shared->FLAG_SERIAL0_ENABLED ? "Online" : "Offline" ), "AH1", 3, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_SERIAL0_ENABLED ? CONFIG_colGreDk : CONFIG_colRedBk ), true );
// 	// DrawCell( shared->serialPacket0.substr( 0, shared->serialPacket0.size() - 1 ), "AE2", 6, 2, fontHeader * 1.6, CONFIG_colWhite, ( shared->serialPacket0 == "DX\n" ) ? CONFIG_colBlack : CONFIG_colGreDk, true );
// 	// DrawCell( "Serial1 (Inbound) ", "AE4", 3, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( ( shared->FLAG_SERIAL1_ENABLED ? "Online" : "Offline" ), "AH4", 3, 1, fontBody, CONFIG_colWhite, ( shared->FLAG_SERIAL0_ENABLED ? CONFIG_colGreDk : CONFIG_colRedBk ), true );
// 	// DrawCell( shared->serialPacket1.substr( 0, shared->serialPacket1.size() - 1 ), "AE5", 6, 2, fontHeader * 1.6, CONFIG_colWhite, ( shared->serialPacket0 == "DX\n" ) ? CONFIG_colBlack : CONFIG_colGreDk, true );


// 	// // Logging Block
// 	// DrawCell( "Logging", "AC7", 2, 1, fontHeader, CONFIG_colWhite, ( shared->FLAG_LOGGING_STARTED ) ? CONFIG_colGreDk : CONFIG_colGraBk, true );
// 	// // DrawCell( ( shared->FLAG_LOGGING_ON ? "Logging" : "Offline" ), "AM2", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
// 	// // DrawCell( "Filename", "AG6", 8, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( ( ( shared->FLAG_LOGGING_ENABLED && shared->FLAG_LOGGING_STARTED ) ? shared->loggingFilename : "DISABLED" ), "AE7", 6, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

// 	// // Motor Output Block
// 	// cv::Point2i center( 1440 + 80, 1222 + 66 );
// 	// DrawCell( "Motor Output", "AK1", 4, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// // cv::rectangle( shared->matFrameOverlay, cv::Rect2i( cv::Point2i( 1440, 1222 ), cv::Point2i( 1600, 1354 ) ), CONFIG_colVioMd, 1 );
// 	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS35 * 60, center.y - SIN35 * 60 ), CONFIG_colGraBk, 2 );
// 	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS145 * 60, center.y - SIN145 * 60 ), CONFIG_colGraBk, 2 );
// 	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * 60 ), CONFIG_colGraBk, 2 );

// 	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS35 * ( shared->controllerPercentage.x * 60.0f ), center.y - SIN35 * ( shared->controllerPercentage.x * 60.0f ) ), CONFIG_colRedMd, 2 );
// 	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x + COS145 * ( shared->controllerPercentage.y * 60.0f ), center.y - SIN145 * ( shared->controllerPercentage.y * 60.0f ) ), CONFIG_colRedMd, 2 );
// 	// cv::line( shared->matFrameOverlay, center, cv::Point2i( center.x, center.y - SIN270 * ( shared->controllerPercentage.z * 60.0f ) ), CONFIG_colRedMd, 2 );

// 	// /// Lines connecting motor pairs
// 	// cv::line( shared->matFrameOverlay, cv::Point2i( center.x + COS35 * ( shared->controllerPercentage.x * 60.0f ), center.y - SIN35 * ( shared->controllerPercentage.x * 60.0f ) ), cv::Point2i( center.x, center.y - SIN270 * ( shared->controllerPercentage.z * 60.0f ) ), CONFIG_colRedMd, 1 );
// 	// cv::line( shared->matFrameOverlay, cv::Point2i( center.x + COS145 * ( shared->controllerPercentage.y * 60.0f ), center.y - SIN145 * ( shared->controllerPercentage.y * 60.0f ) ), cv::Point2i( center.x, center.y - SIN270 * ( shared->controllerPercentage.z * 60.0f ) ), CONFIG_colRedMd, 1 );
// 	// cv::line( shared->matFrameOverlay, cv::Point2i( center.x + COS35 * ( shared->controllerPercentage.x * 60.0f ), center.y - SIN35 * ( shared->controllerPercentage.x * 60.0f ) ),
// 	// 		  cv::Point2i( center.x + COS145 * ( shared->controllerPercentage.y * 60.0f ), center.y - SIN145 * ( shared->controllerPercentage.y * 60.0f ) ), CONFIG_colRedMd, 1 );

// 	// // Motor output circles
// 	// cv::circle( shared->matFrameOverlay, center, 60, ( shared->FLAG_CONTROLLER_ACTIVE ? CONFIG_colGreDk : CONFIG_colGraDk ), 1 );
// 	// cv::circle( shared->matFrameOverlay, center, ( shared->FLAG_CONTROLLER_ACTIVE ? 6 : 2 ), ( shared->FLAG_CONTROLLER_ACTIVE ? CONFIG_colGreDk : CONFIG_colGraDk ), -1 );

// 	// // // Trial blocks
// 	// // Participant Block
// 	// DrawCell( "Participant ID", "AC1", 4, 2, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( std::to_string( shared->TASK_USER_ID ), "AC3", 4, 2, fontHeader, CONFIG_colWhite, ( shared->TASK_USER_ID == 100 ? CONFIG_colRedDk : CONFIG_colGreDk ), true );

// 	// // Task command
// 	// DrawCell( "Task Command", "AG6", 4, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( std::to_string( shared->TASK_COMMAND ), "AG7", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


// 	// cv::Scalar backColor = shared->TASK_NUMBER == 0 ? CONFIG_colGraBk : CONFIG_colBluDk;

// 	// // Task block
// 	// DrawCell( "Current Task", "K1", 5, 2, fontHeader * 1.5, CONFIG_colWhite, backColor, true );
// 	// std::string taskName;
// 	// if ( shared->TASK_NUMBER == 1 ) {
// 	// 	taskName = "1: Discrimination Task";
// 	// } else if ( shared->TASK_NUMBER == 2 ) {
// 	// 	taskName = "2: Fitts-Law Task";
// 	// } else if ( shared->TASK_NUMBER == 3 ) {
// 	// 	taskName = "3: TBD";
// 	// } else {
// 	// 	taskName = "0: None";
// 	// }
// 	// DrawCell( taskName, "P1", 13, 2, fontBody * 1.5, CONFIG_colWhite, backColor, true );

// 	// // Task timer
// 	// DrawCell( "Elapsed Time", "K3", 5, 1, fontBody, CONFIG_colWhite, backColor, true );
// 	// DrawCell( std::to_string( shared->timingTimestamp ), "K4", 5, 1, fontBody, CONFIG_colWhite, backColor, true );

// 	// // Status Block
// 	// DrawCell( "System:", "K6", 2, 2, fontHeader * 1.5, CONFIG_colWhite, CONFIG_colGraBk, true );
// 	// DrawCell( shared->displayString, "M6", 11, 2, fontBody * 1.5, CONFIG_colWhite, CONFIG_colBlack, false );
// }



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
	DrawKeyCell( "Amplifier enable (toggle)", "A3", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "a", "F3", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
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
	DrawKeyCell( "Decrease | increase Kp(x)", "A9", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "o  p", "F9", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Decrease | increase Kp(y)", "A10", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "[  ]", "F10", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Decrease | increase Kd(x)", "A11", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( "k  l", "F11", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawKeyCell( "Decrease | increase Kd(y)", "A12", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	DrawKeyCell( ";  '", "F12", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Visualization Tools
	// DrawKeyCell( "Visualization Tools:", "A13", 6, 1, key_fontHeader * 1.3, CONFIG_colWhite, CONFIG_colGraDk, false );
	// DrawKeyCell( "Enable visualizer (toggle)", "A14", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	// DrawKeyCell( "v", "F14", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawKeyCell( "Clear visualizer ", "A15", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	// DrawKeyCell( "z", "F15", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// // // Tasks
	// DrawKeyCell( "Tasks:", "A14", 6, 1, key_fontHeader * 1.3, CONFIG_colWhite, CONFIG_colGraDk, false );
	// DrawKeyCell( "Calibrate Offset", "A15", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	// DrawKeyCell( "c", "F15", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawKeyCell( "Target Test (Fitts)", "A16", 5, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	// DrawKeyCell( "f", "F16", 1, 1, key_fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


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
	cv::Point3i p3D		= cv::Point3i( shared->targetMarkerPosition3dNew.x, shared->targetMarkerPosition3dNew.y, shared->targetMarkerPosition3dNew.z );
	cv::Point3i p3DInv	= cv::Point3i( shared->targetMarkerPosition3dNew.z, -shared->targetMarkerPosition3dNew.y, shared->targetMarkerPosition3dNew.x );
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
	float zClamped = std::clamp( shared->targetMarkerPosition3dNew.z, 0.0f, 1000.0f );
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

	if ( shared->FLAG_FINGER_MARKER_FOUND ) {
		// Calculate finger marker position
		cv::Point3i p3DFinger	 = cv::Point3i( shared->fingerMarkerPosition3DNew.x, shared->fingerMarkerPosition3DNew.y, shared->fingerMarkerPosition3DNew.z );
		cv::Point3i p3DFingerInv = cv::Point3i( shared->fingerMarkerPosition3DNew.z, -shared->fingerMarkerPosition3DNew.y, shared->fingerMarkerPosition3DNew.x );

		// Line from marker to target
		// cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, 0, 0 ) ), ProjectIsometric( p3DFingerInv ), CONFIG_colCyaMd, 2 );

		// Moving local axis
		cv::line( matVisualizer, ProjectIsometric( cv::Point3i( p3DFingerInv.x, p3DFingerInv.y, vizLimXY ) ), ProjectIsometric( p3DFingerInv ), CONFIG_colGreMd, 1 );
		cv::line( matVisualizer, ProjectIsometric( cv::Point3i( p3DFingerInv.x, vizLimXY, p3DFingerInv.z ) ), ProjectIsometric( p3DFingerInv ), CONFIG_colRedMd, 1 );
		cv::line( matVisualizer, ProjectIsometric( cv::Point3i( 0, p3DFingerInv.y, p3DFingerInv.z ) ), ProjectIsometric( p3DFingerInv ), CONFIG_colBluMd, 1 );

		// // Draw finger
		cv::circle( matVisualizer, ProjectIsometric( p3DFingerInv ), 5, CONFIG_colBluLt, -1 );
		cv::circle( matVisualizer, ProjectIsometric( p3DFingerInv ), 5, CONFIG_colBlack, 1 );

		// Offset vector
		cv::Vec3f v( 10.0f, 0.0f, +35.0f );

		// Rotation angle
		// float		angleRad = CV_PI / 4.0f;
		// cv::Matx33f Rx( 1, 0, 0, 0, cos( angleRad ), -sin( angleRad ), 0, sin( angleRad ), cos( angleRad ) );

		cv::Matx33d R;
		cv::Rodrigues( shared->fingerMarkerRotationVector, R );

		// Apply rotation
		cv::Vec3f	rotated = R * v;
		cv::Point3f F		= shared->fingerMarkerPosition3DNew + cv::Point3i( rotated );

		cv::Point3i p3DTip	  = cv::Point3i( F.x, F.y, F.z );
		cv::Point3i p3DTipInv = cv::Point3i( F.z, -F.y, F.x );

		// // Draw fingertip
		cv::circle( matVisualizer, ProjectIsometric( p3DTipInv ), 5, CONFIG_colRedMd, -1 );
		cv::circle( matVisualizer, ProjectIsometric( p3DTipInv ), 5, CONFIG_colBlack, 1 );

		// Draw line
		cv::line( matVisualizer, ProjectIsometric( p3DFingerInv ), ProjectIsometric( p3DTipInv ), CONFIG_colOraMd, 2 );

		// --- Draw marker frame around ring marker --- //
		float markerHalf = CONFIG_MEDIUM_MARKER_WIDTH * MM2PX;

		// Define marker corners in local frame (Z points forward from marker, Y up)
		std::vector<cv::Vec3f> localCorners = {
			cv::Vec3f( -markerHalf, markerHalf, 0 ),	// top-left
			cv::Vec3f( markerHalf, markerHalf, 0 ),		// top-right
			cv::Vec3f( markerHalf, -markerHalf, 0 ),	// bottom-right
			cv::Vec3f( -markerHalf, -markerHalf, 0 )	// bottom-left
		};

		// Transform each corner to world coordinates
		std::vector<cv::Point3i> worldCorners;
		for ( const auto& c : localCorners ) {
			cv::Vec3f	rotated = R * c;
			cv::Point3f world	= shared->fingerMarkerPosition3DNew + cv::Point3i( rotated );
			worldCorners.push_back( cv::Point3i( world.z, -world.y, world.x ) );	// apply same flip as used in p3DFingerInv
		}

		// Draw square
		for ( size_t i = 0; i < 4; i++ ) {
			cv::Point2i pt1 = ProjectIsometric( worldCorners[i] );
			cv::Point2i pt2 = ProjectIsometric( worldCorners[( i + 1 ) % 4] );
			cv::line( matVisualizer, pt1, pt2, CONFIG_colBluMd, 1 );
		}
	}

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
	cv::Point2i endPtVel( int( shared->targetMarkerVelocity3dNew.x * MM2PX ), int( shared->targetMarkerVelocity3dNew.x * MM2PX ) );
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

	if ( shared->touchDetected == 1 ) {

		// Save image
		if ( shared->FLAG_LOGGING_ENABLED ) {
			std::string imageFilename = "/home/tom/Code/nuring/logging/" + shared->loggingFilename + ".png";
			shared->displayString	  = "Saving file " + imageFilename;
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
