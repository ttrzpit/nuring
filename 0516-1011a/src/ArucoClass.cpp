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
	arucoDetectorParams.adaptiveThreshWinSizeMax	  = 39;									// 13	KEEP THIS
	arucoDetectorParams.adaptiveThreshWinSizeStep	  = 10;									// 10
	arucoDetectorParams.minMarkerPerimeterRate		  = 0.01;								// 0.03
	arucoDetectorParams.maxMarkerPerimeterRate		  = 4.0;								// 4.0 Not critical
	arucoDetectorParams.polygonalApproxAccuracyRate	  = 0.1;								// 0.15
	arucoDetectorParams.minCornerDistanceRate		  = 0.05;								// 0.05
	arucoDetectorParams.minDistanceToBorder			  = 3;									// 3
	arucoDetectorParams.cornerRefinementMethod		  = cv::aruco::CORNER_REFINE_SUBPIX;	// cv::aruco::CORNER_REFINE_SUBPIX;
	arucoDetectorParams.cornerRefinementMaxIterations = 30;									// 30
	arucoDetectorParams.cornerRefinementMinAccuracy	  = 0.01;								// 0.1

	// Re-initialize detector
	arucoDetector = cv::aruco::ArucoDetector( arucoDictionary, arucoDetectorParams );

	// Configure real-world conversion system
	arucoPoints.ptr<cv::Vec3f>( 0 )[0] = cv::Vec3f( -CONFIG_LARGE_MARKER_WIDTH / 2.f, CONFIG_LARGE_MARKER_WIDTH / 2.f, 0 );
	arucoPoints.ptr<cv::Vec3f>( 0 )[1] = cv::Vec3f( CONFIG_LARGE_MARKER_WIDTH / 2.f, CONFIG_LARGE_MARKER_WIDTH / 2.f, 0 );
	arucoPoints.ptr<cv::Vec3f>( 0 )[2] = cv::Vec3f( CONFIG_LARGE_MARKER_WIDTH / 2.f, -CONFIG_LARGE_MARKER_WIDTH / 2.f, 0 );
	arucoPoints.ptr<cv::Vec3f>( 0 )[3] = cv::Vec3f( -CONFIG_LARGE_MARKER_WIDTH / 2.f, -CONFIG_LARGE_MARKER_WIDTH / 2.f, 0 );

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

		// Reset global flags
		shared->FLAG_TARGET_MARKER_FOUND = false;
		shared->FLAG_FINGER_MARKER_FOUND = false;

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

			// Estimate tag pose, moved this into individual tags to only process the markers in the valid range
			// cv::aruco::estimatePoseSingleMarkers( arucoCorners, CONFIG_MARKER_WIDTH, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, arucoRotationVector, arucoTranslationVector );

			// shared->FLAG_FINGER_MARKER_FOUND = false;

			// Process each detected marker
			for ( int i = 0; i < arucoDetectedIDs.size(); i++ ) {

				// Process finger marker
				if ( arucoDetectedIDs[i] == 0 ) {	 // Process finger marker

					// Set correct marker size
					arucoMarkerSize = CONFIG_MEDIUM_MARKER_WIDTH;

					// Extract current corner
					std::vector<std::vector<cv::Point2f>> currentCorner = { arucoCorners[i] };

					// Estimate tag pose formarkers in the valid range
					cv::aruco::estimatePoseSingleMarkers( currentCorner, arucoMarkerSize, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, arucoRotationVector, arucoTranslationVector );
					if ( arucoTranslationVector.empty() )
						continue;

					// Update flag
					shared->FLAG_FINGER_MARKER_FOUND	  = true;
					arucoTagsPresent[arucoDetectedIDs[i]] = true;

					// Update 2D pixel coordinates
					int avgX						   = int( ( currentCorner[0][0].x + currentCorner[0][1].x + currentCorner[0][2].x + currentCorner[0][3].x ) / 4.0f );
					int avgY						   = int( ( currentCorner[0][0].y + currentCorner[0][1].y + currentCorner[0][2].y + currentCorner[0][3].y ) / 4.0f );
					shared->fingerMarkerScreenPosition = cv::Point2i( avgX, avgY );

					// Update 2D corner vector for active marker
					shared->fingerMarkerCorners[0] = cv::Point2i( currentCorner[0][0].x, currentCorner[0][0].y );
					shared->fingerMarkerCorners[1] = cv::Point2i( currentCorner[0][1].x, currentCorner[0][1].y );
					shared->fingerMarkerCorners[2] = cv::Point2i( currentCorner[0][2].x, currentCorner[0][2].y );
					shared->fingerMarkerCorners[3] = cv::Point2i( currentCorner[0][3].x, currentCorner[0][3].y );

					// Update 3D real-world coordinates
					shared->fingerMarkerPosition3DRaw = cv::Point3f( arucoTranslationVector[0][0], -arucoTranslationVector[0][1], arucoTranslationVector[0][2] );

					// Update rotation and translation vectors
					shared->fingerMarkerRotationVector	  = arucoRotationVector[0];
					shared->fingerMarkerTranslationVector = arucoTranslationVector[0];
				}

				// Sort out markers in valid range
				if ( ( arucoDetectedIDs[i] > 0 && arucoDetectedIDs[i] <= 5 ) || ( arucoDetectedIDs[i] == 8 ) ) {

					if ( ( arucoDetectedIDs[i] > 0 ) && ( arucoDetectedIDs[i] == shared->targetMarkerActiveID ) ) {	   // Only process active tag

						// Set correct marker size
						arucoMarkerSize = CONFIG_LARGE_MARKER_WIDTH;

						// Extract current corner
						std::vector<std::vector<cv::Point2f>> currentCorner = { arucoCorners[i] };

						// Estimate tag pose formarkers in the valid range
						cv::aruco::estimatePoseSingleMarkers( currentCorner, arucoMarkerSize, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, arucoRotationVector, arucoTranslationVector );
						if ( arucoTranslationVector.empty() )
							continue;

						// Update flag
						shared->FLAG_TARGET_MARKER_FOUND	  = true;
						arucoTagsPresent[arucoDetectedIDs[i]] = true;

						// Update 2D pixel coordinates
						int avgX						   = int( ( currentCorner[0][0].x + currentCorner[0][1].x + currentCorner[0][2].x + currentCorner[0][3].x ) / 4.0f );
						int avgY						   = int( ( currentCorner[0][0].y + currentCorner[0][1].y + currentCorner[0][2].y + currentCorner[0][3].y ) / 4.0f );
						shared->targetMarkerScreenPosition = cv::Point2i( avgX, avgY );

						// Update 2D corner vector for active marker
						shared->targetMarkerCorners[0] = cv::Point2i( currentCorner[0][0].x, currentCorner[0][0].y );
						shared->targetMarkerCorners[1] = cv::Point2i( currentCorner[0][1].x, currentCorner[0][1].y );
						shared->targetMarkerCorners[2] = cv::Point2i( currentCorner[0][2].x, currentCorner[0][2].y );
						shared->targetMarkerCorners[3] = cv::Point2i( currentCorner[0][3].x, currentCorner[0][3].y );

						// Update 3D real-world coordinates
						shared->targetMarkerPosition3dRaw = cv::Point3f( arucoTranslationVector[0][0], -arucoTranslationVector[0][1], arucoTranslationVector[0][2] );

						// Update rotation and translation vectors
						shared->targetMarkerRotationVector	  = arucoRotationVector[0];
						shared->targetMarkerTranslationVector = arucoTranslationVector[0];

						// Calculate area for active tag
						// shared->arucoTags[index].area = cv::contourArea( shared->arucoCorners[i], true );

						// Save information for frame axis
						// cv::drawFrameAxes( shared->matFrameUndistorted, CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, arucoRotationVector[ID], arucoTranslationVector[ID], CONFIG_MARKER_WIDTH, 2 );
					}


				}	 // End process valid marker
			}	 // For loop

		}	 // End empty check
	} else {

		// Display error message
		std::cout << "ArucoClass: Image not ready for processing!\n";

	}	 // End is frame ready

}	 // End function
