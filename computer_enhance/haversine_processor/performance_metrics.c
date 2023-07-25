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
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);   

    u64 MillisecondsToWait = 1000;

    string_list *Args = PlatformGetCommandLineArgs(&AppState->Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 1)
    {
        MillisecondsToWait = U64FromString(&Args->Entry);
    }

    u64 OSFrequency = PlatformGetOSTimerFrequency();
    PlatformConsoleOut("\t\tOS Frequency: %lu (reported)\n", OSFrequency);

    u64 CPUStart = PlatformReadCPUTimer();
    u64 OSStart = PlatformReadOSTimer();
    u64 OSEnd = 0;
    u64 OSElapsed = 0;
    u64 OSWaitTime = OSFrequency * MillisecondsToWait / 1000;
    while(OSElapsed < OSWaitTime)
    {
        OSEnd = PlatformReadOSTimer();
        OSElapsed = OSEnd - OSStart;
    }

    u64 CPUEnd = PlatformReadCPUTimer();
    u64 CPUElapsed = CPUEnd - CPUStart;
    u64 CPUFrequency = 0;
    if(OSElapsed)
    {
        CPUFrequency = OSFrequency * CPUElapsed / OSElapsed;
    }

    PlatformConsoleOut("\t\tOS Timer: %lu -> %lu = %lu elapsed\n", OSStart, OSEnd, OSElapsed);
    PlatformConsoleOut("\tOS Seconds: %.4f\n", (f64)OSElapsed/(f64)OSFrequency);

    PlatformConsoleOut("\t\tCPU Timer: %lu -> %lu = %lu elapsed\n", CPUStart, CPUEnd, CPUElapsed);
    PlatformConsoleOut("\tCPU Frequency: %lu (guessed)\n", CPUFrequency);

    return 0;
}