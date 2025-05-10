// Call to class header
#include "ArucoClass.h"

// System data manager
#include "SystemDataManager.h"

/**
 * @brief Construct a new Aruco Class:: Aruco Class object
 * 
 * @param ctx Handle to data manager
 */
ArucoClass::ArucoClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	Initialize();
}


/**
 * @brief Initialize ArUco detector parameters
 * 
 */
void ArucoClass::Initialize() {

	// Assign dictionary
	arucoDictionary = cv::aruco::getPredefinedDictionary( cv::aruco::DICT_4X4_50 );

	// Assign ArUco detector parameters
	arucoDetectorParams.adaptiveThreshConstant		  = 7;									// 7
	arucoDetectorParams.adaptiveThreshWinSizeMin	  = 3;									// 3  KEEP THIS
	arucoDetectorParams.adaptiveThreshWinSizeMax	  = 33;									// 13	KEEP THIS
	arucoDetectorParams.adaptiveThreshWinSizeStep	  = 10;									// 10
	arucoDetectorParams.minMarkerPerimeterRate		  = 0.03;								// 0.03
	arucoDetectorParams.maxMarkerPerimeterRate		  = 4.0;								// 4.0 Not critical
	arucoDetectorParams.polygonalApproxAccuracyRate	  = 0.1;								// 0.15
	arucoDetectorParams.minCornerDistanceRate		  = 0.05;								// 0.05
	arucoDetectorParams.minDistanceToBorder			  = 3;									// 3
	arucoDetectorParams.cornerRefinementMethod		  = cv::aruco::CORNER_REFINE_SUBPIX;	// cv::aruco::CORNER_REFINE_SUBPIX;
	arucoDetectorParams.cornerRefinementMaxIterations = 30;									// 30
	arucoDetectorParams.cornerRefinementMinAccuracy	  = 0.1;								// 0.1

	// Re-initialize detector
	arucoDetector = cv::aruco::ArucoDetector( arucoDictionary, arucoDetectorParams );

	// Configure real-world conversion system
	arucoPoints.ptr<cv::Vec3f>( 0 )[0] = cv::Vec3f( -CONFIG_MARKER_WIDTH / 2.f, CONFIG_MARKER_WIDTH / 2.f, 0 );
	arucoPoints.ptr<cv::Vec3f>( 0 )[1] = cv::Vec3f( CONFIG_MARKER_WIDTH / 2.f, CONFIG_MARKER_WIDTH / 2.f, 0 );
	arucoPoints.ptr<cv::Vec3f>( 0 )[2] = cv::Vec3f( CONFIG_MARKER_WIDTH / 2.f, -CONFIG_MARKER_WIDTH / 2.f, 0 );
	arucoPoints.ptr<cv::Vec3f>( 0 )[3] = cv::Vec3f( -CONFIG_MARKER_WIDTH / 2.f, -CONFIG_MARKER_WIDTH / 2.f, 0 );

	// Configure tag vector sizes
	arucoTagsPresent.resize( 11 );

	// Update console
	std::cout << "ArucoClass:   ArUco detector initialized.\n";
}



/**
 * @brief Find ArUco tag markers
 */
void ArucoClass::FindTags() {

	// Make sure image is ready to be processed
	if ( shared->FLAG_FRAME_READY ) {

		// Reset global flag
		shared->FLAG_TARGET_FOUND = false;

		// Reset individual tag state
		for ( size_t t = 0; t < arucoTagsPresent.size(); t++ ) {
			arucoTagsPresent[t] = false;
		}

		// Run detector
		arucoDetector.detectMarkers( shared->matFrameGray, arucoCorners, arucoDetectedIDs, arucoRejects );

		// Check if markers have been found
		if ( !arucoDetectedIDs.empty() ) {

			// Draw tags on frame
			// cv::aruco::drawDetectedMarkers( shared->matFrameUndistorted, shared->arucoCorners, shared->arucoIDs );

			// Estimate tag pose
			cv::aruco::estimatePoseSingleMarkers( arucoCorners, CONFIG_MARKER_WIDTH, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, arucoRotationVector, arucoTranslationVector );

			// Process each detected marker
			for ( int i = 0; i < arucoDetectedIDs.size(); i++ ) {

				// Shortcut to tag number
				short ID = arucoDetectedIDs[i];

				// Sort out markers in valid range
				if ( ( ID > 0 && ID <= 5 ) || ( ID == 8 ) ) {

					// Only process active ID
					if ( ID == shared->targetActiveID ) {

						// Update flag
						shared->FLAG_TARGET_FOUND = true;

						// Update 2D pixel coordinates
						int avgX					  = int( ( arucoCorners[i][0].x + arucoCorners[i][1].x + arucoCorners[i][2].x + arucoCorners[i][3].x ) / 4.0f );
						int avgY					  = int( ( arucoCorners[i][0].y + arucoCorners[i][1].y + arucoCorners[i][2].y + arucoCorners[i][3].y ) / 4.0f );
						shared->targetScreenPosition  = cv::Point2i( CONFIG_CAM_PRINCIPAL_X - avgX, CONFIG_CAM_PRINCIPAL_Y - avgY );
						shared->targetScreenMagnitude = shared->GetNorm2D( shared->targetScreenPosition );

						// Update 2D corner vector for active marker
						shared->targetCorners[0] = cv::Point2i( arucoCorners[i][0].x, arucoCorners[i][0].y );
						shared->targetCorners[1] = cv::Point2i( arucoCorners[i][1].x, arucoCorners[i][1].y );
						shared->targetCorners[2] = cv::Point2i( arucoCorners[i][2].x, arucoCorners[i][2].y );
						shared->targetCorners[3] = cv::Point2i( arucoCorners[i][3].x, arucoCorners[i][3].y );

						// Update 3D real-world coordinates
						shared->targetPosition3dRaw = cv::Point3f( arucoTranslationVector[i][0], arucoTranslationVector[i][1], arucoTranslationVector[i][2] );

						// Update rotation and translation vectors
						shared->targetRotationVector	= arucoRotationVector[ID];
						shared->targetTranslationVector = arucoTranslationVector[ID];

						// Calculate area for active tag
						// shared->arucoTags[index].area = cv::contourArea( shared->arucoCorners[i], true );

						// Save information for frame axis
						// cv::drawFrameAxes( shared->matFrameUndistorted, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, arucoRotationVector[ID], arucoTranslationVector[ID], CONFIG_MARKER_WIDTH, 2 );

					} else {

						// Update flag
						shared->FLAG_TARGET_FOUND = false;
					}	 // End process active ID

				} else {

					shared->FLAG_TARGET_FOUND = false;
					// std::cout << "ArucoClass:   Detected marker out of range.\n";

				}	 // End sort out markers in active range
			}	 // End for loop

		}	 // End is detectedIDs empty

	} else {

		// Display error message
		std::cout << "ArucoClass: Image not ready for processing!\n";

	}	 // End is frame ready

}	 // End function
