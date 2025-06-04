#include "ControllerClass.h"
#include "SystemDataManager.h"

ControllerClass::ControllerClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	//
}


void ControllerClass::Update() {

	// Reset ramp if target was reset
	if ( shared->Telemetry.isTargetReset ) {

		shared->Controller.rampPercentage = 0.0f;
		shared->Controller.rampStartTime  = shared->Timing.elapsedRunningTime;
	}



	// Ramp-up value
	if ( shared->Controller.isRampingUp && shared->Telemetry.isTargetFound ) {
		float elapsed					  = shared->Timing.elapsedRunningTime - shared->Controller.rampStartTime;
		shared->Controller.rampPercentage = std::clamp( elapsed / shared->Controller.rampDurationTime, 0.0f, 1.0f );

		if ( shared->Controller.rampPercentage >= 1.0f ) {
			shared->Controller.isRampingUp = false;
		}
	}

	if ( shared->Telemetry.isTargetFound ) {
		// Calculate proportional term
		shared->controllerProportinalTerm.x = shared->controllerKp.x * shared->Telemetry.positionFilteredNewMM.x;
		shared->controllerProportinalTerm.y = shared->controllerKp.y * shared->Telemetry.positionFilteredNewMM.y;
		shared->controllerProportinalTerm.z = shared->controllerKp.z * shared->Telemetry.positionFilteredNewMM.z;

		// Calculate derivative term
		shared->controllerDerivativeTerm.x = shared->controllerKd.x * shared->Telemetry.velocityFilteredNewMM.x;
		shared->controllerDerivativeTerm.y = shared->controllerKd.y * shared->Telemetry.velocityFilteredNewMM.y;
		shared->controllerDerivativeTerm.z = shared->controllerKd.z * shared->Telemetry.velocityFilteredNewMM.z;

		// Calculate integral term
		shared->controllerIntegralTerm.x = shared->controllerKi.x * shared->Telemetry.positionIntegratedMM.x;
		shared->controllerIntegralTerm.y = shared->controllerKi.y * shared->Telemetry.positionIntegratedMM.y;
		shared->controllerIntegralTerm.z = shared->controllerKi.z * shared->Telemetry.positionIntegratedMM.z;

		// Sum terms
		shared->controllerTotalTerm = ( shared->controllerProportinalTerm + shared->controllerDerivativeTerm + shared->controllerIntegralTerm ) * shared->Controller.rampPercentage;
	} else {
		shared->controllerTotalTerm = cv::Point3f( 0.0f, 0.0f, 0.0f );
	}
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
	thTarget = std::atan2( shared->Controller.combinedPIDTerms.y, shared->Controller.combinedPIDTerms.x );
	rTarget	 = cv::norm( cv::Vec2f( shared->Controller.combinedPIDTerms.x, shared->Controller.combinedPIDTerms.y ) );

	// Calculate max strength
	rMax = std::clamp( float( std::atan2( rTarget, shared->Telemetry.positionFilteredNewMM.z ) ), 0.0f, float( 40.0 * DEG2RAD ) ) / ( 40.0 * DEG2RAD );

	// Wrap angle within 0-360 deg
	if ( thTarget < 0 ) {
		thTarget += ( 360.0f * DEG2RAD );
	}

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

	// Update percentage and constrain to max
	shared->Controller.commandedPercentage.x = std::clamp( ( contribution.x + shared->Controller.commandedTensionABC.x ), 0.0f, shared->Controller.commandedPercentageLimit.x );
	shared->Controller.commandedPercentage.y = std::clamp( ( contribution.y + shared->Controller.commandedTensionABC.y ), 0.0f, shared->Controller.commandedPercentageLimit.y );
	shared->Controller.commandedPercentage.z = std::clamp( ( contribution.z + shared->Controller.commandedTensionABC.z ), 0.0f, shared->Controller.commandedPercentageLimit.z );

	// Map to current
	shared->Controller.commandedCurrentABC = MapToCurrent( shared->Controller.commandedPercentage, CONFIG_DEVICE_NOMINAL_CURRENT );

	// Map to PWM
	shared->Controller.commandedPwmABC = MapToPWM( shared->Controller.commandedPercentage, 4, 2044 );
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


void ControllerClass::Update4D() {

	// Reset ramp if target was reset
	if ( shared->Telemetry.isTargetReset ) {

		shared->Controller.rampPercentage = 0.0f;
		shared->Controller.rampStartTime  = shared->Timing.elapsedRunningTime;
	}

	// Ramp-up value
	if ( shared->Controller.isRampingUp && shared->Telemetry.isTargetFound ) {
		float elapsed					  = shared->Timing.elapsedRunningTime - shared->Controller.rampStartTime;
		shared->Controller.rampPercentage = std::clamp( elapsed / shared->Controller.rampDurationTime, 0.0f, 1.0f );

		if ( shared->Controller.rampPercentage >= 1.0f ) {
			shared->Controller.isRampingUp = false;
		}
	}

	if ( shared->Telemetry.isTargetFound ) {


		// Proportional AB+AD / FLEX+EXT
		shared->Telemetry.positionFilteredNewMM.x < 0 ? ( shared->Controller.proportionalTerm.x = shared->Controller.gainKp.abb * shared->Telemetry.positionFilteredNewMM.x ) : ( shared->Controller.proportionalTerm.x = shared->Controller.gainKp.add * shared->Telemetry.positionFilteredNewMM.x );
		shared->Telemetry.positionFilteredNewMM.y < 0 ? ( shared->Controller.proportionalTerm.y = shared->Controller.gainKp.flex * shared->Telemetry.positionFilteredNewMM.y ) : ( shared->Controller.proportionalTerm.y = shared->Controller.gainKp.ext * shared->Telemetry.positionFilteredNewMM.y );

		// Calculate derivative term
		shared->Telemetry.velocityFilteredNewMM.x < 0 ? ( shared->Controller.derivativeTerm.x = shared->Controller.gainKd.abb * shared->Telemetry.velocityFilteredNewMM.x ) : ( shared->Controller.derivativeTerm.x = shared->Controller.gainKd.add * shared->Telemetry.velocityFilteredNewMM.x );
		shared->Telemetry.velocityFilteredNewMM.y < 0 ? ( shared->Controller.derivativeTerm.y = shared->Controller.gainKd.flex * shared->Telemetry.velocityFilteredNewMM.y ) : ( shared->Controller.derivativeTerm.y = shared->Controller.gainKd.ext * shared->Telemetry.velocityFilteredNewMM.y );

		// Calculate integral term
		shared->Telemetry.positionIntegratedMM.x < 0 ? ( shared->Controller.integralTerm.x = shared->Controller.gainKi.abb * shared->Telemetry.positionIntegratedMM.x ) : ( shared->Controller.integralTerm.x = shared->Controller.gainKi.add * shared->Telemetry.positionIntegratedMM.x );
		shared->Telemetry.positionIntegratedMM.y < 0 ? ( shared->Controller.integralTerm.y = shared->Controller.gainKi.flex * shared->Telemetry.positionIntegratedMM.y ) : ( shared->Controller.integralTerm.y = shared->Controller.gainKi.ext * shared->Telemetry.positionIntegratedMM.y );

		// Sum terms
		shared->Controller.combinedPIDTerms = ( shared->Controller.proportionalTerm + shared->Controller.integralTerm + shared->Controller.derivativeTerm ) * shared->Controller.rampPercentage;

	} else {
		shared->Controller.proportionalTerm *= 0.9f;
		shared->Controller.integralTerm *= 0.9f;
		shared->Controller.derivativeTerm *= 0.9f;
		shared->Controller.combinedPIDTerms = ( shared->Controller.proportionalTerm + shared->Controller.integralTerm + shared->Controller.derivativeTerm ) * shared->Controller.rampPercentage;
	}
	// Calculate motor contributions
	MapToContributionABC();
}
