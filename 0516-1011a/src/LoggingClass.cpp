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
	newEntry.errXmm	   = shared->targetMarkerPosition3dNew.x;
	newEntry.errYmm	   = shared->targetMarkerPosition3dNew.y;
	newEntry.errZmm	   = shared->targetMarkerPosition3dNew.z;
	newEntry.custom1   = shared->loggingVariable1;
	newEntry.custom2   = shared->loggingVariable2;
	newEntry.custom3   = shared->loggingVariable3;
	newEntry.custom4   = shared->loggingVariable4;
	newEntry.custom5   = shared->loggingVariable5;

	logFile.push_back( newEntry );

	newEntry = {};
}



void LoggingClass::Save() {

	// Stop logging
	shared->FLAG_LOGGING_STARTED = false;

	// Open file
	std::ofstream file( "/home/tom/Code/nuring/logging/" + shared->loggingFilename + ".txt" );

	// Check if file opened safely
	if ( !file.is_open() ) {
		std::cerr << "Failed to open file\n";
		shared->displayString = "Logging Class: Failed to open file";
		return;
	}


	// Write header
	std::string headerString = "timestamp,errXmm,errYmm,errZmm," + shared->loggingHeader1 + "," + shared->loggingHeader2 + "," + shared->loggingHeader3 + "," + shared->loggingHeader4 + "," + shared->loggingHeader5 + "\n";
	file << headerString;

	// Write file contents
	for ( const auto& entry : logFile ) {
		file << entry.timestamp << "," << entry.errXmm << "," << entry.errYmm << "," << entry.errZmm << "," << entry.custom1 << "," << entry.custom2 << "," << entry.custom3 << "," << entry.custom4 << "," << entry.custom5 << "\n";
	}

	// Close file
	file.close();

	// Update flag
	// shared->FLAG_LOGGING_STARTED = false;
}


void LoggingClass::Initialize() {


	// Clear old data
	logFile.clear();

	// Update new file name
	shared->loggingFilename = "p" + PadValues( shared->TASK_USER_ID, 3 ) + "_t" + shared->TASK_NAME + "_r" + std::to_string( shared->TASK_REP_NUMBER ) + shared->loggingTimestamp;
	shared->displayString	= "LoggingClass: Filename = " + shared->loggingFilename;
}



/**
 * @brief Pad the given value with a specified number of zeros
 * @param val Value to be padded
 * @param nZeroes Number of padding zeroes
 */
std::string LoggingClass::PadValues( int val, int nZeroes ) {
	std::ostringstream sstream;
	sstream << std::setw( nZeroes ) << std::setfill( '0' ) << val;
	return sstream.str();
}