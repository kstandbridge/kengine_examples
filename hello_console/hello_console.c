#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

typedef struct app_state
{
    memory_arena Arena;

    s32 Value;
} app_state;

s32
MainLoop(app_memory *AppMemory)
{
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    AppState->Value = 42;

    PlatformConsoleOut("Before %d after\n", AppState->Value);
    
    return 0;
}