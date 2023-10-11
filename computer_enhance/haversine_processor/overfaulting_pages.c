#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "virtual_address.h"

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
                
                if(PageIndex > 0)
                {
                    PrintAsLine(String("\tPrevious Pointer: "), DecomposePointer4K(Data + TotalSize - 1 - PageSize*(PageIndex - 1)));
                }
                PrintAsLine(    String("\t    This Pointer: "), DecomposePointer4K(Data + TotalSize - 1 - PageSize*PageIndex));
                
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
