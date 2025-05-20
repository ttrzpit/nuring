#include "ControllerClass.h"
#include "SystemDataManager.h"

ControllerClass::ControllerClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	//
}


void ControllerClass::Update() {

	if ( shared->TASK_NAME == "FITTS_ANGLE" ) {


		// Motor vectors
		cv::Point2f motorA( COS35, SIN35 );
		cv::Point2f motorB( COS145, SIN145 );
		cv::Point2f motorC( COS270, SIN270 );

		// Calculate error angle for motor selection
		float angle = shared->angleDesired - shared->angleFiltered;
		float r		= 100.0f;
		float rMax	= 0.0f;
		float thA	= 35.0f * DEG2RAD;
		float thB	= 145.0f * DEG2RAD;
		float thC	= 270.0f * DEG2RAD;
		float thAT	= 0.0f;
		float thBT	= 0.0f;
		float thCT	= 0.0f;
		float thAB	= 110.0f * DEG2RAD;
		float thBC	= 125.0f * DEG2RAD;
		float thAC	= 125.0f * DEG2RAD;

		cv::Point3f contribution( 0, 0, 0 );


		std::cout << "eTheta: " << shared->FormatDecimal( angle, 2 ) << "\n";

		// OLD
		// float eTheta = shared->angleDesired - shared->angleFiltered;
		// // Calculate controller values based on error & gains
		// // shared->controllerXYProportional = cv::Point2f( shared->controllerKp.x * -0.1f * ( eTheta ), shared->controllerKp.y * 0.1f * 0 );
		// // shared->controllerXYDerivative	 = cv::Point2f( shared->controllerKd.x * 10.0f * ( shared->angleVelocity ), 0 );
		// // shared->controllerXYTotal		 = shared->controllerXYProportional + shared->controllerXYDerivative;

		// // Assuming unit vectors for each motor direction
		// const cv::Point2f uA = cv::Point2f( COS35, SIN35 );
		// const cv::Point2f uB = cv::Point2f( COS145, SIN145 );
		// const cv::Point2f uC = cv::Point2f( COS270, SIN270 );

		// // Desired XY torque
		// cv::Point2f F_des = shared->controllerXYTotal;	  // e.g., from PID

		// // Project desired torque onto each motor direction
		// shared->controllerTorqueABC.x = F_des.dot( uA );
		// shared->controllerTorqueABC.y = F_des.dot( uB );
		// shared->controllerTorqueABC.z = abs( F_des.dot( uC ) ) * -1.0f;

		// // // Map to motor values
		// // shared->controllerTorqueABC.x = std::max( ( shared->controllerXYTotal.x * COS35 ) + ( shared->controllerXYTotal.y * SIN35 ), 0.0 );
		// // shared->controllerTorqueABC.y = std::max( ( shared->controllerXYTotal.x * COS145 ) + ( shared->controllerXYTotal.y * SIN145 ), 0.0 );
		// // shared->controllerTorqueABC.z = std::max( ( shared->controllerXYTotal.y * SIN270 ), 0.0 );

		// // // Add tension component and map to current
		// shared->controllerCurrent	 = ( shared->controllerTension + shared->controllerTorqueABC ) / 14.4;
		// shared->controllerPercentage = shared->controllerCurrent / nominalCurrent;

		// // Map to PWM
		// shared->controllerPWM.x = std::clamp( 2048 - int( ( shared->controllerCurrent.x / nominalCurrent ) * 2047 ), 1, 2048 );
		// shared->controllerPWM.y = std::clamp( 2048 - int( ( shared->controllerCurrent.y / nominalCurrent ) * 2047 ), 1, 2048 );
		// shared->controllerPWM.z = std::clamp( 2048 - int( ( shared->controllerCurrent.z / nominalCurrent ) * 2047 ), 1, 2048 );
	} else {

		// Motor vectors
		cv::Point2f motorA( COS35, SIN35 );
		cv::Point2f motorB( COS145, SIN145 );
		cv::Point2f motorC( COS270, SIN270 );

		// Calculate error angle for motor selection
		float angle = std::atan2( shared->targetMarkerPosition3dNew.y, shared->targetMarkerPosition3dNew.x );
		float r		= cv::norm( cv::Vec2f( shared->targetMarkerPosition3dNew.x, shared->targetMarkerPosition3dNew.y ) );
		float rMax	= 0.0f;
		float thA	= 35.0f * DEG2RAD;
		float thB	= 145.0f * DEG2RAD;
		float thC	= 270.0f * DEG2RAD;
		float thAT	= 0.0f;
		float thBT	= 0.0f;
		float thCT	= 0.0f;
		float thAB	= 110.0f * DEG2RAD;
		float thBC	= 125.0f * DEG2RAD;
		float thAC	= 125.0f * DEG2RAD;

		cv::Point3f contribution( 0, 0, 0 );

		// Wrap angle within 0-360 deg
		if ( angle < 0 ) {
			angle += ( 360.0f * DEG2RAD );
		}

		// Calculate max radius
		rMax = std::clamp( float( std::atan2( r, shared->targetMarkerPosition3dNew.z ) ), 0.0f, float( 40.0 * DEG2RAD ) ) / ( 40.0 * DEG2RAD ) * shared->controllerKp.x;

		// Determine angle and solve
		if ( angle >= ( 270 * DEG2RAD ) || angle < ( 35 * DEG2RAD ) ) {

			// Wrap-around for 360/0 deg, calculate angles AC
			if ( angle < thA ) {
				thAT = thA - angle;
				thCT = thAC - thAT;
			}
			if ( angle > thC ) {
				thCT = angle - thC;
				thAT = thAC - thCT;
			}

			// rMax		   = rMax * RAD2DEG;
			contribution.x = ( 1.0f - ( thAT / thAC ) ) * rMax;
			contribution.z = ( 1.0f - ( thCT / thAC ) ) * rMax;

			// Debug
			std::cout << "|A+C| Angle: " << shared->FormatDecimal( angle * RAD2DEG, 2 ) << "   Contrib: A: " << shared->FormatDecimal( contribution.x, 2 ) << " , C: " << shared->FormatDecimal( contribution.z, 2 ) << "   rMax: " << shared->FormatDecimal( rMax, 2 ) << "\n";

		} else if ( angle >= ( 35 * DEG2RAD ) && angle < ( 145 * DEG2RAD ) ) {

			// Calculate angles AB
			thAT = angle - thA;
			thBT = thAB - thAT;

			contribution.x = ( 1.0f - ( thAT / thAB ) ) * rMax;
			contribution.y = ( 1.0f - ( thBT / thAB ) ) * rMax;

			std::cout << "|A+B| Angle: " << shared->FormatDecimal( angle * RAD2DEG, 2 ) << "   Contrib: A: " << shared->FormatDecimal( contribution.x, 2 ) << " , B: " << shared->FormatDecimal( contribution.y, 2 ) << "   rMax: " << shared->FormatDecimal( rMax, 2 ) << "\n";

		} else if ( angle >= ( 145 * DEG2RAD ) && angle < ( 270 * DEG2RAD ) ) {

			// Calculate angles BC
			thBT = angle - thB;
			thCT = thBC - thBT;

			contribution.y = ( 1.0f - ( thBT / thBC ) ) * rMax;
			contribution.z = ( 1.0f - ( thCT / thBC ) ) * rMax;

			std::cout << "|B+C| Angle: " << shared->FormatDecimal( angle * RAD2DEG, 2 ) << "   Contrib: B: " << shared->FormatDecimal( contribution.y, 2 ) << " , C: " << shared->FormatDecimal( contribution.z, 2 ) << "   rMax: " << shared->FormatDecimal( rMax, 2 ) << "\n";
		}

		// Multiply with gains
		shared->controllerPercentage = cv::Point3f( contribution );

		// std::cout << "MotorA: " << shared->controllerPercentage.x << "  MotorB: " << shared->controllerPercentage.y << "  MotorC: " << shared->controllerPercentage.z << "\n";

		// Motor force vector
		// cv::Vec3f F;

		// // Compute pseudo-inverse
		// cv::Mat pinvM;
		// cv::invert( M, pinvM, cv::DECOMP_SVD );	   // pinvM is 3x2

		// // Compute solution
		// cv::Mat Fmat = pinvM * f;

		// // Convert to motor force vector
		// cv::Vec3f F( Fmat );

		// // Solve for F
		// // cv::solve( M, f, F, cv::DECOMP_SVD );

		// // Share to torque
		// shared->controllerTorqueABC = cv::Point3f( F( 0 ), F( 1 ), F( 2 ) );

		// No tension
		// shared->controllerCurrent	 = ( shared->controllerTorqueABC ) / 14.4;
		// shared->controllerPercentage = shared->controllerCurrent / nominalCurrent;

		// Map to PWM
		shared->controllerPWM.x = std::clamp( 2048 - int( ( shared->controllerPercentage.x ) * 2047 ), 1, 2048 );
		shared->controllerPWM.y = std::clamp( 2048 - int( ( shared->controllerPercentage.y ) * 2047 ), 1, 2048 );
		shared->controllerPWM.z = std::clamp( 2048 - int( ( shared->controllerPercentage.z ) * 2047 ), 1, 2048 );

		// std::cout << "MotorA: " << shared->controllerTorqueABC.x << "  MotorB: " << shared->controllerTorqueABC.y << "  MotorC: " << shared->controllerTorqueABC.z << "\n";



		// // Old
		// // Calculate controller values based on error & gains
		// shared->controllerXYProportional = cv::Point2f( shared->controllerKp.x * ( shared->targetMarkerAngleNew.x ), shared->controllerKp.y * ( shared->targetMarkerAngleNew.y ) );
		// shared->controllerXYDerivative	 = cv::Point2f( shared->controllerKd.x * ( shared->targetMarkerAnglularVelocityNew.x ), shared->controllerKd.y * ( shared->targetMarkerAnglularVelocityNew.y ) );
		// shared->controllerXYTotal		 = shared->controllerXYProportional + shared->controllerXYDerivative;


		// shared->controllerXYProportional = cv::Point2f( shared->controllerKp.x * ( shared->targetMarkerAngleNew.x ), shared->controllerKp.y * ( shared->targetMarkerAngleNew.y ) );
		// shared->controllerXYDerivative	 = cv::Point2f( shared->controllerKd.x * ( shared->targetMarkerAnglularVelocityNew.x ), shared->controllerKd.y * ( shared->targetMarkerAnglularVelocityNew.y ) );
		// shared->controllerXYTotal		 = shared->controllerXYProportional + shared->controllerXYDerivative;


		// // Map to motor values
		// shared->controllerTorqueABC.x = std::max( ( shared->controllerXYTotal.x * COS35 ) + ( shared->controllerXYTotal.y * SIN35 ), 0.0 );
		// shared->controllerTorqueABC.y = std::max( ( shared->controllerXYTotal.x * COS145 ) + ( shared->controllerXYTotal.y * SIN145 ), 0.0 );
		// shared->controllerTorqueABC.z = std::max( ( shared->controllerXYTotal.y * SIN270 ), 0.0 );

		// // Add tension component and map to current
		// shared->controllerCurrent	 = ( shared->controllerTension + shared->controllerTorqueABC ) / 14.4;
		// shared->controllerCurrent	 = ( shared->controllerTension + shared->controllerTorqueABC ) / 14.4;
		// shared->controllerPercentage = shared->controllerCurrent / nominalCurrent;

		// // // Map to PWM
		// shared->controllerPWM.x = std::clamp( 2048 - int( ( shared->controllerCurrent.x / nominalCurrent ) * 2047 ), 1, 2048 );
		// shared->controllerPWM.y = std::clamp( 2048 - int( ( shared->controllerCurrent.y / nominalCurrent ) * 2047 ), 1, 2048 );
		// shared->controllerPWM.z = std::clamp( 2048 - int( ( shared->controllerCurrent.z / nominalCurrent ) * 2047 ), 1, 2048 );
	}
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