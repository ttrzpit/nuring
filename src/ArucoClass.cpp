// Call to class header
#include "ArucoClass.h"

// System data manager
#include "SystemDataManager.h"

// Constructor
ArucoClass::ArucoClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	Initialize();
}



/**
 * @brief Initialize ArUco detector
 */
void ArucoClass::Initialize() {

	// Assign dictionary
	shared->arucoDictionary = cv::aruco::getPredefinedDictionary( cv::aruco::DICT_4X4_50 );

	// Assign ArUco detector parameters
	shared->arucoDetectorParams.adaptiveThreshConstant		  = 7;
	shared->arucoDetectorParams.adaptiveThreshWinSizeMin	  = 3;									// 3
	shared->arucoDetectorParams.adaptiveThreshWinSizeMax	  = 33;									// 23
	shared->arucoDetectorParams.adaptiveThreshWinSizeStep	  = 10;									// 10
	shared->arucoDetectorParams.minMarkerPerimeterRate		  = 0.02;								// 0.03
	shared->arucoDetectorParams.maxMarkerPerimeterRate		  = 4.0;								// 4.0
	shared->arucoDetectorParams.minCornerDistanceRate		  = 0.05;								// 0.05
	shared->arucoDetectorParams.minDistanceToBorder			  = 3;									// 3
	shared->arucoDetectorParams.cornerRefinementMethod		  = cv::aruco::CORNER_REFINE_SUBPIX;	// cv::aruco::CORNER_REFINE_SUBPIX;
	shared->arucoDetectorParams.cornerRefinementMaxIterations = 30;									// 30
	shared->arucoDetectorParams.cornerRefinementMinAccuracy	  = 0.1;								// 0.1

	// Re-initialize detector
	shared->arucoDetector = cv::aruco::ArucoDetector( shared->arucoDictionary, shared->arucoDetectorParams );

	// Configure real-world conversion system
	shared->arucoPoints.ptr<cv::Vec3f>( 0 )[0] = cv::Vec3f( -CONFIG_MARKER_WIDTH / 2.f, CONFIG_MARKER_WIDTH / 2.f, 0 );
	shared->arucoPoints.ptr<cv::Vec3f>( 0 )[1] = cv::Vec3f( CONFIG_MARKER_WIDTH / 2.f, CONFIG_MARKER_WIDTH / 2.f, 0 );
	shared->arucoPoints.ptr<cv::Vec3f>( 0 )[2] = cv::Vec3f( CONFIG_MARKER_WIDTH / 2.f, -CONFIG_MARKER_WIDTH / 2.f, 0 );
	shared->arucoPoints.ptr<cv::Vec3f>( 0 )[3] = cv::Vec3f( -CONFIG_MARKER_WIDTH / 2.f, -CONFIG_MARKER_WIDTH / 2.f, 0 );

	// Configure tag vector sizes
	shared->arucoTags.resize( 11 );

	// Update console
	std::cout << "ArucoClass:   ArUco detector initialized.\n";
}



/**
 * @brief Find ArUco tag markers
 */
