#include "ControllerClass.h"
#include "SystemDataManager.h"

ControllerClass::ControllerClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	//
}


void ControllerClass::Update() {

	// Calculate proportional term
	shared->controllerProportinalTerm.x = shared->controllerKp.x * shared->targetMarkerPosition3dNew.x;
	shared->controllerProportinalTerm.y = shared->controllerKp.y * shared->targetMarkerPosition3dNew.y;
	shared->controllerProportinalTerm.z = shared->controllerKp.z * shared->targetMarkerPosition3dNew.z;

	// Calculate derivative term
	shared->controllerDerivativeTerm.x = shared->controllerKd.x * shared->targetMarkerVelocity3dNew.x;
	shared->controllerDerivativeTerm.y = shared->controllerKd.y * shared->targetMarkerVelocity3dNew.y;
	shared->controllerDerivativeTerm.z = shared->controllerKd.z * shared->targetMarkerVelocity3dNew.z;

	// Calculate integral term
	shared->controllerIntegralTerm.x = shared->controllerKi.x * shared->targetMarkerIntegralError.x;
	shared->controllerIntegralTerm.y = shared->controllerKi.y * shared->targetMarkerIntegralError.y;
	shared->controllerIntegralTerm.z = shared->controllerKi.z * shared->targetMarkerIntegralError.z;

	// Sum terms
	shared->controllerTotalTerm = shared->controllerProportinalTerm + shared->controllerDerivativeTerm + shared->controllerIntegralTerm;

	// Calculate motor contributions
	MapToContributionABC();
}


/**
 * @brief Maps the controller PID values to motor output values
 * 
 */
