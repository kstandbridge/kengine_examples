#pragma once

typedef struct decomposed_virtual_address
{
    u16 PML4Index;
    u16 DirectoryPtrIndex;
    u16 DirectoryIndex;
    u16 TableIndex;
    u32 Offset;
} decomposed_virtual_address;

internal void
Print(decomposed_virtual_address Address)
{
    PlatformConsoleOut("|%3u|%3u|%3u|%3u|%10u",
                       Address.PML4Index, Address.DirectoryPtrIndex,
                       Address.DirectoryIndex, Address.TableIndex,
                       Address.Offset);
}

internal void
PrintAsLine(string Label, decomposed_virtual_address Address)
{
    PlatformConsoleOut("%S", Label);
    Print(Address);
    PlatformConsoleOut("\n");
}

internal decomposed_virtual_address
DecomposePointer4K(void *Ptr)
{
    u64 Address = (u64)Ptr;
    decomposed_virtual_address Result = 
    {
        .PML4Index = ((Address >> 39) & 0x1ff),
        .DirectoryPtrIndex = ((Address >> 30) & 0x1ff),
        .DirectoryIndex = ((Address >> 21) & 0x1ff),
        .TableIndex = ((Address >> 12) & 0x1ff),
        .Offset = ((Address >> 0) & 0xfff),
    };

    return Result;
}

internal decomposed_virtual_address
DecomposePointer2MB(void *Ptr)
{
    u64 Address = (u64)Ptr;
    decomposed_virtual_address Result = 
    {
        .PML4Index = ((Address >> 39) & 0x1ff),
        .DirectoryPtrIndex = ((Address >> 30) & 0x1ff),
        .DirectoryIndex = ((Address >> 21) & 0x1ff),
        .Offset = ((Address >> 0) & 0x1fffff),
    };

    return Result;
}

internal decomposed_virtual_address
DecomposePointer1GB(void *Ptr)
{
    u64 Address = (u64)Ptr;
    decomposed_virtual_address Result = 
    {
        .PML4Index = ((Address >> 39) & 0x1ff),
        .DirectoryPtrIndex = ((Address >> 30) & 0x1ff),
        .DirectoryIndex = ((Address >> 21) & 0x1ff),
        .Offset = ((Address >> 0) & 0x3fffffff),
    };

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

internal void
PrintBinaryBits(u64 Value, u32 FirstBit, u32 BitCount)
{
    for(u32 BitIndex = 0; BitIndex < BitCount; ++BitIndex)
    {
        u64 Bit = (Value >> ((BitCount - 1 - BitIndex) + FirstBit)) & 1;
        PlatformConsoleOut("%s", Bit ? "1" : "0");
    }
}
