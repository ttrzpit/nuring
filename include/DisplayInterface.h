#pragma once

// Necessary libraries
#include <iostream>
#include <math.h>

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

// Constants
#define RAD2DEG 57.2958

class DisplayInterface {

public:
	// Constructor
	DisplayInterface() {
		std::cout << "DisplayInterface: Display interface initialized.\n";
		if( CONFIG_TYPE == "LowResolution" ) {
			fontHeader = 0.6f;
			fontBody   = 0.5f;
		} else if( CONFIG_TYPE == "HighResolution" ) {
			fontHeader = 0.5f;
			fontBody   = 0.4f;
		}
	}

	// Functions
	cv::Mat		Update( cv::Mat& frame, CaptureInterface& Capture );
	void		setFrequency( char freq );
	void		setStatusString( std::string status );
	void		setSerialString( std::string packet );
	void		SetMouseXY( cv::Point3i mouseData );
	cv::Point3i GetMouseXY();

	// Variables
	bool FLAG_SERIAL	 = false;
	bool FLAG_AMPLIFIERS = false;



private:
	// Variables
	cv::cuda::GpuMat		 gpuFrame		   = cv::cuda::GpuMat( CONFIG_CAM_HEIGHT, CONFIG_CAM_WIDTH, CV_8UC3 );
	std::vector<std::string> stringVector	   = { "", "", "", "", "" };
	cv::Mat					 matDisplay		   = cv::Mat( CONFIG_DIS_HEIGHT, CONFIG_DIS_WIDTH, CV_8UC3 );
	char					 measuredFrequency = 0.0f;
	std::string				 statusString	   = "";
	std::string				 serialString	   = "";

	// Text properties
	float	   cellFontSize	 = 1.0f;
	cv::Scalar cellOutline	 = CONFIG_colWhite;
	cv::Scalar cellFill		 = CONFIG_colGraDk;
	cv::Scalar cellTextColor = CONFIG_colYelLt;
	short	   cellPx		 = 40;
	short	   WIDTH		 = CONFIG_DIS_CELL_WIDTH;
	short	   HEIGHT		 = CONFIG_DIS_CELL_HEIGHT;
	cv::Size   textSize;
	float	   fontHeader = 0.6f;
	float	   fontBody	  = 0.5f;


	// Cell properties
	short c0	= 0;
	short r0	= 0;
	short nR	= 0;
	short nC	= 0;
	short cW	= 0;
	short rH	= 0;
	short textX = 0;	// Text X position
	short textY = 0;	// Text Y position

	// Mouse position
	cv::Point3i mouseXY = cv::Point3i( 0, 0, 0 );

	// Functions
	void DrawCell( std::string str, std::string cell0, short width, short height, float sz, cv::Scalar textColor, cv::Scalar fillColor, bool centered );
	void FormatCell( cv::Scalar outline, cv::Scalar fill, cv::Scalar textColor, bool center );
	void AddText( CaptureInterface& Capture );
	void DrawActiveMarkerBorder( bool markerFound, bool activeMarker, std::vector<cv::Point2i> corners );
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
		cv::circle( matDisplay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_DET_RADIUS, CONFIG_colGreMd, 2 );
		// cv::rectangle( matDisplay, cv::Point2i( 1, 1 ), cv::Point2i( frame.cols - 2, frame.rows - 3 ), CONFIG_colGreMd, 3 );
		cv::line( matDisplay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, frame.rows ), CONFIG_colGreMd, 1 );
		cv::line( matDisplay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( frame.cols, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_colGreMd, 1 );

		// Draw vector to center of marker
		if( Capture.Markers[Capture.activeMarker].errorMagnitude2D > CONFIG_DET_RADIUS ) {
			cv::line( matDisplay,
					  cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ),
					  cv::Point2i( CONFIG_CAM_PRINCIPAL_X + Capture.Markers[Capture.activeMarker].errorMagnitude2D * cos( Capture.Markers[Capture.activeMarker].errorHeading ),
								   CONFIG_CAM_PRINCIPAL_Y - Capture.Markers[Capture.activeMarker].errorMagnitude2D * sin( Capture.Markers[Capture.activeMarker].errorHeading ) ),
					  CONFIG_colCyaDk,
					  1 );
		}
		cv::line( matDisplay,
				  cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ),
				  cv::Point2i( CONFIG_CAM_PRINCIPAL_X + Capture.Markers[Capture.activeMarker].errorMagnitudeNorm * ( CONFIG_DET_RADIUS / 100 ) * cos( Capture.Markers[Capture.activeMarker].errorHeading ),
							   CONFIG_CAM_PRINCIPAL_Y - Capture.Markers[Capture.activeMarker].errorMagnitudeNorm * ( CONFIG_DET_RADIUS / 100 ) * sin( Capture.Markers[Capture.activeMarker].errorHeading ) ),
				  CONFIG_colCyaMd,
				  2 );

		// Draw border on active marker
		DrawActiveMarkerBorder( Capture.markerFound, Capture.Markers[Capture.activeMarker].present, Capture.activeCorners );
	} else {
		// cv::rectangle( matDisplay, cv::Point2i( 1, 1 ), cv::Point2i( frame.cols - 2, frame.rows - 3 ), CONFIG_colRedDk, 3 );
		cv::circle( matDisplay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_DET_RADIUS, CONFIG_colRedDk, 1 );
		cv::line( matDisplay, cv::Point2i( CONFIG_CAM_PRINCIPAL_X, 0 ), cv::Point2i( CONFIG_CAM_PRINCIPAL_X, frame.rows ), CONFIG_colRedDk, 1 );
		cv::line( matDisplay, cv::Point2i( 0, CONFIG_CAM_PRINCIPAL_Y ), cv::Point2i( frame.cols, CONFIG_CAM_PRINCIPAL_Y ), CONFIG_colRedDk, 1 );
	}



	// Add gui text
	AddText( Capture );

	return matDisplay;

	// Iterate over markers and only update present ones
	// for( size_t i = 0; i < Capture.Markers.size(); i++ ) {
	// 	if( Capture.Markers[i].present ) {
	// 		// stringVector[i] = Capture.Markers[i].error3D.x
	// 	}
	// }
}