void ControllerClass::MapToContributionABC() {

	// Old (no PID)
	// Calculate error angle for motor selection
	thTarget = std::atan2( shared->controllerTotalTerm.y, shared->controllerTotalTerm.x );
	rTarget	 = cv::norm( cv::Vec2f( shared->controllerTotalTerm.x, shared->controllerTotalTerm.y ) );

	// Calculate max strength
	rMax = std::clamp( float( std::atan2( rTarget, shared->targetMarkerPosition3dNew.z ) ), 0.0f, float( 40.0 * DEG2RAD ) ) / ( 40.0 * DEG2RAD );


	// // Old (no PID)
	// // Calculate error angle for motor selection
	// thTarget = std::atan2( shared->targetMarkerPosition3dNew.y, shared->targetMarkerPosition3dNew.x );
	// rTarget	 = cv::norm( cv::Vec2f( shared->targetMarkerPosition3dNew.x, shared->targetMarkerPosition3dNew.y ) );

	// // Calculate max strength
	// rMax = std::clamp( float( std::atan2( rTarget, shared->targetMarkerPosition3dNew.z ) ), 0.0f, float( 40.0 * DEG2RAD ) ) / ( 40.0 * DEG2RAD );


	// Wrap angle within 0-360 deg
	if ( thTarget < 0 ) {
		thTarget += ( 360.0f * DEG2RAD );
	}

	// rMaxXY = rMax;
	// rMaxZ  = rMax;
	// rMaxXY = rMax * shared->controllerKp.x;
	// rMaxZ  = rMax * shared->controllerKp.y;

	// Determine angle and calculate contributions
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
		contribution.x = ( 1.0f - ( thAT / thAC ) ) * rMax;
		contribution.z = ( 1.0f - ( thCT / thAC ) ) * rMax;

		// Debug
		// std::cout << "|A+C| Angle: " << shared->FormatDecimal( thTarget * RAD2DEG, 2 ) << "   Contrib: A: " << shared->FormatDecimal( contribution.x, 2 ) << " , C: " << shared->FormatDecimal( contribution.z, 2 ) << "   rMax: " << shared->FormatDecimal( rMax, 2 ) << "\n";
	} else if ( thTarget >= ( 35 * DEG2RAD ) && thTarget < ( 145 * DEG2RAD ) ) {

		// Calculate angles AB
		thAT = thTarget - thA;
		thBT = thAB - thAT;

		contribution.x = ( 1.0f - ( thAT / thAB ) ) * rMax;
		contribution.y = ( 1.0f - ( thBT / thAB ) ) * rMax;

		// std::cout << "|A+B| Angle: " << shared->FormatDecimal( thTarget * RAD2DEG, 2 ) << "   Contrib: A: " << shared->FormatDecimal( contribution.x, 2 ) << " , B: " << shared->FormatDecimal( contribution.y, 2 ) << "   rMax: " << shared->FormatDecimal( rMax, 2 ) << "\n";
	} else if ( thTarget >= ( 145 * DEG2RAD ) && thTarget < ( 270 * DEG2RAD ) ) {

		// Calculate angles BC
		thBT = thTarget - thB;
		thCT = thBC - thBT;

		contribution.y = ( 1.0f - ( thBT / thBC ) ) * rMax;
		contribution.z = ( 1.0f - ( thCT / thBC ) ) * rMax;

		// std::cout << "|B+C| Angle: " << shared->FormatDecimal( thTarget * RAD2DEG, 2 ) << "   Contrib: B: " << shared->FormatDecimal( contribution.y, 2 ) << " , C: " << shared->FormatDecimal( contribution.z, 2 ) << "   rMax: " << shared->FormatDecimal( rMax, 2 ) << "\n";
	}

	// Update percentage
	shared->controllerPercentage = cv::Point3f( contribution ) + shared->controllerTension;

	// Map to current
	shared->controllerCurrent = MapToCurrent( shared->controllerPercentage, CONFIG_DEVICE_NOMINAL_CURRENT );

	// Map to PWM
	shared->controllerPWM = MapToPWM( shared->controllerPercentage, 4, 2044 );



	// Debug output
	// std::cout << "ControllerClass: E_T: ( " << shared->FormatDecimal( shared->targetMarkerPosition3dNew.x, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerPosition3dNew.y, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerPosition3dNew.z, 2, 1 ) << " ) ";
	// std::cout << "\t dE_t: ( " << shared->FormatDecimal( shared->targetMarkerVelocity3dNew.x, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerVelocity3dNew.y, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerVelocity3dNew.z, 2, 1 ) << " ) ";
	// std::cout << "\t iE_t: ( " << shared->FormatDecimal( shared->targetMarkerIntegralError.x, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerIntegralError.y, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerIntegralError.z, 2, 1 ) << " ) ";
	// std::cout << "\n";

	// std::cout << "ControllerClass: E_T: ( " << shared->FormatDecimal( shared->targetMarkerPosition3dNew.x, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerPosition3dNew.y, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerPosition3dNew.z, 2, 1 ) << " ) ";
	// std::cout << "\t dE_t: ( " << shared->FormatDecimal( shared->targetMarkerVelocity3dNew.x, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerVelocity3dNew.y, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerVelocity3dNew.z, 2, 1 ) << " ) ";
	// std::cout << "\t iE_t: ( " << shared->FormatDecimal( shared->targetMarkerIntegralError.x, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerIntegralError.y, 2, 1 ) << " , " << shared->FormatDecimal( shared->targetMarkerIntegralError.z, 2, 1 ) << " ) ";
	// std::cout << "\n";

	// Map to PWM
	// shared->controllerPWM.y = std::clamp( 2048 - int( ( shared->controllerPercentage.x ) * 2047 ), 4, 2044 );
	// shared->controllerPWM.y = std::clamp( 2048 - int( ( shared->controllerPercentage.y ) * 2047 ), 4, 2044 );
	// shared->controllerPWM.z = std::clamp( 2048 - int( ( shared->controllerPercentage.z ) * 2047 ), 4, 2044 );
}


cv::Point3f ControllerClass::MapToCurrent( cv::Point3f percentage, float iNominal ) {

	return cv::Point3f( percentage.x * iNominal, percentage.y * iNominal, percentage.z * iNominal );
}


cv::Point3i ControllerClass::MapToPWM( cv::Point3f percentage, int min, int max ) {

	cv::Point3i pt = cv::Point3i( 0, 0, 0 );

	// Re-map
	pt.x = std::clamp( 2048 - int( percentage.x * 2047 ), min, max );
	pt.y = std::clamp( 2048 - int( percentage.y * 2047 ), min, max );
	pt.z = std::clamp( 2048 - int( percentage.z * 2047 ), min, max );

	// Return result
	return pt;
};