#include "TestClass.h"

// Constructor
TestClass::TestClass( SystemDataManager& ctx )
	: dataHandle( ctx ) { }



void TestClass::GetName() {
	// Pull access to data
	auto sharedData = dataHandle.getData();

	std::shared_lock<std::shared_mutex> lock( sharedData->sharedMutex );
	std::cout << "String: " << sharedData->myString << "\n";
}


void TestClass::SetName( const std::string& newName ) {
	auto								sharedData = dataHandle.getData();
	std::unique_lock<std::shared_mutex> lock( sharedData->sharedMutex );
	sharedData->myString = newName;
}