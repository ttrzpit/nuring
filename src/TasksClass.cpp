#include "TasksClass.h"

// System data manager
#include "SystemDataManager.h"

TasksClass::TasksClass( SystemDataManager& ctx) : dataHandle ( ctx) , shared(ctx.getData() ) { 

    // Stuff
    
}