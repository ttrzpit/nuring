#pragma once

// OpenCV core functions
#include <opencv2/core.hpp>

// Memory for shared data
#include <memory>

// Configuration
#include <config.h>

// Configuration
#include <config.h>

// Forward declarations
class SystemDataManager;
struct ManagedData;



/**
 * @brief Controller class containing individual controller
 * 
 */
class ControllerClass {

public:
	ControllerClass( SystemDataManager& dataHandle );

	// Public functions
	void Initialize( float T );
	void RunHybridController();


private:
	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;

	// Private functions
	void  ImpedanceController();
	float TauRaised( uint8_t power );
	void  CalculateTau();
	void  MinJerkReset();
	void  CalculateThetaDesired();
	void  GenerateAmplifierPWM();
	float ConstrainValues( float x, float minVal, float maxVal );

	// MJ variables
	float timeTrajectorySegmentT = 0.0f;
	float timeStart				 = 0.0f;
	float timeElapsed			 = 0.0f;
	float tau					 = 0.0f;
	float tauCoeff				 = 0.0f;	// Coefficient for MJ profile position
	float tauCoeffDot			 = 0.0f;	// Coefficient for MJ profile velocity

	// System properties
	float		radius				= 0.005f;							  // Pulley radius in [m]
	float		torqueConstant		= 14.4f;							  // Motor torque constant [nM*m/A]
	float		nominalCurrent		= 1.89f;							  // Motor nominal current
	cv::Point3f commandedCurrent	= cv::Point3f( 0.0f, 0.0f, 0.0f );	  // Commanded current
	cv::Point3i commandedPWM		= cv::Point3i( 0, 0, 0 );			  // Commanded PWM values
	cv::Point3f commandedPercentage = cv::Point3f( 0.0f, 0.0f, 0.0f );	  // Commanded percentage values

	// Initial state
	cv::Point3f positionInitial = cv::Point3f( 0.0f, 0.0f, 0.0f );	  // Initial MJ position
	cv::Point2f thetaInitial	= cv::Point2f( 0.0f, 0.0f );		  // Initial MJ angle

	// Desired minimum-jerk state
	cv::Point2f thetaDesired	= cv::Point2f( 0.0f, 0.0f );	// Desired MJ angle
	cv::Point2f thetaDotDesired = cv::Point2f( 0.0f, 0.0f );	// Desired MJ angular velocity

	// Measured state
	cv::Point3f positionMeasured = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Measured position from camera
	cv::Point3f velocityMeasured = cv::Point3f( 0.0f, 0.0f, 0.0f );	   // Measured position from camera
	cv::Point2f thetaMeasured	 = cv::Point2f( 0.0f, 0.0f );		   // Calulcated theta from measured position
	cv::Point2f thetaDotMeasured = cv::Point2f( 0.0f, 0.0f );		   // Calculated theta velocity from measured velocity

	// Virtual force
	cv::Point2f virtualForceXY	= cv::Point2f( 0.0f, 0.0f );		  // Calculated virtual force from impedance controller
	cv::Point3f virtualForceABC = cv::Point3f( 0.0f, 0.0f, 0.0f );	  // Mapped  virtual force from impedance controller

};
