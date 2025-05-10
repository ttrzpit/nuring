#include "ControllerClass.h"
#include "SystemDataManager.h"

ControllerClass::ControllerClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	//
}


// void ControllerClass::Initialize() {
// 	timeLastUpdate = shared->timingTimestamp;
// 	UpdateThetaPlan();
// }

// void ControllerClass::RunRecedingHorizon() {
// 	float timeNow  = shared->timingTimestamp;
// 	float tElapsed = timeNow - timeLastUpdate;

// 	if ( shared->arucoTags[shared->arucoActiveID].present ) {
// 		UpdateThetaPlan();

// 		float tau	 = Tau( tElapsed );
// 		float tauDot = TauDot( tElapsed );

// 		// Update desired angle and velocity
// 		thetaDesired.x	  = thetaInitial.x * tau;
// 		thetaDesired.y	  = thetaInitial.y * tau;
// 		thetaDotDesired.x = ( thetaInitial.x / T ) * tauDot;
// 		thetaDotDesired.y = ( thetaInitial.y / T ) * tauDot;

// 		shared->controllerThetaDesired	  = thetaDesired;
// 		shared->controllerThetaDotDesired = thetaDotDesired;
// 	}
// }

// void ControllerClass::UpdateThetaPlan() {
// 	// Get latest error position
// 	cv::Point3f err = shared->arucoTags[shared->arucoActiveID].error3D;
// 	thetaInitial	= cv::Point2f( atan2( err.x, err.z ), atan2( err.y, err.z ) );
// 	timeLastUpdate	= shared->timingTimestamp;
// }

// float ControllerClass::Tau( float tElapsed ) {
// 	float s = std::min( tElapsed / T, 1.0f );
// 	return 10 * pow( s, 3 ) - 15 * pow( s, 4 ) + 6 * pow( s, 5 );
// }

// float ControllerClass::TauDot( float tElapsed ) {
// 	float s = std::min( tElapsed / T, 1.0f );
// 	return ( 30 * pow( s, 2 ) - 60 * pow( s, 3 ) + 30 * pow( s, 4 ) );
// }


// void ControllerClass::GenerateAmplifierPWM() {

// 	// Calculate torque = Force * radius
// 	cv::Point3f commandedTorque( virtualForceABC.x * radius, virtualForceABC.y * radius, virtualForceABC.z * radius );

// 	// Calculate current = torque / torqueConstant
// 	commandedCurrent.x = ConstrainValues( commandedTorque.x / torqueConstant, -nominalCurrent, nominalCurrent );
// 	commandedCurrent.y = ConstrainValues( commandedTorque.y / torqueConstant, -nominalCurrent, nominalCurrent );
// 	commandedCurrent.z = ConstrainValues( commandedTorque.z / torqueConstant, -nominalCurrent, nominalCurrent );

// 	// Normalize current to percentage of full-scale (1.0 = nominalCurrent)
// 	commandedPercentage = commandedCurrent / nominalCurrent;

// 	// Convert to PWM values where 2047 = 0 A, 1 = max current
// 	commandedPWM.x = ConstrainValues( 2047 - commandedPercentage.x * 2046, 1, 2047 );
// 	commandedPWM.y = ConstrainValues( 2047 - commandedPercentage.y * 2046, 1, 2047 );
// 	commandedPWM.z = ConstrainValues( 2047 - commandedPercentage.z * 2046, 1, 2047 );

// 	// Save to shared data
// 	shared->controllerCurrent	 = commandedCurrent;
// 	shared->controllerPercentage = commandedPercentage;
// 	shared->controllerPWM		 = commandedPWM;
// }



// // // Call to class header
// // #include "ControllerClass.h"

// // // System data manager
// // #include "SystemDataManager.h"


// // /**
// //  * @brief Construct a new Controller object
// //  *
// //  * @param ctx Data manager handle
// //  */
// // ControllerClass::ControllerClass( SystemDataManager& ctx )
// // 	: dataHandle( ctx )
// // 	, shared( ctx.getData() ) {
// // 	// Stuff
// // }


// // void ControllerClass::Initialize( float T ) {

// // 	// Update time segment
// // 	timeTrajectorySegmentT = T;

// // 	// Calculate initial position and angle
// // 	positionInitial	 = cv::Point3f( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z );
// // 	thetaInitial	 = cv::Point2f( atan2( positionInitial.x, positionInitial.z ), atan2( positionInitial.y, positionInitial.z ) );
// // 	timeStart		 = shared->timingTimestamp;
// // 	timeElapsed		 = 0.0f;
// // 	positionPrevious = positionInitial;
// // }



