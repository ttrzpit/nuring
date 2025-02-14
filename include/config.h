/*  config.h = Configuration header
 */

#pragma once

#include <opencv2/core.hpp>

// Camera properties                                                     // min|default|max
inline constexpr short			CONFIG_CAM_BRIGHTNESS		 = 0;		 // -64|0|64
inline constexpr unsigned short CONFIG_CAM_CONTRAST			 = 0;		 // 0|0|95
inline constexpr unsigned short CONFIG_CAM_SATURATION		 = 56;		 // 0|56|255
inline constexpr short			CONFIG_CAM_HUE				 = 0;		 // -2000|0|2000
inline constexpr bool			CONFIG_CAM_AUTO_WHITEBALANCE = false;	 // Auto whitebalance
inline constexpr unsigned short CONFIG_CAM_GAMMA			 = 110;		 // 64|110|300
inline constexpr unsigned short CONFIG_CAM_GAIN				 = 10;		 // 0|0|1023
inline constexpr unsigned short CONFIG_CAM_POWERLINE_FREQ	 = 2;		 // 0|1|2 (off|50Hz|60Hz)
inline constexpr unsigned short CONFIG_CAM_WHITEBALANCE_TEMP = 4600;	 // 2800|4600|6500
inline constexpr unsigned short CONFIG_CAM_SHARPNESS		 = 0;		 // 0|0|7
inline constexpr unsigned short CONFIG_CAM_BACKLIGHT		 = 54;		 // 36|54|160
inline constexpr unsigned short CONFIG_CAM_AUTO_EXPOSURE	 = 1;		 // 0|3|3 (1=manual, 3=aperature priority)
inline constexpr unsigned short CONFIG_CAM_EXPOSURE_LEVEL	 = 156;		 // 1|156|10000
inline constexpr int			CONFIG_CAM_PAN				 = 0;		 // -648000|0|648000 (step 3600)
inline constexpr int			CONFIG_CAM_TILT				 = 0;		 // -648000|0|648000 (step 3600)
inline constexpr unsigned short CONFIG_CAM_FOCUS_LEVEL		 = 0;		 //  0|0|1023
inline constexpr bool			CONFIG_CAM_AUTO_FOCUS		 = 0;		 //  0|1|1
inline constexpr unsigned short CONFIG_CAM_ZOOM				 = 0;		 //  0|0|60
inline constexpr unsigned short CONFIG_CAM_PRINCIPAL_X		 = 632;		 //  0|0|60
inline constexpr unsigned short CONFIG_CAM_PRINCIPAL_Y		 = 424;		 //  0|0|60

// Capture properties
inline constexpr unsigned short CONFIG_CAP_FRAMERATE = 90;
inline constexpr unsigned short CONFIG_CAP_WIDTH	 = 1280;	// 1280
inline constexpr unsigned short CONFIG_CAP_HEIGHT	 = 960;		// 960

// Display properties
inline constexpr unsigned short CONFIG_DIS_WIDTH  = 1280;
inline constexpr unsigned short CONFIG_DIS_HEIGHT = 1200;

// Available Resolutions @ 90Hz
// Valid Calib                                      x
// Resolution   1920x1080   1920x1200   1600x1200   1280x960    1280x720    1024x768    960x720     800x600
// MPixels      2.07        2.30        1.92        1.23        0.92        0.79        0.69        0.48
// Aspect Ratio 1.778       1.600       1.33        1.33        1.778       1.333       1.333       1.333



// Marker constants
inline constexpr short CONFIG_MARKER_WIDTH = 20;	// [mm] Marker dimension
// inline constexpr float             CONFIG_PX_TO_MM  =   ( MARKER_WIDTH
// / 72.0f )   ;    // [mm] Conversion factor; inline constexpr float
// CONFIG_MM_TO_PX  =   ( 72.0f / MARKER_WIDTH )   ;    // [px] Conversion
// factor;

// Camera constants
const cv::Mat CONFIG_CAMERA_MATRIX	   = ( cv::Mat_<double>( 3, 3 ) << 478.775f, 0.0f, 631.579f, 0.0f, 478.625f, 424.489f, 0.0f, 0.0f, 1.0f );
const cv::Mat CONFIG_DISTORTION_COEFFS = ( cv::Mat_<double>( 1, 5 ) << 0.009663f, -0.015094f, -0.001703f, -0.002634f, -0.002227f );

// Colors                                B G R
cv::Scalar CONFIG_colRedMd = cv::Scalar( 0, 0, 255 );
cv::Scalar CONFIG_colRedLt = cv::Scalar( 127, 127, 255 );
cv::Scalar CONFIG_colRedDk = cv::Scalar( 0, 0, 127 );
cv::Scalar CONFIG_colOraMd = cv::Scalar( 0, 127, 255 );
cv::Scalar CONFIG_colOraLt = cv::Scalar( 127, 191, 255 );
cv::Scalar CONFIG_colOraDk = cv::Scalar( 0, 64, 127 );
cv::Scalar CONFIG_colYelMd = cv::Scalar( 0, 255, 255 );
cv::Scalar CONFIG_colYelLt = cv::Scalar( 127, 255, 255 );
cv::Scalar CONFIG_colYelDk = cv::Scalar( 0, 127, 127 );
cv::Scalar CONFIG_colGreMd = cv::Scalar( 0, 255, 0 );
cv::Scalar CONFIG_colGreLt = cv::Scalar( 127, 255, 127 );
cv::Scalar CONFIG_colGreDk = cv::Scalar( 0, 127, 0 );
cv::Scalar CONFIG_colGreBk = cv::Scalar( 0, 64, 0 );
cv::Scalar CONFIG_colBluMd = cv::Scalar( 255, 0, 0 );
cv::Scalar CONFIG_colBluLt = cv::Scalar( 255, 127, 127 );
cv::Scalar CONFIG_colBluDk = cv::Scalar( 127, 0, 0 );
cv::Scalar CONFIG_colVioMd = cv::Scalar( 255, 0, 255 );
cv::Scalar CONFIG_colVioLt = cv::Scalar( 255, 127, 255 );
cv::Scalar CONFIG_colVioDk = cv::Scalar( 127, 0, 127 );
cv::Scalar CONFIG_colGraMd = cv::Scalar( 127, 127, 127 );
cv::Scalar CONFIG_colGraLt = cv::Scalar( 192, 192, 192 );
cv::Scalar CONFIG_colGraDk = cv::Scalar( 64, 64, 64 );
cv::Scalar CONFIG_colGraBk = cv::Scalar( 32, 32, 32 );
cv::Scalar CONFIG_colBlack = cv::Scalar( 0, 0, 0 );
cv::Scalar CONFIG_colWhite = cv::Scalar( 255, 255, 255 );
cv::Scalar CONFIG_colCyaMd = cv::Scalar( 255, 255, 0 );
cv::Scalar CONFIG_colCyaLt = cv::Scalar( 255, 255, 127 );
cv::Scalar CONFIG_colCyaDk = cv::Scalar( 127, 127, 0 );
cv::Scalar CONFIG_colMagMd = cv::Scalar( 255, 0, 255 );
cv::Scalar CONFIG_colMagLt = cv::Scalar( 255, 127, 255 );
cv::Scalar CONFIG_colMagDk = cv::Scalar( 127, 0, 127 );
