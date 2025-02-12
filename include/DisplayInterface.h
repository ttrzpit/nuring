#pragma once

// Necessary libraries
#include <iostream>

// References to colors and frame properties
#include "CaptureInterface.h"
#include "config.h"

// OpenCV libraries
#include <opencv2/core.hpp>	   // Core OpenCV functions
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/highgui.hpp>	  // For pollKey()
#include <opencv2/imgproc.hpp>	  // For remapping
#include <opencv2/opencv.hpp>

class DisplayInterface {

public:
	// Constructor
	DisplayInterface() {
		std::cout << "DisplayInterface: Display interface initialized.\n";
	}

	// Functions
	cv::Mat Update( cv::Mat& frame, CaptureInterface& Capture );
	// void CheckFrequency();

private:
	// Variables
	cv::cuda::GpuMat		 gpuFrame	  = cv::cuda::GpuMat( CONFIG_CAP_HEIGHT, CONFIG_CAP_WIDTH, CV_8UC3 );
	std::vector<std::string> stringVector = { "", "", "", "", "" };
	cv::Mat					 matDisplay	  = cv::Mat( CONFIG_DIS_HEIGHT, CONFIG_DIS_WIDTH, CV_8UC3 );

	// Text properties
	float	   cellFontSize	 = 1.0f;
	cv::Scalar cellOutline	 = CONFIG_colWhite;
	cv::Scalar cellFill		 = CONFIG_colGraDk;
	cv::Scalar cellTextColor = CONFIG_colYelLt;
	bool	   cellCentered	 = false;
	short	   cellPx		 = 40;
	short	   WIDTH		 = 64;
	short	   HEIGHT		 = 30;
	cv::Size   textSize;

	// Cell properties
	short c0	= 0;
	short r0	= 0;
	short nR	= 0;
	short nC	= 0;
	short cW	= 0;
	short rH	= 0;
	short textX = 0;	// Text X position
	short textY = 0;	// Text Y position

	// Functions
	void DrawCell( std::string str, std::string cell0, std::string cell1, float sz );
	void FormatCell( cv::Scalar outline, cv::Scalar fill, cv::Scalar textColor, bool center );
};


/** 
 * @brief Update the display
 */
cv::Mat DisplayInterface::Update( cv::Mat& frame, CaptureInterface& Capture ) {

	// Clear display frame
	matDisplay = 0;

	// Copy camera frame to display frame
	frame.copyTo( matDisplay( cv::Rect( 0, 0, frame.cols, frame.rows ) ) );


	// Draw green outline and crosshairs if marker found
	if( Capture.markerFound ) {
		cv::rectangle( matDisplay, cv::Point2i( 1, 1 ), cv::Point2i( frame.cols - 2, frame.rows - 3 ), CONFIG_colGreMd, 3 );
		cv::line( matDisplay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, frame.rows ), CONFIG_colGreMd, 1 );
		cv::line( matDisplay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( frame.cols, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_colGreMd, 1 );
	} else {
		// cv::rectangle( matDisplay, cv::Point2i( 1, 1 ), cv::Point2i( frame.cols - 2, frame.rows - 3 ), CONFIG_colRedDk, 3 );
		cv::line( matDisplay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, frame.rows ), CONFIG_colRedDk, 1 );
		cv::line( matDisplay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( frame.cols, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_colRedDk, 1 );
	}



	// Set cell format
	FormatCell( CONFIG_colGraLt, CONFIG_colGraBk, CONFIG_colWhite, true );

	// Draw interface
	DrawCell( "Active Marker", "A1", "C1", 0.7f );
	DrawCell( "1", "A2", "C2", 0.7f );
	DrawCell( " ", "A3", "B3", 0.7f );
	DrawCell( "Position", "B3", "C3", 0.7f );
	DrawCell( "x", "A4", "A4", 0.6f );
	DrawCell( "y", "A5", "A5", 0.6f );
	DrawCell( "z", "A6", "A6", 0.6f );
	DrawCell( std::to_string( Capture.Markers[0].error3D.x ), "B4", "C4", 0.6f );
	DrawCell( std::to_string( Capture.Markers[0].error3D.y ), "B5", "C5", 0.6f );
	DrawCell( std::to_string( Capture.Markers[0].error3D.z ), "B6", "C6", 0.6f );

	return matDisplay;

	// Iterate over markers and only update present ones
	// for( size_t i = 0; i < Capture.Markers.size(); i++ ) {
	// 	if( Capture.Markers[i].present ) {
	// 		// stringVector[i] = Capture.Markers[i].error3D.x
	// 	}
	// }
}

void DisplayInterface::DrawCell( std::string str, std::string cell0, std::string cell1, float sz ) {


	// Extract cell address
	c0 = ( cell0[0] - 'A' ) * WIDTH;
	r0 = ( CONFIG_CAP_HEIGHT + 1 ) + ( ( std::stoi( cell0.substr( 1 ) ) - 1 ) * HEIGHT - 1 );

	// Calculate cell dimensions
	rH = ( ( std::stoi( cell1.substr( 1 ) ) ) - ( std::stoi( cell0.substr( 1 ) ) ) + 1 ) * HEIGHT;
	cW = ( ( cell1[0] - 'A' + 1 ) - ( cell0[0] - 'A' ) ) * WIDTH;

	// Draw cell frame
	cv::rectangle( matDisplay, cv::Rect( c0, r0, cW, rH ), cellFill, -1 );
	cv::rectangle( matDisplay, cv::Rect( c0, r0, cW + 1, rH + 1 ), cellOutline, 1 );

	// Calculate text dimensions
	if( sz >= 0.8f ) {
		textSize = cv::getTextSize( str, cv::FONT_HERSHEY_DUPLEX, sz, 1, 0 );
	} else {
		textSize = cv::getTextSize( str, cv::FONT_HERSHEY_SIMPLEX, sz, 1, 0 );
	}

	// Calculate position for center
	if( cellCentered ) {
		textX = c0 + ( cW - textSize.width ) / 2;
		textY = r0 + ( rH + textSize.height ) / 2;	  //+ ( cH - textSize.height ) / 2;
	} else {
		// textX = c0 + 10;
		// textY = r0 + r1 - ( textSize.height / 2 ) + 1;	  //+ ( cH - textSize.height ) / 2;
	}

	// Place text
	if( sz >= 0.8f ) {
		cv::putText( matDisplay, str, cv::Point( textX, textY ), cv::FONT_HERSHEY_DUPLEX, sz, cellTextColor, 1 );
	} else {
		cv::putText( matDisplay, str, cv::Point( textX, textY ), cv::FONT_HERSHEY_SIMPLEX, sz, cellTextColor, 1 );
	}
}


/**
 * @brief Set cell format, called before DrawCell
 * @param   size    Font size [float]
 * @param   outline Outline color [cv::Scalar]
 * @param   fill Fill color [cv::Scalar]
 * @param   textColor Font color [cv::Scalar]
 * @param   center Flag to center text [bool]
 */
void DisplayInterface::FormatCell( cv::Scalar outline, cv::Scalar fill, cv::Scalar textColor, bool center ) {

	cellOutline	  = outline;
	cellFill	  = fill;
	cellTextColor = textColor;
	cellCentered  = center;
}