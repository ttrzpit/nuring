#include "T_SharedDataManagerClass.h"

SharedDataManager::SharedDataManager() {
    data = std::make_shared<ManagedData>();
}

std::shared_ptr<ManagedData> SharedDataManager::getData() {
    return data;
}