void DisplayInterface::AddText( CaptureInterface& Capture ) {

	// Set cell format
	FormatCell( CONFIG_colGraLt, CONFIG_colGraBk, CONFIG_colWhite, true );

	// Active Marker Block
	DrawCell( "Active Marker", "A1", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "1", "A2", 1, 1, fontBody, ( Capture.Markers[1].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( Capture.activeMarker == 1 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "2", "B2", 1, 1, fontBody, ( Capture.Markers[2].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( Capture.activeMarker == 2 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "3", "C2", 1, 1, fontBody, ( Capture.Markers[3].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( Capture.activeMarker == 3 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "4", "D2", 1, 1, fontBody, ( Capture.Markers[4].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( Capture.activeMarker == 4 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "5", "E2", 1, 1, fontBody, ( Capture.Markers[5].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( Capture.activeMarker == 5 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "6", "A3", 1, 1, fontBody, ( Capture.Markers[6].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( Capture.activeMarker == 6 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "7", "B3", 1, 1, fontBody, ( Capture.Markers[7].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( Capture.activeMarker == 7 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "8", "C3", 1, 1, fontBody, ( Capture.Markers[8].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( Capture.activeMarker == 8 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "9", "D3", 1, 1, fontBody, ( Capture.Markers[9].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( Capture.activeMarker == 9 ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( "10", "E3", 1, 1, fontBody, ( Capture.Markers[10].present ? CONFIG_colWhite : CONFIG_colGraDk ), ( Capture.activeMarker == 10 ? CONFIG_colGreDk : CONFIG_colBlack ), true );

	// Position [mm] Block
	// DrawCell( "", "A4", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Position [mm]", "A4", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "x", "A5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "y", "A6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "z", "A7", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "R", "A8", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( Capture.Markers[Capture.activeMarker].error3D.x ), "B5", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( Capture.Markers[Capture.activeMarker].error3D.y ), "B6", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( Capture.Markers[Capture.activeMarker].error3D.z ), "B7", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( Capture.Markers[Capture.activeMarker].errorMagnitude3D ), "B8", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Position [px] Block
	DrawCell( "Position [px]", "F4", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "x", "F5", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "y", "F6", 1, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "P", "F7", 1, 2, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "|R|", "G7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( "Theta", "I7", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( Capture.Markers[Capture.activeMarker].error2D.x ), "G5", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( Capture.Markers[Capture.activeMarker].error2D.y ), "G6", 4, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	// DrawCell( std::to_string( Capture.Markers[Capture.activeMarker].errorMagnitude2D ), "G8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( Capture.Markers[Capture.activeMarker].errorMagnitudeNorm ), "G8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( int( Capture.Markers[Capture.activeMarker].errorHeading * RAD2DEG ) ), "I8", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );


	// Mouse output block
	DrawCell( "Mouse", "AF5", 4, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( mouseXY.x ), "AF6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( std::to_string( mouseXY.y ), "AH6", 2, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );
	DrawCell( ( mouseXY.z == 1 ? "Button1" : "" ), "AF7", 4, 1, fontBody, CONFIG_colWhite, ( FLAG_AMPLIFIERS ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	// std::cout << mouseXY.z << "\n";

	// Frequency Block
	DrawCell( "Frequency", "AJ1", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( std::to_string( measuredFrequency ), "AJ2", 5, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Motor Output Block
	DrawCell( "Amplifiers", "AJ3", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( FLAG_AMPLIFIERS ? "Enabled" : "Off" ), "AJ4", 5, 1, fontBody, CONFIG_colWhite, ( FLAG_AMPLIFIERS ? CONFIG_colGreDk : CONFIG_colBlack ), true );

	// Serial Block
	DrawCell( "Serial Status", "AJ5", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( ( FLAG_SERIAL ? "Online" : "Offline" ), "AJ6", 5, 1, fontBody, CONFIG_colWhite, ( FLAG_SERIAL ? CONFIG_colGreDk : CONFIG_colBlack ), true );
	DrawCell( ( FLAG_SERIAL ? serialString : "---" ), "AJ7", 5, 2, fontBody, CONFIG_colWhite, CONFIG_colBlack, true );

	// Status Block
	// DrawCell( "Messages:", "A8", 5, 1, fontHeader, CONFIG_colWhite, CONFIG_colGraBk, true );
	DrawCell( statusString, "K8", 25, 1, fontBody, CONFIG_colWhite, CONFIG_colBlack, false );
	// measuredFrequency/
}



void DisplayInterface::DrawCell( std::string str, std::string cell0, short width, short height, float sz, cv::Scalar textColor, cv::Scalar fillColor, bool centered ) {

	// Check if using double letters (e.g., AA, AB)
	if( std::isalpha( cell0[0] ) && !std::isalpha( cell0[1] ) ) {	 // Only one letter
		c0 = ( cell0[0] - 'A' ) * WIDTH;
		r0 = ( CONFIG_CAM_HEIGHT + 1 ) + ( ( std::stoi( cell0.substr( 1 ) ) - 1 ) * HEIGHT - 1 );
		rH = height * HEIGHT;
		cW = width * WIDTH;
	} else if( std::isalpha( cell0[0] ) && std::isalpha( cell0[1] ) ) {	   // Two letters)
		c0 = ( 26 + ( cell0[1] - 'A' ) ) * WIDTH;
		r0 = ( CONFIG_CAM_HEIGHT + 1 ) + ( ( std::stoi( cell0.substr( 2 ) ) - 1 ) * HEIGHT - 1 );
		rH = height * HEIGHT;
		cW = width * WIDTH;
	}

	// Draw cell frame
	cv::rectangle( matDisplay, cv::Rect( c0, r0, cW, rH ), fillColor, -1 );
	cv::rectangle( matDisplay, cv::Rect( c0, r0 - 1, cW + 1, rH + 1 ), cellOutline, 1 );

	// Calculate text dimensions
	if( sz >= fontHeader ) {
		textSize = cv::getTextSize( str, cv::FONT_HERSHEY_DUPLEX, sz, 1, 0 );
	} else {
		textSize = cv::getTextSize( str, cv::FONT_HERSHEY_SIMPLEX, sz, 1, 0 );
	}

	// Calculate position for center
	if( centered ) {
		textX = c0 + ( cW - textSize.width ) / 2;
		textY = r0 + ( rH + textSize.height ) / 2 - 1;	  //+ ( cH - textSize.height ) / 2;
	} else {
		textX = c0 + 10;
		textY = r0 + ( rH + textSize.height ) / 2 - 1;	  //+ ( cH - textSize.height ) / 2;
	}

	// Place text
	if( sz >= fontHeader ) {
		cv::putText( matDisplay, str, cv::Point( textX, textY ), cv::FONT_HERSHEY_DUPLEX, sz, textColor, 1 );
	} else {
		cv::putText( matDisplay, str, cv::Point( textX, textY ), cv::FONT_HERSHEY_SIMPLEX, sz, textColor, 1 );
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
}


void DisplayInterface::setFrequency( char freq ) {
	measuredFrequency = freq;
}

void DisplayInterface::setStatusString( std::string status ) {

	statusString = status;
}

void DisplayInterface::setSerialString( std::string packet ) {

	serialString = packet;
}


void DisplayInterface::DrawActiveMarkerBorder( bool markerFound, bool active, std::vector<cv::Point2i> corners ) {
	if( markerFound && active ) {
		cv::polylines( matDisplay, corners, true, CONFIG_colCyaMd, 2 );
	} else {
		cv::polylines( matDisplay, corners, true, CONFIG_colCyaDk, 2 );
	}
}


void DisplayInterface::SetMouseXY( cv::Point3i mouseData ) {
	mouseXY.x = mouseData.x;
	mouseXY.y = mouseData.y;
	mouseXY.z = mouseData.z;
}

cv::Point3i DisplayInterface::GetMouseXY() {
	return cv::Point3i( mouseXY.x, mouseXY.y, mouseXY.z );
}
