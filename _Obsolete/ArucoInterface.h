#pragma once

#include "config.h"

// OpenCV libraries
#include <opencv2/aruco.hpp>
#include <opencv2/calib3d/calib3d.hpp> // For undistort
#include <opencv2/core.hpp> // Core OpenCV functions
#include <opencv2/objdetect/aruco_detector.hpp> // For AruCo tags

class ArucoInterface
{

public:
	// Constructor
	ArucoInterface()
	{
		// Begin() ;
	}

	// Variables
	bool MarkersFound = false;

	// Function prototypes
	bool Begin();
	void Detect(cv::Mat& mat);

private:
	cv::aruco::Dictionary				  dictionary;
	cv::aruco::DetectorParameters		  detectorParams;
	cv::aruco::ArucoDetector			  arucoDetector;
	std::vector<int>					  markerIDs;
	std::vector<std::vector<cv::Point2f>> markerCorners, markerRejects;
	std::vector<cv::Vec3d>				  rotationVector, translationVector;
	std::vector<int>					  markersPresent = {0, 0, 0, 0, 0, 0};
	cv::Mat								  objPoints{4, 1, CV_32FC3};
};

bool ArucoInterface::Begin()
{

	// Define aruco parameters
	dictionary	   = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
	detectorParams = cv::aruco::DetectorParameters();

	// Configure real-world conversion system
	objPoints.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-CONFIG_MARKER_WIDTH / 2.f, CONFIG_MARKER_WIDTH / 2.f, 0);
	objPoints.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(CONFIG_MARKER_WIDTH / 2.f, CONFIG_MARKER_WIDTH / 2.f, 0);
	objPoints.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(CONFIG_MARKER_WIDTH / 2.f, -CONFIG_MARKER_WIDTH / 2.f, 0);
	objPoints.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-CONFIG_MARKER_WIDTH / 2.f, -CONFIG_MARKER_WIDTH / 2.f, 0);

	// Adjust ArUco detector parameters
	detectorParams.adaptiveThreshWinSizeMin		 = 3; // 3
	detectorParams.adaptiveThreshWinSizeMax		 = 23; // 23
	detectorParams.adaptiveThreshWinSizeStep	 = 10; // 10
	detectorParams.minMarkerPerimeterRate		 = 0.01; // 0.03
	detectorParams.maxMarkerPerimeterRate		 = 4.0; // 4.0
	detectorParams.minCornerDistanceRate		 = 0.05; // 0.05
	detectorParams.minDistanceToBorder			 = 3; // 3
	detectorParams.cornerRefinementMethod		 = cv::aruco::CORNER_REFINE_SUBPIX;
	detectorParams.cornerRefinementMaxIterations = 30; // 30
	detectorParams.cornerRefinementMinAccuracy	 = 0.1; // 0.1

	// Re-initialize ArUco detector to apply parameter edits
	try
	{
		arucoDetector = cv::aruco::ArucoDetector(dictionary, detectorParams);
		return true;
	}
	catch(cv::Exception& e)
	{
		std::cout << "Exception in CameraInterface::Begin(): " << e.msg << "\n";
		return false;
	}

	return false;
}

void ArucoInterface::Detect(cv::Mat& mat)
{
	// Run tag detector
	arucoDetector.detectMarkers(mat, markerCorners, markerIDs, markerRejects);

	// Check if markers were found
	if(!markerIDs.empty())
	{
		// Reset flag
		MarkersFound = false;

		// Process each detected marker
		for(size_t i = 0; i < markerIDs.size(); i++)
		{
			// Look only for markers with IDs between 1 and 5
			if(markerIDs[i] > 0 && markerIDs[i] < 6)
			{

				std::cout << "Corners: " << markerCorners[i].size() << "\n";
				// Ensure 4 corners per marker
				if(markerCorners[i].size() == 4)
				{
					MarkersFound = true;
					std::cout << "Tag: " << markerIDs[i] << " found.\n";

					for(int k = 0; k < 4; k++)
					{
						std::cout << markerCorners[k] << " ";
					}
					std::cout << "\n";

					// Calculate the center (2D pixel space) - optional
					unsigned short avgX = int((markerCorners[i][0].x + markerCorners[i][1].x + markerCorners[i][2].x + markerCorners[i][3].x) / 4.0f);
					unsigned short avgY = int((markerCorners[i][0].y + markerCorners[i][1].y + markerCorners[i][2].y + markerCorners[i][3].y) / 4.0f);

					// Resize vectors for pose estimation
					rotationVector.resize(markerIDs.size());
					translationVector.resize(markerIDs.size());

					// Use perspective-n-point solver to get camera pose for this marker
					try
					{
						for(size_t j = 0; j < markerIDs.size(); j++)
						{
							cv::solvePnP(objPoints, markerCorners.at(j), CONFIG_CAMERA_MATRIX, CONFIG_DISTORTION_COEFFS, rotationVector.at(j), translationVector.at(j));
							// cv::aruco::drawDetectedMarkers(mat, markerCorners, markerIDs);
						}
					}
					catch(cv::Exception& e)
					{
						std::cout << "Exception in solvePnP: " << e.msg << "\n";
					}
				}
			}
		}
	}
	else
	{
		// No markers found
	}
}
