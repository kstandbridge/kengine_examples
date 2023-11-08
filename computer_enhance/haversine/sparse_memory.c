#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "virtual_address.h"

typedef struct sparse_buffer
{
    string Base;
} sparse_buffer;

internal b32
IsValid(sparse_buffer Buffer)
{
    b32 Result = (Buffer.Base.Data != 0);
    return Result;
}

internal sparse_buffer
AllocateSparseBuffer(u64 Size)
{
    sparse_buffer Result = {0};

    Result.Base.Data = (u8 *)VirtualAlloc(0, Size, MEM_RESERVE, PAGE_NOACCESS);
    if(Result.Base.Data)
    {
        Result.Base.Size = Size;
    }

    return Result;
}

internal void
DeallocateSparseBuffer(sparse_buffer *Buffer)
{
    if(Buffer)
    {
        VirtualFree(Buffer->Base.Data, 0, MEM_RELEASE);
        ZeroStruct(Buffer);
    }
}

internal void
EnsureMemoryIsMapped(sparse_buffer *Buffer, void *Pointer, u32 Size)
{
    VirtualAlloc(Pointer, Size, MEM_COMMIT, PAGE_READWRITE);
}

typedef struct app_state
{
    memory_arena Arena;
} app_state;

s32
MainLoop(app_memory *AppMemory)
{
    // app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);

    PlatformConsoleOut("\nSparse memory test:\n");

    sparse_buffer Sparse = AllocateSparseBuffer(Gigabytes(256));
    if(IsValid(Sparse))
    {
        u8 *Write = Sparse.Base.Data;

        u64 Offsets[] = { Gigabytes(16), Gigabytes(100), Gigabytes(200), Gigabytes(255) };

        for(u32 OffsetIndex = 0; OffsetIndex < ArrayCount(Offsets); ++OffsetIndex)
        {
            u64 Offset = Offsets[OffsetIndex];
            EnsureMemoryIsMapped(&Sparse, Write + Offset, sizeof(*Write));
            Write[Offset] = (u8)(100 + OffsetIndex);
        }

        for(u32 OffsetIndex = 0; OffsetIndex < ArrayCount(Offsets); ++OffsetIndex)
        {
            u64 Offset = Offsets[OffsetIndex];
            PlatformConsoleOut("\t%u: %u\n", OffsetIndex, Write[Offset]);
        }
    }
    else
    {
        PlatformConsoleOut("\tFAILED\n");
    }
    DeallocateSparseBuffer(&Sparse);

    return 0;
}