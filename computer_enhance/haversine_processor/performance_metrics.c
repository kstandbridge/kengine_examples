#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

s32
MainLoop(app_memory *AppMemory)
{
    //app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);   

    u64 OSFrequency = PlatformGetOSTimerFrequency();
    PlatformConsoleOut("\t\tOS Frequency: %lu\n", OSFrequency);

    u64 CPUStart = PlatformReadCPUTimer();
    u64 OSStart = PlatformReadOSTimer();
    u64 OSEnd = 0;
    u64 OSElapsed = 0;
    while(OSElapsed < OSFrequency)
    {
        OSEnd = PlatformReadOSTimer();
        OSElapsed = OSEnd - OSStart;
    }

    u64 CPUEnd = PlatformReadCPUTimer();
    u64 CPUElapsed = CPUEnd - CPUStart;

    PlatformConsoleOut("\t\tOS Timer: %lu -> %lu = %lu elapsed\n", OSStart, OSEnd, OSElapsed);
    PlatformConsoleOut("\tOS Seconds: %.4f\n", (f64)OSElapsed/(f64)OSFrequency);

    PlatformConsoleOut("\t\tCPU Timer: %lu -> %lu = %lu elapsed\n", CPUStart, CPUEnd, CPUElapsed);

    return 0;
}