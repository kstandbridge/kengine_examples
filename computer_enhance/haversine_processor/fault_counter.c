#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

u64 
GetPageSize()
{
    u64 Result;

#if KENGINE_WIN32
    Result = 4096;
#elif KENGINE_LINUX
    Result = sysconf(_SC_PAGESIZE);
#else
#error Unsupported platform
#endif

    return Result;
}

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

void
FreeMemory(u8 *Memory, u64 TotalSize)
{
#if KENGINE_WIN32
    VirtualFree(Memory, 0, MEM_RELEASE);
#elif KENGINE_LINUX
#else
    munmap(Memory, TotalSize);
#error Unsupported platform
#endif
}

s32
MainLoop(app_memory *AppMemory)
{
    app_state *AppState = BootstrapPushStruct(app_state, Arena);
    memory_arena *Arena = &AppState->Arena;

    string_list *Args = PlatformGetCommandLineArgs(Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 1)
    {
        u64 PageSize = GetPageSize();
        u64 PageCount = U64FromString(&Args->Entry);
        u64 TotalSize = PageSize*PageCount;

        PlatformConsoleOut("# PageSize:%lu, PageCount:%lu, TotalSize:%lu\n", PageSize, PageCount, TotalSize);

        PlatformConsoleOut("Page Count, Touch Count, Fault Count, Extra Faults\n");

        for(u64 TouchCount = 0; TouchCount < PageCount; ++TouchCount)
        {
            u64 TouchSize = PageSize*TouchCount;
            u8 *Data = AllocateMemory(TotalSize);
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

                FreeMemory(Data, TotalSize);
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