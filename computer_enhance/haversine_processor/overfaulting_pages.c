#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

u8 *
AllocateMemory(u64 TotalSize)
{
    u8 *Result;

#if KENGINE_WIN32
    Result = (u8 *)VirtualAlloc(0, TotalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
#elif KENGINE_LINUX
    Result = (u8 *)mmap(0, TotalSize, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#else
#error Unsupported platform
#endif

    return Result;
}

typedef struct app_state
{
    memory_arena Arena;
} app_state;

s32
MainLoop(app_memory *AppMemory)
{
    // app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    u64 PageSize = 4096;
    u64 PageCount = 16384;
    u64 TotalSize = PageCount*PageSize;

    u8 *Data = AllocateMemory(TotalSize);
    if(Data)
    {
        u64 StartFaultCount = PlatformReadOSPageFaultCount();

        u64 PriorOverFaultCount = 0;
        u64 PriorPageIndex = 0;
        for(u64 PageIndex = 0; PageIndex < PageCount; ++PageIndex)
        {
            Data[TotalSize - 1 - PageSize*PageIndex] = (u8)PageIndex;
            u64 EndFaultCount = PlatformReadOSPageFaultCount();

            u64 OverFaultCount = (EndFaultCount - StartFaultCount) - PageIndex;
            if(OverFaultCount > PriorOverFaultCount)
            {
                PlatformConsoleOut("Page %lu: %lu extra faults (%lu pages since last increase)\n",
                                   PageIndex, OverFaultCount, (PageIndex - PriorPageIndex));
                
                PriorOverFaultCount = OverFaultCount;
                PriorPageIndex = PageIndex;
            }
        }
    }
    else
    {
        PlatformConsoleOut("ERROR: Unable to allocate memory\n");
    }

    return 0;
}