void ArucoClass::FindTags() {

	if( shared->FLAG_FRAME_READY ) {


		// Reset global flag
		shared->FLAG_TAG_FOUND = false;

		// Reset individual tag state
		for( size_t t = 0; t < shared->arucoTags.size(); t++ ) {
			shared->arucoTags[t].present = false;
		}

		// Run detector
		shared->arucoDetector.detectMarkers( shared->matFrameGray, shared->arucoCorners, shared->arucoIDs, shared->arucoRejects );

		// Check if markers have been found
		if( !shared->arucoIDs.empty() ) {

			// Draw tags on frame
			cv::aruco::drawDetectedMarkers( shared->matFrameUndistorted, shared->arucoCorners, shared->arucoIDs );

			// Estimate tag pose
			cv::aruco::estimatePoseSingleMarkers( shared->arucoCorners, CONFIG_MARKER_WIDTH, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, shared->arucoRotationVector, shared->arucoTranslationVector );

			// Process each detected marker
			for( size_t i = 0; i < shared->arucoIDs.size(); i++ ) {

				// Shortcut to tag number
				short index = shared->arucoIDs[i];

				// Sort out markers in valid range
				if( shared->arucoIDs[i] > 0 && shared->arucoIDs[i] <= 11 ) {

					// Update global flag
					shared->FLAG_TAG_FOUND = true;

					// Update individual flag
					shared->arucoTags[index].present = true;

					// Calculate 2D pixel space
					int avgX = int( ( shared->arucoCorners[i][0].x + shared->arucoCorners[i][1].x + shared->arucoCorners[i][2].x + shared->arucoCorners[i][3].x ) / 4.0f );
					int avgY = int( ( shared->arucoCorners[i][0].y + shared->arucoCorners[i][1].y + shared->arucoCorners[i][2].y + shared->arucoCorners[i][3].y ) / 4.0f );


					// Update marker positions
					shared->arucoTags[index].error3D = cv::Point3i( int( shared->arucoTranslationVector[i][0] ), int( shared->arucoTranslationVector[i][1] ), int( shared->arucoTranslationVector[i][2] ) );
					shared->arucoTags[index].error2D = cv::Point2d( CONFIG_CAM_PRINCIPAL_X - avgX, CONFIG_CAM_PRINCIPAL_Y - avgY );
					shared->arucoTags[index].theta	 = shared->arucoRotationVector[i][1];
					shared->arucoTags[index].errorMagnitude3D =
						sqrt( ( shared->arucoTags[index].error3D.x * shared->arucoTags[index].error3D.x ) + ( shared->arucoTags[index].error3D.y * shared->arucoTags[index].error3D.y ) + ( shared->arucoTags[index].error3D.z * shared->arucoTags[index].error3D.z ) );
					shared->arucoTags[index].errorMagnitude2D	  = sqrt( ( shared->arucoTags[index].error2D.x * shared->arucoTags[index].error2D.x ) + ( shared->arucoTags[index].error2D.y * shared->arucoTags[index].error2D.y ) );
					shared->arucoTags[index].errorMagnitudeNorm2D = std::min( shared->arucoTags[index].errorMagnitude2D / ( CONFIG_DET_RADIUS / 100 ), 100 );
					shared->arucoTags[index].errorTheta			  = atan2( shared->arucoTags[index].error2D.y, -shared->arucoTags[index].error2D.x );

					// Constraint angle to positive values
					if( shared->arucoTags[index].errorTheta < 0.0f ) {
						shared->arucoTags[index].errorTheta = shared->arucoTags[index].errorTheta + ( 2.0f * 3.14159f );
					}

					// Update corners vector for active marker
					if( index == shared->arucoActiveID ) {
						shared->arucoActiveCorners[0] = cv::Point2i( shared->arucoCorners[i][0].x, shared->arucoCorners[i][0].y );
						shared->arucoActiveCorners[1] = cv::Point2i( shared->arucoCorners[i][1].x, shared->arucoCorners[i][1].y );
						shared->arucoActiveCorners[2] = cv::Point2i( shared->arucoCorners[i][2].x, shared->arucoCorners[i][2].y );
						shared->arucoActiveCorners[3] = cv::Point2i( shared->arucoCorners[i][3].x, shared->arucoCorners[i][3].y );
					}

					// Update area for each tag
					shared->arucoTags[index].area = cv::contourArea( shared->arucoCorners[i], true );

					// Draw frame axis
					cv::drawFrameAxes( shared->matFrameUndistorted, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, shared->arucoRotationVector[i], shared->arucoTranslationVector[i], CONFIG_MARKER_WIDTH, 2 );
				}
			}



		} else {

			// Update flag
			shared->FLAG_TAG_FOUND = false;
		}



	} else {
		std::cout << "ArucoClass:   Frame not ready.\n";
	}
}
