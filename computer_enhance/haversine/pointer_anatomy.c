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

    for(int PointerIndex = 0; PointerIndex < 16; ++PointerIndex)
    {
        void *Pointer = AllocateMemory(1024*1024);

        u64 Address = (u64)Pointer;
        PrintBinaryBits(Address, 48, 16);
        PlatformConsoleOut("|");
        PrintBinaryBits(Address, 39, 9);
        PlatformConsoleOut("|");
        PrintBinaryBits(Address, 30, 9);
        PlatformConsoleOut("|");
        PrintBinaryBits(Address, 21, 9);
        PlatformConsoleOut("|");
        PrintBinaryBits(Address, 12, 9);
        PlatformConsoleOut("|");
        PrintBinaryBits(Address, 0, 12);
        PlatformConsoleOut("\n");

        PrintAsLine(String(" 4k paging: "), DecomposePointer4K(Pointer));
        PrintAsLine(String("2mb paging: "), DecomposePointer2MB(Pointer));
        PrintAsLine(String("1gb paging: "), DecomposePointer1GB(Pointer));

        PlatformConsoleOut("\n");
    }

    return 0;
}