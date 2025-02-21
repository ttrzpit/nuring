// System data manager
#include "SystemDataManager.h"



SystemDataManager::SystemDataManager()
	: data( std::make_shared<ManagedData>() ) { }


/***
 * @brief Function to return pointer to data
 */
std::shared_ptr<ManagedData> SystemDataManager::getData() {
	return data;
}
