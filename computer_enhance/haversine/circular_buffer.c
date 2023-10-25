#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "virtual_address.h"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

// NOTE(kstandbridge): Only defined in newer versions of Windows SDK
#ifndef MEM_REPLACE_PLACEHOLDER

#define MEM_REPLACE_PLACEHOLDER 0x4000
#define MEM_RESERVE_PLACEHOLDER 0x40000
#define MEM_PRESERVE_PLACEHOLDER 0x2
#define MemExtendedParameterAddressRequirements 1

struct MEM_EXTENDED_PARAMETER;

#endif
typedef PVOID virtual_alloc_2(HANDLE                 Process,
                              PVOID                  BaseAddress,
                              SIZE_T                 Size,
                              ULONG                  AllocationType,
                              ULONG                  PageProtection,
                              MEM_EXTENDED_PARAMETER *ExtendedParameters,
                              ULONG                  ParameterCount);

typedef PVOID map_view_of_file_3(HANDLE                 FileMapping,
                                 HANDLE                 Process,
                                 PVOID                  BaseAddress,
                                 ULONG64                Offset,
                                 SIZE_T                 ViewSize,
                                 ULONG                  AllocationType,
                                 ULONG                  PageProtection,
                                 MEM_EXTENDED_PARAMETER *ExtendedParameters,
                                 ULONG                  ParameterCount);

typedef struct circular_buffer
{
    string Base;

    HANDLE FileMapping;
    u32 RepCount;
} circular_buffer;

internal b32
IsBufferValid(circular_buffer Buffer)
{
    b32 Result = (Buffer.Base.Data != 0);
    return Result;
}

internal u64
RoundToPow2Size(u64 MinimumSize, u64 Pow2Size)
{
    u64 Result = (MinimumSize + Pow2Size - 1) & ~(Pow2Size - 1);
    return Result;
}

internal void
UnmapCircularBuffer(u8 *Base, u64 Size, u32 RepCount)
{
    for(u32 RepIndex = 0; RepIndex < RepCount; ++RepIndex)
    {
        UnmapViewOfFile(Base + RepIndex*Size);
    }
}

internal void
DeallocateCircularBuffer(circular_buffer *Buffer)
{
    if(Buffer)
    {
        if(Buffer->FileMapping != INVALID_HANDLE_VALUE)
        {
            UnmapCircularBuffer(Buffer->Base.Data, Buffer->Base.Size, Buffer->RepCount);
            CloseHandle(Buffer->FileMapping);
        }

        ZeroStruct(Buffer);
    }
}

internal circular_buffer
AllocateCircularBuffer(u64 MinimumSize, u32 RepCount)
{
    circular_buffer Result = {0};

    // NOTE(kstandbridge): We need to align the allocation size to the allocation granularity
    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    u64 DataSize = RoundToPow2Size(MinimumSize, SystemInfo.dwAllocationGranularity);
    u64 TotalRepeatedSize = RepCount * DataSize;

    Result.FileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE,
                                           (DWORD)(DataSize >> 32), (DWORD)(DataSize & 0xffffffff), 0);
    
    Result.RepCount = RepCount;

    if(Result.FileMapping != INVALID_HANDLE_VALUE)
    {
        HMODULE Kernel = LoadLibraryA("kernelbase.dll");
        virtual_alloc_2 *VirtualAlloc2 = (virtual_alloc_2 *)GetProcAddress(Kernel, "VirtualAlloc2");
        map_view_of_file_3 *MapViewOfFile3 = (map_view_of_file_3 *)GetProcAddress(Kernel, "MapViewOfFile3");

        if(VirtualAlloc2 && MapViewOfFile3)
        {
            // NOTE(kstandbridge): Newer apis allow us to allocate ring buffers without using placeholders

            u8 *BasePtr = (u8 *)VirtualAlloc2(0, 0, TotalRepeatedSize, MEM_RESERVE|MEM_RESERVE_PLACEHOLDER, PAGE_NOACCESS, 0, 0);

            b32 Mapped = true;
            for(u32 RepIndex = 0; RepIndex < RepCount; ++RepIndex)
            {
                VirtualFree(BasePtr + RepIndex*DataSize, DataSize, MEM_RELEASE|MEM_PRESERVE_PLACEHOLDER);
                if(!MapViewOfFile3(Result.FileMapping, 0, BasePtr + RepIndex*DataSize,
                                   0, DataSize, MEM_REPLACE_PLACEHOLDER, PAGE_READWRITE, 0, 0))
                {
                    Mapped = false;
                }
            }

            if(Mapped)
            {
                Result.Base.Data = BasePtr;
                Result.Base.Size = DataSize;
            }
        }
        else
        {
            // NOTE(kstandbridge): Older APIs we need to try multiple times incase someone uses the address range
            // between testing the allocation and mapping

            for(u32 AttemptIndex = 0; AttemptIndex < 100; ++AttemptIndex)
            {
                u8 *BasePtr = (u8 *)VirtualAlloc(0, TotalRepeatedSize, MEM_RESERVE, PAGE_NOACCESS);
                if(BasePtr)
                {
                    VirtualFree(BasePtr, 0, MEM_RELEASE);

                    b32 Mapped = true;
                    for(u32 RepIndex = 0; RepIndex < RepCount; ++RepIndex)
                    {
                        if(!MapViewOfFileEx(Result.FileMapping, FILE_MAP_ALL_ACCESS, 0, 0, DataSize, BasePtr + RepIndex*DataSize))
                        {
                            Mapped = false;
                            break;
                        }
                    }

                    if(Mapped)
                    {
                        Result.Base.Data = BasePtr;
                        Result.Base.Size = DataSize;
                        break;
                    }
                    else
                    {
                        UnmapCircularBuffer(BasePtr, DataSize, RepCount);
                    }
                }
            }
        }
    }

    if(!Result.Base.Data)
    {
        DeallocateCircularBuffer(&Result);
    }

    return Result;
}

s32
MainLoop(app_memory *AppMemory)
{
    // app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);

    PlatformConsoleOut("Circulr buffer test:\n");

    circular_buffer Circular = AllocateCircularBuffer(64*4096, 3);
    if(IsBufferValid(Circular))
    {
        s32 Size = (s32)Circular.Base.Size;
        u8 *Data = Circular.Base.Data + Size;
        Data[0] = 123;

        PlatformConsoleOut("\t[%8d]: %u\n", -Size, Data[-Size]);
        PlatformConsoleOut("\t[%8d]: %u\n", 0, Data[0]);
        PlatformConsoleOut("\t[%8d]: %u\n", Size, Data[Size]);

        DeallocateCircularBuffer(&Circular);
    }
    else
    {
        PlatformConsoleOut("\tFAILED\n");
    }

    return 0;
}