// // /**
// //  * @brief Reset the Minimum Jerk controller
// //  *
// //  */
// // void ControllerClass::MinJerkReset() {

// // 	// Calculate initial position and angle
// // 	positionInitial	 = cv::Point3f( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z );
// // 	thetaInitial	 = cv::Point2f( atan2( positionInitial.x, positionInitial.z ), atan2( positionInitial.y, positionInitial.z ) );
// // 	timeStart		 = shared->timingTimestamp;
// // 	timeElapsed		 = 0.0f;
// // 	positionPrevious = positionInitial;
// // }



// // /**
// //  * @brief Main controller logic
// //  *
// //  */
// // void ControllerClass::RunHybridController() {


// // 	// Check that marker is detected
// // 	if ( !shared->arucoTags[shared->arucoActiveID].present ) {
// // 		return;
// // 	}

// // 	// Update elapsed time
// // 	timeElapsed = shared->timingTimestamp - timeStart;

// // 	// Get current position
// // 	cv::Point3f currentPosition = cv::Point3f( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z );

// // 	// Calculate distance moved
// // 	float distanceMoved = CalculateDistance( currentPosition, positionPrevious );

// // 	// Check if target moved far
// // 	if ( distanceMoved > 10.0f ) {

// // 		// Store previous position
// // 		positionPrevious = currentPosition;

// // 		// Re-initialize MJ
// // 		MinJerkReset();
// // 	}

// // 	//Check if timeElapsed > timeTrajectoryT, which triggers the reset
// // 	if ( timeElapsed < timeTrajectorySegmentT ) {

// // 		// Calculate new tau
// // 		CalculateTau();

// // 		// Calculate new angle and angle velocity
// // 		CalculateThetaDesired();

// // 		// Run impedance controller using MJ profile
// // 		ImpedanceController();

// // 		// Generate PWM values for amplifier
// // 		GenerateAmplifierPWM();
// // 	} else {

// // 		// Re-initialize MJ
// // 		MinJerkReset();
// // 	}
// // }


// // /**
// //  * @brief Returns tau raised to a given power
// //  *
// //  * @param power The power to raise tau by
// //  * @return float Tau raised to p power
// //  */
// // float ControllerClass::TauRaised( uint8_t power ) {

// // 	float tauRaised = tau;

// // 	// Iterate
// // 	for ( int i = 1; i < power; i++ ) {

// // 		tauRaised *= tau;
// // 	}

// // 	return tauRaised;
// // }


// // /**
// //  * @brief Calculate tau
// //  *
// //  */
// // void ControllerClass::CalculateTau() {

// // 	// Verify not dividing by 0
// // 	if ( timeTrajectorySegmentT > 0.0f ) {
// // 		tau = std::min( timeElapsed / timeTrajectorySegmentT, 1.0f );
// // 	} else {
// // 		std::cout << "ControllerClass: Divide by zero error in CalculateTau()\n";
// // 	}
// // }


// // /**
// //  * @brief Calculate the desired theta and theta velocity
// //  *
// //  */
// // void ControllerClass::CalculateThetaDesired() {

// // 	// Calculate profile coefficient
// // 	tauCoeff	= 1.0f - ( 10.0f * TauRaised( 3 ) ) + ( 15.0f * TauRaised( 4 ) ) - ( 6.0f * TauRaised( 5 ) );
// // 	tauCoeffDot = ( 30.0f * TauRaised( 2 ) ) - ( 60.0f * TauRaised( 3 ) ) + ( 30.0f * TauRaised( 4 ) );

// // 	// Calculate desired theta
// // 	thetaDesired.x = thetaInitial.x * tauCoeff;
// // 	thetaDesired.y = thetaInitial.y * tauCoeff;

// // 	// Calculate desired theta velocity
// // 	thetaDotDesired.x = ( thetaInitial.x / timeTrajectorySegmentT ) * tauCoeffDot;
// // 	thetaDotDesired.y = ( thetaInitial.y / timeTrajectorySegmentT ) * tauCoeffDot;

// // 	// Update global
// // 	shared->controllerThetaDesired	  = thetaDesired;
// // 	shared->controllerThetaDotDesired = thetaDotDesired;
// // }


// // /**
// //  * @brief Impedance controller
// //  *
// //  */
// // void ControllerClass::ImpedanceController() {

// // 	// Update measured position / angle values
// // 	positionMeasured = cv::Point3f( shared->arucoTags[shared->arucoActiveID].error3D.x, shared->arucoTags[shared->arucoActiveID].error3D.y, shared->arucoTags[shared->arucoActiveID].error3D.z );
// // 	thetaMeasured	 = cv::Point2f( atan2( positionMeasured.x, positionMeasured.z ), atan2( positionMeasured.y, positionMeasured.z ) );

