/*  config.h = Configuration header
 */

#pragma once

#include <opencv2/core.hpp>

// Camera constants
inline constexpr unsigned short CONFIG_CAM_FRAMERATE		 = 90;
inline constexpr unsigned short CONFIG_CAM_WIDTH			 = 1280;
inline constexpr unsigned short CONFIG_CAM_HEIGHT			 = 720;
inline constexpr signed char	CONFIG_CAM_EXPOSURE_LEVEL	 = 156; // [PX] Camera exposure level
inline constexpr unsigned char	CONFIG_CAM_GAIN_LEVEL		 = 0; // [PX] Camera height
inline constexpr bool			CONFIG_CAM_AUTO_EXPOSURE	 = false; // [PX] Camera height
inline constexpr bool			CONFIG_CAM_AUTO_WHITEBALANCE = false; // [PX] Camera height
inline constexpr bool			CONFIG_CAM_AUTO_FOCUS		 = false; // [PX] Camera height

// Marker constants
inline constexpr short CONFIG_MARKER_WIDTH = 20; // [mm] Marker dimension
// inline constexpr float             CONFIG_PX_TO_MM  =   ( MARKER_WIDTH
// / 72.0f )   ;    // [mm] Conversion factor; inline constexpr float
// CONFIG_MM_TO_PX  =   ( 72.0f / MARKER_WIDTH )   ;    // [px] Conversion
// factor;

const cv::Mat CONFIG_CAMERA_MATRIX = ( cv::Mat_<double>( 3, 3 ) << 478.775f, 0.0f, 631.579f, 0.0f, 478.625f, 424.489f, 0.0f, 0.0f, 1.0f );

const cv::Mat CONFIG_DISTORTION_COEFFS = ( cv::Mat_<double>( 1, 5 ) << 0.009663f, -0.015094f, -0.001703f, -0.002634f, -0.002227f );
