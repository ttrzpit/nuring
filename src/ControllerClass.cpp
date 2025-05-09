// Call to class header
#include "ControllerClass.h"

// System data manager
#include "SystemDataManager.h"


/**
 * @brief Construct a new Controller object
 * 
 * @param ctx Data manager handle
 */
ControllerClass::ControllerClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	// Stuff
}


void ControllerClass::Initialize( float T ) {

	// Update time segment
	timeTrajectorySegmentT = T;

	// Calculate initial position and angle
	positionInitial = cv::Point3f( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z );
	thetaInitial	= cv::Point2f( atan2( positionInitial.x, positionInitial.z ), atan2( positionInitial.y, positionInitial.z ) );
	timeStart		= shared->timingTimestamp;
	timeElapsed		= 0.0f;
}



/**
 * @brief Reset the Minimum Jerk controller
 * 
 */
void ControllerClass::MinJerkReset() {

	// Calculate initial position and angle
	positionInitial = cv::Point3f( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z );
	thetaInitial	= cv::Point2f( atan2( positionInitial.x, positionInitial.z ), atan2( positionInitial.y, positionInitial.z ) );
	timeStart		= shared->timingTimestamp;
	timeElapsed		= 0.0f;
}



/**
 * @brief Main controller logic
 * 
 */
void ControllerClass::RunHybridController() {

	// Update elapsed time
	timeElapsed = shared->timingTimestamp - timeStart;

	//Check if timeElapsed > timeTrajectoryT, which triggers the reset
	if ( timeElapsed < timeTrajectorySegmentT ) {

		// Calculate new tau
		CalculateTau();

		// Calculate new angle and angle velocity
		CalculateThetaDesired();

		// Run impedance controller using MJ profile
		ImpedanceController();

		// Generate PWM values for amplifier
		GenerateAmplifierPWM();
	} else {

		// Re-initialize MJ
		MinJerkReset();
	}
}


/**
 * @brief Returns tau raised to a given power
 * 
 * @param power The power to raise tau by
 * @return float Tau raised to p power
 */
float ControllerClass::TauRaised( uint8_t power ) {

	float tauRaised = tau;

	// Iterate
	for ( int i = 1; i < power; i++ ) {

		tauRaised *= tau;
	}

	return tauRaised;
}


/**
 * @brief Calculate tau
 * 
 */
void ControllerClass::CalculateTau() {

	// Verify not dividing by 0
	if ( timeTrajectorySegmentT > 0.0f ) {
		tau = std::min( timeElapsed / timeTrajectorySegmentT, 1.0f );
	} else {
		std::cout << "ControllerClass: Divide by zero error in CalculateTau()\n";
	}
}


/**
 * @brief Calculate the desired theta and theta velocity
 * 
 */
void ControllerClass::CalculateThetaDesired() {

	// Calculate profile coefficient
	tauCoeff	= 1.0f - ( 10.0f * TauRaised( 3 ) ) + ( 15.0f * TauRaised( 4 ) ) - ( 6.0f * TauRaised( 5 ) );
	tauCoeffDot = ( 30.0f * TauRaised( 2 ) ) - ( 60.0f * TauRaised( 3 ) ) + ( 30.0f * TauRaised( 4 ) );

	// Calculate desired theta
	thetaDesired.x = thetaInitial.x * tauCoeff;
	thetaDesired.y = thetaInitial.y * tauCoeff;

	// Calculate desired theta velocity
	thetaDotDesired.x = ( thetaInitial.x / timeTrajectorySegmentT ) * tauCoeffDot;
	thetaDotDesired.y = ( thetaInitial.y / timeTrajectorySegmentT ) * tauCoeffDot;

	// Update global
	shared->controllerThetaDesired	  = thetaDesired;
	shared->controllerThetaDotDesired = thetaDotDesired;
	
}


/**
 * @brief Impedance controller
 * 
 */
void ControllerClass::ImpedanceController() {

	// Update measured position / angle values
	positionMeasured = cv::Point3f( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z );
	thetaMeasured	 = cv::Point2f( atan2( positionMeasured.x, positionMeasured.z ), atan2( positionMeasured.y, positionMeasured.z ) );

	// Update measured velocity / angular velocity values
	velocityMeasured = cv::Point3f( shared->arucoTags[shared->arucoActiveID].errorVel3D.x, shared->arucoTags[shared->arucoActiveID].errorVel3D.y, shared->arucoTags[shared->arucoActiveID].errorVel3D.z );
	thetaDotMeasured = cv::Point2f( atan2( velocityMeasured.x, velocityMeasured.z ), atan2( velocityMeasured.y, velocityMeasured.z ) );

	// Calculate virtual force
	virtualForceXY.x = ( shared->controllerKp.x * ( thetaDesired.x - thetaMeasured.x ) ) + ( shared->controllerKd.x * ( thetaDotDesired.x - thetaDotMeasured.x ) );
	virtualForceXY.y = ( shared->controllerKp.y * ( thetaDesired.y - thetaMeasured.y ) ) + ( shared->controllerKd.y * ( thetaDotDesired.y - thetaDotMeasured.y ) );

	// Map virtual forces to virtual motor forces
	virtualForceABC.x = ( virtualForceXY.x * COS35 ) + ( virtualForceXY.y * SIN35 );
	virtualForceABC.y = ( virtualForceXY.x * COS145 ) + ( virtualForceXY.y * SIN145 );
	virtualForceABC.z = ( virtualForceXY.y * SIN270 );

	// Save to global
	shared->controllerForceXY  = virtualForceXY;
	shared->controllerForceABC = virtualForceABC;
}

/**
 * @brief Generate PWM values for amplifier
 * 
 */
void ControllerClass::GenerateAmplifierPWM() {

	// Calculate current
	commandedCurrent.x = ConstrainValues( ( virtualForceABC.x * radius ) / torqueConstant, -nominalCurrent, nominalCurrent );
	commandedCurrent.y = ConstrainValues( ( virtualForceABC.y * radius ) / torqueConstant, -nominalCurrent, nominalCurrent );
	commandedCurrent.z = ConstrainValues( ( virtualForceABC.z * radius ) / torqueConstant, -nominalCurrent, nominalCurrent );

	// Calculate commanded current percentage
	commandedPercentage = commandedCurrent / nominalCurrent;

	// Calculate PWM values
	commandedPWM.x = ConstrainValues( 2047 - ( commandedPercentage.x * 2046 ), 1, 2047 );
	commandedPWM.y = ConstrainValues( 2047 - ( commandedPercentage.y * 2046 ), 1, 2047 );
	commandedPWM.z = ConstrainValues( 2047 - ( commandedPercentage.z * 2046 ), 1, 2047 );


	// Save to system data manager
	shared->controllerPercentage = commandedPercentage;
	shared->controllerPWM		 = commandedPWM;
}



float ControllerClass::ConstrainValues( float x, float minVal, float maxVal ) {

	// Check boundaries
	if ( x < minVal )
		return minVal;
	if ( x > maxVal )
		return maxVal;
	return x;
}