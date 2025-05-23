#include "ControllerClass.h"
#include "SystemDataManager.h"

ControllerClass::ControllerClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	//
}


void ControllerClass::Update() {



	// Calculate error angle for motor selection
	thTarget = std::atan2( shared->targetMarkerPosition3dNew.y, shared->targetMarkerPosition3dNew.x );
	rTarget	 = cv::norm( cv::Vec2f( shared->targetMarkerPosition3dNew.x, shared->targetMarkerPosition3dNew.y ) );

	// Wrap angle within 0-360 deg
	if ( thTarget < 0 ) {
		thTarget += ( 360.0f * DEG2RAD );
	}

	// Calculate max radius
	rMax   = std::clamp( float( std::atan2( rTarget, shared->targetMarkerPosition3dNew.z ) ), 0.0f, float( 40.0 * DEG2RAD ) ) / ( 40.0 * DEG2RAD );
	rMaxXY = rMax * shared->controllerKp.x;
	rMaxZ  = rMax * shared->controllerKp.y;

	// Determine angle and solve
	if ( thTarget >= ( 270 * DEG2RAD ) || thTarget < ( 35 * DEG2RAD ) ) {

		// Wrap-around for 360/0 deg, calculate angles AC
		if ( thTarget < thA ) {
			thAT = thA - thTarget;
			thCT = thAC - thAT;
		}
		if ( thTarget > thC ) {
			thCT = thTarget - thC;
			thAT = thAC - thCT;
		}

		// rMax		   = rMax * RAD2DEG;
		contribution.x = ( 1.0f - ( thAT / thAC ) ) * rMaxXY;
		contribution.z = ( 1.0f - ( thCT / thAC ) ) * rMaxZ;

		// Debug
		// std::cout << "|A+C| Angle: " << shared->FormatDecimal( thTarget * RAD2DEG, 2 ) << "   Contrib: A: " << shared->FormatDecimal( contribution.x, 2 ) << " , C: " << shared->FormatDecimal( contribution.z, 2 ) << "   rMax: " << shared->FormatDecimal( rMax, 2 ) << "\n";
	} else if ( thTarget >= ( 35 * DEG2RAD ) && thTarget < ( 145 * DEG2RAD ) ) {

		// Calculate angles AB
		thAT = thTarget - thA;
		thBT = thAB - thAT;

		contribution.x = ( 1.0f - ( thAT / thAB ) ) * rMaxXY;
		contribution.y = ( 1.0f - ( thBT / thAB ) ) * rMaxZ;

		// std::cout << "|A+B| Angle: " << shared->FormatDecimal( thTarget * RAD2DEG, 2 ) << "   Contrib: A: " << shared->FormatDecimal( contribution.x, 2 ) << " , B: " << shared->FormatDecimal( contribution.y, 2 ) << "   rMax: " << shared->FormatDecimal( rMax, 2 ) << "\n";
	} else if ( thTarget >= ( 145 * DEG2RAD ) && thTarget < ( 270 * DEG2RAD ) ) {

		// Calculate angles BC
		thBT = thTarget - thB;
		thCT = thBC - thBT;

		contribution.y = ( 1.0f - ( thBT / thBC ) ) * rMaxXY;
		contribution.z = ( 1.0f - ( thCT / thBC ) ) * rMaxZ;

		// std::cout << "|B+C| Angle: " << shared->FormatDecimal( thTarget * RAD2DEG, 2 ) << "   Contrib: B: " << shared->FormatDecimal( contribution.y, 2 ) << " , C: " << shared->FormatDecimal( contribution.z, 2 ) << "   rMax: " << shared->FormatDecimal( rMax, 2 ) << "\n";
	}

	// Multiply with gains
	shared->controllerPercentage = cv::Point3f( contribution );

	// Map to PWM
	shared->controllerPWM.x = std::clamp( 2048 - int( ( shared->controllerPercentage.x ) * 2047 ), 4, 2044 );
	shared->controllerPWM.y = std::clamp( 2048 - int( ( shared->controllerPercentage.y ) * 2047 ), 4, 2044 );
	shared->controllerPWM.z = std::clamp( 2048 - int( ( shared->controllerPercentage.z ) * 2047 ), 4, 2044 );
}