// // 	// Update measured velocity / angular velocity values
// // 	// velocityMeasured = cv::Point3f( shared->arucoTags[shared->arucoActiveID].errorVel3D.x, shared->arucoTags[shared->arucoActiveID].errorVel3D.y, shared->arucoTags[shared->arucoActiveID].errorVel3D.z );
// // 	// thetaDotMeasured = cv::Point2f( atan2( velocityMeasured.x, velocityMeasured.z ), atan2( velocityMeasured.y, velocityMeasured.z ) );

// // 	// Update measured velocity
// // 	velocityMeasured = cv::Point3f( shared->arucoTags[shared->arucoActiveID].errorVel3D.x, shared->arucoTags[shared->arucoActiveID].errorVel3D.y, shared->arucoTags[shared->arucoActiveID].errorVel3D.z );

// // 	// Raw angular velocity
// // 	cv::Point2f rawThetaDotMeasured = cv::Point2f( atan2( velocityMeasured.x, velocityMeasured.z ), atan2( velocityMeasured.y, velocityMeasured.z ) );

// // 	// 🟦 Low-pass filter the angular velocity
// // 	filteredThetaDotMeasured.x = ( 1.0f - filterAlpha ) * filteredThetaDotMeasured.x + filterAlpha * rawThetaDotMeasured.x;
// // 	filteredThetaDotMeasured.y = ( 1.0f - filterAlpha ) * filteredThetaDotMeasured.y + filterAlpha * rawThetaDotMeasured.y;

// // 	// Save filtered result (optional global export)
// // 	thetaDotMeasured = filteredThetaDotMeasured;

// // 	// Calculate virtual force
// // 	virtualForceXY.x = ( shared->controllerKp.x * ( thetaDesired.x - thetaMeasured.x ) ) + ( shared->controllerKd.x * ( thetaDotDesired.x - thetaDotMeasured.x ) );
// // 	virtualForceXY.y = ( shared->controllerKp.y * ( thetaDesired.y - thetaMeasured.y ) ) + ( shared->controllerKd.y * ( thetaDotDesired.y - thetaDotMeasured.y ) );

// // 	// Map virtual forces to virtual motor forces
// // 	virtualForceABC.x = ( virtualForceXY.x * COS35 ) + ( virtualForceXY.y * SIN35 );
// // 	virtualForceABC.y = ( virtualForceXY.x * COS145 ) + ( virtualForceXY.y * SIN145 );
// // 	virtualForceABC.z = ( virtualForceXY.y * SIN270 );

// // 	// Save to global
// // 	shared->controllerForceXY  = virtualForceXY;
// // 	shared->controllerForceABC = virtualForceABC;
// // }

// // /**
// //  * @brief Generate PWM values for amplifier
// //  *
// //  */
// // void ControllerClass::GenerateAmplifierPWM() {

// // 	// Calculate current
// // 	commandedCurrent.x = ConstrainValues( ( virtualForceABC.x * radius ) / torqueConstant, -nominalCurrent, nominalCurrent );
// // 	commandedCurrent.y = ConstrainValues( ( virtualForceABC.y * radius ) / torqueConstant, -nominalCurrent, nominalCurrent );
// // 	commandedCurrent.z = ConstrainValues( ( virtualForceABC.z * radius ) / torqueConstant, -nominalCurrent, nominalCurrent );

// // 	// Calculate commanded current percentage
// // 	commandedPercentage = commandedCurrent / nominalCurrent;

// // 	// Calculate PWM values
// // 	commandedPWM.x = ConstrainValues( 2047 - ( commandedPercentage.x * 2046 ), 1, 2047 );
// // 	commandedPWM.y = ConstrainValues( 2047 - ( commandedPercentage.y * 2046 ), 1, 2047 );
// // 	commandedPWM.z = ConstrainValues( 2047 - ( commandedPercentage.z * 2046 ), 1, 2047 );


// // 	// Save to system data manager
// // 	shared->controllerPercentage = commandedPercentage;
// // 	shared->controllerPWM		 = commandedPWM;
// // }



// float ControllerClass::ConstrainValues( float x, float minVal, float maxVal ) {

// 	// Check boundaries
// 	if ( x < minVal )
// 		return minVal;
// 	if ( x > maxVal )
// 		return maxVal;
// 	return x;
// }


// // float ControllerClass::CalculateDistance( cv::Point3f pt1, cv::Point3f pt2 ) {

// // 	float dx = pt1.x - pt2.x;
// // 	float dy = pt1.y - pt2.y;
// // 	float dz = pt1.z - pt2.z;

// // 	return std::sqrt( dx * dx + dy * dy + dz * dz );
// // }