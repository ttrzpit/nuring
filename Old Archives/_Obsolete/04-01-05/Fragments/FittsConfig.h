/*  configHighResolution.h = Configuration header for high-resolution feed
 */

#pragma once

#include "config.h"
#include <opencv2/core.hpp>

// Display properties
inline constexpr unsigned short FITTS_SCREEN_EXCLUSION_ZONE = 93;
inline constexpr unsigned short FITTS_MARKER_SIZE			= CONFIG_MARKER_WIDTH;
inline constexpr unsigned short FITTS_SCREEN_WIDTH			= 1920;
inline constexpr unsigned short FITTS_SCREEN_HEIGHT			= 1080;