#pragma once

#include "SystemDataManager.h"


class TestClass {

public:
	// Constructor
	TestClass( SystemDataManager& dataHandle );
	void GetName();
	void SetName( const std::string& newName );

private:
	SystemDataManager& dataHandle;
};
