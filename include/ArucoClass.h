#pragma once

// Memory for shared data
#include <memory>

// OpenCV core functions
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>



// Forward declarations
class SystemDataManager;
struct ManagedData;




/** 
 * @brief Display class definition
 */
class ArucoClass {

public:
	// Data manager handle
	ArucoClass( SystemDataManager& dataHandle );

	// Public functions
	void FindTags();


private:
	// Private functions
	void Initialize();

	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;
};

class KalmanFilter3D {

public:
	cv::KalmanFilter kf;
	cv::Mat			 state;
	cv::Mat			 measurement;
	bool			 initialized = false;

	KalmanFilter3D() {
		kf.init( 6, 3, 0 );	   // 6 state vars: x, y, z, vx, vy, vz — 3 measurements: x, y, z

		// Initialize transition matrix (dt terms will be filled in during update)
		kf.transitionMatrix = ( cv::Mat_<float>( 6, 6 ) << 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 );

		// Measurement matrix — maps state [x, y, z, vx, vy, vz] to measured [x, y, z]
		cv::setIdentity( kf.measurementMatrix );

		// Tuning process noise: allow more uncertainty in velocity than position
		kf.processNoiseCov					 = cv::Mat::eye( 6, 6, CV_32F );
		kf.processNoiseCov.at<float>( 0, 0 ) = 1e-4;	// x
		kf.processNoiseCov.at<float>( 1, 1 ) = 1e-4;	// y
		kf.processNoiseCov.at<float>( 2, 2 ) = 1e-4;	// z
		kf.processNoiseCov.at<float>( 3, 3 ) = 1e-3;	// vx
		kf.processNoiseCov.at<float>( 4, 4 ) = 1e-3;	// vy
		kf.processNoiseCov.at<float>( 5, 5 ) = 1e-3;	// vz

		// Measurement noise — trust measurements fairly well
		cv::setIdentity( kf.measurementNoiseCov, cv::Scalar::all( 1e-3 ) );	   // 1e-2 trust less to smooth, 1e-3 trust more

		// Post-update error covariance
		cv::setIdentity( kf.errorCovPost, cv::Scalar::all( 1 ) );

		// Init state & measurement
		state		= cv::Mat::zeros( 6, 1, CV_32F );
		measurement = cv::Mat::zeros( 3, 1, CV_32F );
	}


	void reset( const cv::Point3f& initial ) {
		state.at<float>( 0 ) = initial.x;
		state.at<float>( 1 ) = initial.y;
		state.at<float>( 2 ) = initial.z;
		state.at<float>( 3 ) = 0;
		state.at<float>( 4 ) = 0;
		state.at<float>( 5 ) = 0;
		kf.statePost		 = state.clone();
		initialized			 = true;
	}

	void update( const cv::Point3f& pos, float dt ) {
		if ( !initialized )
			reset( pos );

		// Update dynamic transition matrix
		kf.transitionMatrix.at<float>( 0, 3 ) = dt;
		kf.transitionMatrix.at<float>( 1, 4 ) = dt;
		kf.transitionMatrix.at<float>( 2, 5 ) = dt;

		measurement.at<float>( 0 ) = pos.x;
		measurement.at<float>( 1 ) = pos.y;
		measurement.at<float>( 2 ) = pos.z;

		kf.predict();
		kf.correct( measurement );
		state = kf.statePost;
	}


	cv::Point3f getPosition() const {
		return { state.at<float>( 0 ), -1 * state.at<float>( 1 ), state.at<float>( 2 ) };
	}
	cv::Point3f getVelocity() const {
		return { state.at<float>( 3 ), -1 * state.at<float>( 4 ), state.at<float>( 5 ) };
	}
};
