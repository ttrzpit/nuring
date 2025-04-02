// Call to class header
#include "LoggingClass.h"

// System data manager
#include "SystemDataManager.h"


/**
 * @brief Constructor
 */
LoggingClass::LoggingClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {
	logFile.reserve( 6000 );
}


/**
 * Items to log
 * timestamp, tagID, errXmm, errYmm, errZmm, errXpx, errYpx, mag, theta
 */
void LoggingClass::AddEntry() {

	newEntry.timestamp = shared->timingTimestamp;
	newEntry.ID		   = shared->arucoActiveID;
	newEntry.errXmm	   = shared->arucoTags[shared->arucoActiveID].error3D.x;
	newEntry.errYmm	   = shared->arucoTags[shared->arucoActiveID].error3D.y;
	newEntry.errZmm	   = shared->arucoTags[shared->arucoActiveID].error3D.z;
	newEntry.errXpx	   = shared->arucoTags[shared->arucoActiveID].error2D.x;
	newEntry.errYpx	   = shared->arucoTags[shared->arucoActiveID].error2D.y;
	newEntry.magnitude = shared->arucoTags[shared->arucoActiveID].errorMagnitude2D;
	newEntry.heading   = shared->arucoTags[shared->arucoActiveID].errorTheta;
	newEntry.custom1   = shared->loggingCustom1;
	newEntry.custom2   = shared->loggingCustom2;
	newEntry.custom3   = shared->loggingCustom3;
	newEntry.custom4   = shared->loggingCustom4;
	newEntry.custom5   = shared->loggingCustom5;

	logFile.push_back( newEntry );

	newEntry = {};
}



void LoggingClass::Save() {

	std::string logFilename;
}