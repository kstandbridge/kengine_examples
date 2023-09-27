#define _GNU_SOURCE

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
    app_state *AppState = BootstrapPushStruct(app_state, Arena);
    memory_arena *Arena = &AppState->Arena;

    string_list *Args = PlatformGetCommandLineArgs(Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 1)
    {
        u64 PageSize = 4096;
        u64 PageCount = U64FromString(&Args->Entry);
        u64 TotalSize = PageSize*PageCount;

        PlatformConsoleOut("Page Count, Touch Count, Fault Count, Extra Faults\n");

        for(u64 TouchCount = 0; TouchCount < PageCount; ++TouchCount)
        {
            u64 TouchSize = PageSize*TouchCount;
            u8 *Data = (u8 *)VirtualAlloc(0, TotalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            if(Data)
            {
                u64 StartFaultCount = PlatformReadOSPageFaultCount();
                for(u64 Index = 0; Index < TouchSize; ++Index)
                {
                    Data[Index] = (u8)Index;
                }
                u64 EndFaultCount = PlatformReadOSPageFaultCount();

                u64 FaultCount = EndFaultCount - StartFaultCount;

                PlatformConsoleOut("%lu, %lu, %lu, %ld\n", PageCount, TouchCount, FaultCount, (FaultCount - TouchCount));

                VirtualFree(Data, 0, MEM_RELEASE);
            }
            else
            {
                PlatformConsoleOut("Error: Unable to allocate memory\n");
            }
        }

    }
    else
    {
        PlatformConsoleOut("Usage: fault_counter [# of 4k pages to allocate]\n");
    }

    return 0;
}