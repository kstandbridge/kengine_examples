#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "virtual_address.h"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

typedef struct address_array
{
    void **Addresses;
    u64 Count;
    u64 PageSize;
} address_array;

typedef struct tracked_buffer
{
    string Base;
    address_array Results;
} tracked_buffer;

internal b32
IsValid(tracked_buffer Buffer)
{
    b32 Result = (Buffer.Base.Data != 0);
    return Result;
} 

internal void
DeallocateTrackedBuffer(tracked_buffer *Buffer)
{
    if(Buffer)
    {
        if(Buffer->Base.Data)
        {
            DeallocateMemory(Buffer->Base.Data, Buffer->Base.Size);
        }
        if(Buffer->Results.Addresses)
        {
            u64 TotalSize = Buffer->Results.PageSize*Buffer->Results.Count;
            DeallocateMemory(Buffer->Results.Addresses, TotalSize);
        }
        ZeroStruct(Buffer);
    }
}

internal tracked_buffer
AllocateTrackedBuffer(u64 MinimumSize)
{
    tracked_buffer Result = {0};

    // NOTE(kstandbridge): To make sure we have enough space to store all of the changed pages,
    // we have to ensure we allocate as many entries in our changed address table as there
    // are total pages in the requested buffer size
    SYSTEM_INFO Info;
    GetSystemInfo(&Info);
    u64 PageCount = ((MinimumSize + Info.dwPageSize - 1) / Info.dwPageSize);

    Result.Base.Size = MinimumSize;
    Result.Base.Data = (u8 *)VirtualAlloc(0, MinimumSize, MEM_RESERVE|MEM_COMMIT|MEM_WRITE_WATCH, PAGE_READWRITE);

    Result.Results.Count = PageCount;
    Result.Results.Addresses = (void **)VirtualAlloc(0, PageCount * sizeof(void **), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    if(Result.Base.Data && Result.Results.Addresses)
    {
        // NOTE(kstandbridge): Allocation successful
    }
    else
    {
        DeallocateTrackedBuffer(&Result);
    }

    return Result;
}

internal address_array
GetAndResetWrittenPages(tracked_buffer *Buffer)
{
    address_array Result = {0};

    DWORD PageSize = 0;
    ULONG_PTR AddressCount = Buffer->Results.Count;
    if(GetWriteWatch(WRITE_WATCH_FLAG_RESET, Buffer->Base.Data, Buffer->Base.Size,
                     Buffer->Results.Addresses, &AddressCount, &PageSize) == 0)
    {
        Result.Addresses = Buffer->Results.Addresses;
        Result.Count = AddressCount;
        Result.PageSize = PageSize;
    }

    return Result;
}

internal void
PrintAddressArray(address_array Written, u8 *BaseAddress)
{
    for(u64 PageIndex = 0; PageIndex < Written.Count; ++PageIndex)
    {
        PlatformConsoleOut("\t %lu: %lu\n", PageIndex, ((u8 *)Written.Addresses[PageIndex] - BaseAddress) / Written.PageSize);
    }
}

s32
MainLoop(app_memory *AppMemory)
{
    // app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);

    PlatformConsoleOut("Tracked buffer test:\n");

    tracked_buffer Tracked = AllocateTrackedBuffer(256*4096);
    if(IsValid(Tracked))
    {
        Tracked.Base.Data[15*4096] = 1;
        Tracked.Base.Data[25*4096] = 2;
        Tracked.Base.Data[35*4096] = 3;
        Tracked.Base.Data[45*4096] = 4;
        Tracked.Base.Data[55*4096] = 5;

        PlatformConsoleOut("\t--- Pass A ---\n");
        address_array Written = GetAndResetWrittenPages(&Tracked);
        PrintAddressArray(Written, Tracked.Base.Data);
        
        Tracked.Base.Data[11*4096] = 1;
        Tracked.Base.Data[11*4096 + 10] = 2;
        Tracked.Base.Data[22*4096 + 291] = 3;
        Tracked.Base.Data[33*4096 + 382] = 4;
        Tracked.Base.Data[44*4096 + 473] = 5;
        Tracked.Base.Data[55*4096 + 948] = 6;

        PlatformConsoleOut("\t--- Pass B ---\n");
        Written = GetAndResetWrittenPages(&Tracked);
        PrintAddressArray(Written, Tracked.Base.Data);

        DeallocateTrackedBuffer(&Tracked);
    }
    else
    {
        PlatformConsoleOut("\tFAILED\n");
    }

    return 0;
}