// Call to class header
#include "KalmanClass.h"

// System data manager
#include "SystemDataManager.h"

// Math
#include <cmath>

/**
 * @brief Construct a new Kalman Class:: Kalman Class object
 * 
 * @param ctx 
 */
KalmanClass::KalmanClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() )
	, isInitialized( false ) {

	// Build matrices
	state = cv::Mat::zeros( 6, 1, CV_32F );
	P	  = cv::Mat::eye( 6, 6, CV_32F ) * 1.0f;
	Q	  = cv::Mat::eye( 6, 6, CV_32F ) * 0.01f;
	R	  = cv::Mat::eye( 6, 6, CV_32F ) * 10.0f;
	H	  = cv::Mat::zeros( 3, 6, CV_32F );
	I	  = cv::Mat::eye( 6, 6, CV_32F );

	// Only position is measured
	H.at<float>( 0, 0 ) = 1.0f;
	H.at<float>( 1, 1 ) = 1.0f;
	H.at<float>( 2, 2 ) = 1.0f;
}



/**
 * @brief Initialize kalman filter
 * 
 * @param initialPos        Initial position of system
 * @param initialTimestamp  Initial timestamp
 */
void KalmanClass::Initialize( const cv::Point3f& initialPos, float tInitial ) {

	// Populate state
	state.at<float>( 0 ) = initialPos.x;	// Position
	state.at<float>( 1 ) = initialPos.y;
	state.at<float>( 2 ) = initialPos.z;
	state.at<float>( 3 ) = 0.0f;	// Velocity
	state.at<float>( 4 ) = 0.0f;
	state.at<float>( 5 ) = 0.0f;

	// Update timestamp
	tPrevious = tInitial;

	// Update flag
	isInitialized = true;
}



/**
 * @brief Update kalman filter
 * 
 * @param measuredPos       Measured position
 * @param currentTimestamp  Updated timestamp
 */
void KalmanClass::Update( const cv::Point3f& measuredPos, float tCurrent ) {

	// Make sure the filter is initialized
	if ( !isInitialized ) {

		// Initialize
		Initialize( measuredPos, tCurrent );

		// Break out of method
		return;
	}

	// Update timestep
	dt		  = tCurrent - tPrevious;
	tPrevious = tCurrent;

	// State transition matrix
	cv::Mat F			= cv::Mat::eye( 6, 6, CV_32F );
	F.at<float>( 0, 3 ) = dt;
	F.at<float>( 1, 4 ) = dt;
	F.at<float>( 2, 5 ) = dt;

	// Predict state
	state = F * state;
	P	  = F * P * F.t() + Q;

	// Measurement
	cv::Mat z = ( cv::Mat_<float>( 3, 1 ) << measuredPos.x, measuredPos.y, measuredPos.z );
	cv::Mat y = z - H * state;
	cv::Mat S = H * P * H.t() + R;
	cv::Mat K = P * H.t() * S.inv();

	// Update
	state = state + K * y;
	P	  = ( I - K * H ) * P;
}



/**
 * @brief Get filtered position
 * 
 * @return cv::Point3f 3D position
 */
cv::Point3f KalmanClass::GetPosition() const {
	return cv::Point3f( state.at<float>( 0 ), state.at<float>( 1 ), state.at<float>( 2 ) );
}



/**
 * @brief Get filtered velocity
 * 
 * @return cv::Point3f 
 */
cv::Point3f KalmanClass::GetVelocity() const {
	return cv::Point3f( state.at<float>( 3 ), state.at<float>( 4 ), state.at<float>( 5 ) );
}



/**
 * @brief Get angle of error
 * 
 * @return cv::Point2f 
 */
cv::Point2f KalmanClass::GetAngle() const {

	// Get position
	cv::Point3f pos = GetPosition();

	// Calculate angle
	return cv::Point2f( atan2( pos.x, pos.z ), atan2( pos.y, pos.z ) );
}


/**
 * @brief Get filtered angular velocity
 * 
 * @return cv::Point2f 
 */
cv::Point2f KalmanClass::GetAnglularVelocity() const {

	// Get position and velocity
	cv::Point3f pos = GetPosition();
	cv::Point3f vel = GetVelocity();

	// Calculate angular velocity
	float dz	  = pos.z != 0.0f ? pos.z : 1e-6f;
	float dThetaX = ( vel.x * dz - pos.x * vel.z ) / ( dz * dz + pos.x * pos.x );
	float dThetaY = ( vel.y * dz - pos.y * vel.z ) / ( dz * dz + pos.y * pos.y );

	// Return filtered angular velocity
	return cv::Point2f( dThetaX, dThetaY );
}
