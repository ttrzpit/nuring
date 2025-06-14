/** Amplifier Class **/

#pragma once

// Memory for shared data
#include "T_Config.h"
#include <Arduino.h>
#include <memory>

// Forward declarations
class SharedDataManager;
struct ManagedData;



/** 
 * @brief Amplifier  class definition
 */
class T_VibrotactileClass {

public:
	// Default constructor
	T_VibrotactileClass( SharedDataManager& dataHandle );

	// Public functions
	void Begin();
	void Drive();
	

private:
	// Data manager handle
	std::shared_ptr<ManagedData> shared;
	SharedDataManager&			 dataHandle;

};
