#pragma once

// Memory for shared data
#include <memory>

// OpenCV
#include <opencv2/core.hpp>

// Configuration
#include <config.h>


class SystemDataManager;
struct ManagedData;

/**
 * @brief Create a new kalman filter object
 * 
 */
class KalmanClass {
public:
	/**
	 * @brief Construct a new Kalman Class object
	 * 
	 * @param ctx Datahandle 
	 */
	KalmanClass( SystemDataManager& ctx );

	// Public methods
	void		Initialize( const cv::Point3f& initialPos, float tInitial );	// Initialize filter
	void		Update( const cv::Point3f& measuredPos, float tCurrent );		// Update filter
	cv::Point3f GetPosition() const;											// Get filtered position
	cv::Point3f GetVelocity() const;											// Get filtered velocity
	cv::Point2f GetAngle() const;												// Get filtered angle
	cv::Point2f GetAnglularVelocity() const;									// Get filtered angular velocity

	void		   InitializeAngle( float measuredAngle, float tInitial );
	void		   UpdateAngle( float measuredAngle, float tCurrent );
	float		   GetFilteredAngle() const;
	float		   GetFilteredAngularVelocity() const;
	const cv::Mat& GetCovariance() const;



private:
	// Data manager handle
	SystemDataManager&			 dataHandle;
	std::shared_ptr<ManagedData> shared;

	// Private matrices
	cv::Mat state;												// [x y z vx vy vz]
	cv::Mat P = cv::Mat::eye( 6, 6, CV_32F ) * 1.0f;	// Covariance
	cv::Mat Q;													// Process noise
	cv::Mat R;													// Measurement noise
	cv::Mat H;													// Measurement matrix
	cv::Mat I;													// Identity

	// Measurement matrices
	cv::Mat z;
	cv::Mat y;
	cv::Mat S;
	cv::Mat K;
	cv::Mat F;

	// Private variables
	float tPrevious		= 0.0f;		// Previous timestamp
	bool  isInitialized = false;	// Check if filter is initialized
	float dt			= 0.0f;		// Timestep
};