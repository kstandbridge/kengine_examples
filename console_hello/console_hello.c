#define KENGINE_WIN32
#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

s32
MainLoop(app_memory *AppMemory)
{
    UNREFERENCED_PARAMETER(AppMemory);
    
    s32 Result = 0;
    
    PlatformConsoleOut("Hello, world!");
    
    return Result;
}

