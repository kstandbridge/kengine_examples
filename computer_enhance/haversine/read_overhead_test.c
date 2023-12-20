#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#pragma comment (lib, "advapi32.lib")

typedef struct app_state
{
    memory_arena *Arena;
} app_state;

typedef enum allocation_type
{
    AllocType_none,
    AllocType_VirtualAlloc,
    AllocType_VirtualAllocLargePages,
    AllocType_malloc,
    AllocType_arena,

    AllocTyp_Count,
} allocation_type;

typedef struct read_parameters
{
    allocation_type AllocType;
    string Dest;
    char *FilePath;
    u64 MinLargePageSize;
} read_parameters;

typedef void read_overhead_test_func(repetition_tester *Tester, read_parameters *Params);

internal string
AllocateBuffer(umm Size)
{
    string Result =
    {
        .Data = (u8 *)malloc(Size),
        .Size = Size,
    };

    return Result;
}

internal void
FreeBuffer(string *Buffer)
{
    if(Buffer->Data)
    {
        free(Buffer->Data);
    }
    ZeroStruct(Buffer);
}

internal string
AllocationTypeToString(allocation_type Type)
{
    string Result;

    switch(Type)
    {
        case AllocType_none:                    { Result = String(""); } break;
        case AllocType_VirtualAlloc:            { Result = String("VirtualAlloc"); } break;
        case AllocType_VirtualAllocLargePages:  { Result = String("VirtualAlloc (large)"); } break;
        case AllocType_malloc:                  { Result = String("malloc"); } break;
        case AllocType_arena:                   { Result = String("arena"); } break;
        default:                                { Result = String("UNKNOWN"); } break;
    }

    return Result;
}

global memory_arena GlobalArena;
global temporary_memory GlobalTempMemory;

internal void
HandleAllocation(repetition_tester *Tester, read_parameters *Params, string *Buffer)
{
    switch (Params->AllocType)
    {
        case AllocType_none:
        {
            
        } break;

        case AllocType_VirtualAlloc:
        case AllocType_VirtualAllocLargePages:
        {
            SIZE_T AllocSize = Params->Dest.Size;
            DWORD Flags = MEM_COMMIT|MEM_RESERVE;
            if(Params->AllocType == AllocType_VirtualAllocLargePages)
            {
                u64 MinLargePageSize = Params->MinLargePageSize;
                if(MinLargePageSize != 0)
                {
                    Flags |= MEM_LARGE_PAGES;

                    // NOTE(kstandbridge): Large page allocations need to be an even multiple of the large page size,
                    // even though non-large allocations are automatically rounded up for you
                    AllocSize = (AllocSize + MinLargePageSize - 1) & ~(MinLargePageSize - 1);
                }                
                else
                {
                    RepetitionTestError(Tester, "No large page support");
                }
            }

            u8 *AllocData = (u8 *)VirtualAlloc(0, AllocSize, Flags, PAGE_READWRITE);
            if(AllocData)
            {
                Buffer->Size = Params->Dest.Size;
                Buffer->Data = AllocData;
            }

        } break;

        case AllocType_malloc:
        {
            *Buffer = AllocateBuffer(Params->Dest.Size);
        } break;

        case AllocType_arena:
        {
            GlobalTempMemory = BeginTemporaryMemory(&GlobalArena);
            Buffer->Data = (u8 *)PushSize(&GlobalArena, Params->Dest.Size);
            Buffer->Size = Params->Dest.Size;
        } break;

        InvalidDefaultCase;
    
    }
}

internal void
HandleDeallocation(repetition_tester *Tester, read_parameters *Params, string *Buffer)
{
    switch (Params->AllocType)
    {
        case AllocType_none:
        {
            
        } break;

        case AllocType_VirtualAlloc:
        case AllocType_VirtualAllocLargePages:
        {
            if(Buffer->Data)
            {
                VirtualFree(Buffer->Data, 0, MEM_RELEASE);
            }
            ZeroStruct(Buffer);
        } break;

        case AllocType_malloc:
        {
            FreeBuffer(Buffer);
        } break;

        case AllocType_arena:
        {
            EndTemporaryMemory(GlobalTempMemory);
        } break;

        InvalidDefaultCase;
    
    }
}

global u64 volatile GlobalSumSink;

internal void
ReadViaMapViewOfFile(repetition_tester *Tester, read_parameters *Params)
{
    while(RepetitionTestIsTesting(Tester))
    {
        HANDLE File = CreateFileA(Params->FilePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 0,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        
        RepetitionTestBeginTime(Tester);
        HANDLE Mapping = CreateFileMappingA(File, 0, PAGE_READONLY, 0, 0, 0);
        u8 *Data = (u8 *)MapViewOfFile(Mapping, FILE_MAP_READ, 0, 0, 0);
        if(Data)
        {
            u64 TotalSize = Params->Dest.Size;
            u64 PageSize = 4096;

            u64 TestSum = 0;
            for(u64 ByteIndex = 0; ByteIndex < TotalSize; ByteIndex += PageSize)
            {
                TestSum += Data[ByteIndex];
            }

            // NOTE(kstandbridge): Trying to prevent the compiler optimizing out the above code
            GlobalSumSink += TestSum;

            RepetitionTestCountBytes(Tester, TotalSize);
        }
        else
        {
            RepetitionTestError(Tester, "Unable to read file");
        }

        RepetitionTestEndTime(Tester);

        UnmapViewOfFile(Data);
        CloseHandle(Mapping);
        CloseHandle(File);

    }
    
}

internal void
WriteToAllBytes(repetition_tester *Tester, read_parameters *Params)
{
    while(RepetitionTestIsTesting(Tester))
    {
        string DestBuffer = Params->Dest;
        HandleAllocation(Tester, Params, &DestBuffer);

        RepetitionTestBeginTime(Tester);
        for(u64 Index = 0; Index < DestBuffer.Size; ++Index)
        {
            DestBuffer.Data[Index] = (u8)Index;
        }
        RepetitionTestEndTime(Tester);

        RepetitionTestCountBytes(Tester, DestBuffer.Size);

        HandleDeallocation(Tester, Params, &DestBuffer);
    }
}

extern void MOVAllBytesASM(u64 Count, u8 *Data);
extern void NOPAllBytesASM(u64 Count);
extern void CMPAllBytesASM(u64 Count);
extern void DECAllBytesASM(u64 Count);
#pragma comment (lib, "win32_nop_loop.lib")

internal void
MOVAllBytes(repetition_tester *Tester, read_parameters *Params)
{
    while(RepetitionTestIsTesting(Tester))
    {
        string DestBuffer = Params->Dest;
        HandleAllocation(Tester, Params, &DestBuffer);

        RepetitionTestBeginTime(Tester);
        MOVAllBytesASM(DestBuffer.Size, DestBuffer.Data);
        RepetitionTestEndTime(Tester);

        RepetitionTestCountBytes(Tester, DestBuffer.Size);

        HandleDeallocation(Tester, Params, &DestBuffer);
    }
}

internal void
NOPAllBytes(repetition_tester *Tester, read_parameters *Params)
{
    while(RepetitionTestIsTesting(Tester))
    {
        string DestBuffer = Params->Dest;
        HandleAllocation(Tester, Params, &DestBuffer);

        RepetitionTestBeginTime(Tester);
        NOPAllBytesASM(DestBuffer.Size);
        RepetitionTestEndTime(Tester);

        RepetitionTestCountBytes(Tester, DestBuffer.Size);

        HandleDeallocation(Tester, Params, &DestBuffer);
    }
}

internal void
CMPAllBytes(repetition_tester *Tester, read_parameters *Params)
{
    while(RepetitionTestIsTesting(Tester))
    {
        string DestBuffer = Params->Dest;
        HandleAllocation(Tester, Params, &DestBuffer);

        RepetitionTestBeginTime(Tester);
        CMPAllBytesASM(DestBuffer.Size);
        RepetitionTestEndTime(Tester);

        RepetitionTestCountBytes(Tester, DestBuffer.Size);

        HandleDeallocation(Tester, Params, &DestBuffer);
    }
}

internal void
DECAllBytes(repetition_tester *Tester, read_parameters *Params)
{
    while(RepetitionTestIsTesting(Tester))
    {
        string DestBuffer = Params->Dest;
        HandleAllocation(Tester, Params, &DestBuffer);

        RepetitionTestBeginTime(Tester);
        DECAllBytesASM(DestBuffer.Size);
        RepetitionTestEndTime(Tester);

        RepetitionTestCountBytes(Tester, DestBuffer.Size);

        HandleDeallocation(Tester, Params, &DestBuffer);
    }
}

internal void
ReadViaFRead(repetition_tester *Tester, read_parameters *Params)
{
    while(RepetitionTestIsTesting(Tester))
    {
        FILE *File = fopen(Params->FilePath, "rb");
        if(File)
        {
            string DestBuffer = Params->Dest;
            HandleAllocation(Tester, Params, &DestBuffer);

            RepetitionTestBeginTime(Tester);
            size_t Result = fread(DestBuffer.Data, DestBuffer.Size, 1, File);
            RepetitionTestEndTime(Tester);

            if(Result == 1)
            {
                RepetitionTestCountBytes(Tester, DestBuffer.Size);
            }
            else
            {
                RepetitionTestError(Tester, "fread failed");
            }

            HandleDeallocation(Tester, Params, &DestBuffer);
            fclose(File);
        }
        else
        {
            RepetitionTestError(Tester, "fopen failed");
        }
    }
}

#if KENGINE_LINUX
internal void
ReadViaRead(repetition_tester *Tester, read_parameters *Params)
{
    while(RepetitionTestIsTesting(Tester))
    {
        s32 FileHandle = open(Params->FilePath, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(FileHandle >= 0)
        {
            string DestBuffer = Params->Dest;
            HandleAllocation(Params, &DestBuffer);


            RepetitionTestBeginTime(Tester);
            s64 Offset = 0;
            ssize_t BytesRead = pread(FileHandle, DestBuffer.Data, DestBuffer.Size, Offset);
            RepetitionTestEndTime(Tester);
        
            if(DestBuffer.Size == (umm)BytesRead)
            {
                RepetitionTestCountBytes(Tester, DestBuffer.Size);
            }
            else
            {
                RepetitionTestError(Tester, "pread failed");
            }


            HandleDeallocation(Params, &DestBuffer);
            close(FileHandle);
        }
        else
        {
            RepetitionTestError(Tester, "open failed");
        }
    }
}
#elif KENGINE_WIN32
internal void
ReadViaRead(repetition_tester *Tester, string *Output, char *FilePath)
{
    while(RepetitionTestIsTesting(Tester))
    {
        HANDLE FileHandle = CreateFileA(FilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if(FileHandle != INVALID_HANDLE_VALUE)
        {
            RepetitionTestBeginTime(Tester);
            u32 BytesRead = 0;
            ReadFile(FileHandle, Output->Data, (u32)Output->Size, (LPDWORD)&BytesRead, 0);

            RepetitionTestEndTime(Tester);
        
            if(Output->Size == (umm)BytesRead)
            {
                RepetitionTestCountBytes(Tester, Output->Size);
            }
            else
            {
                RepetitionTestError(Tester, "pread failed");
            }


            CloseHandle(FileHandle);
        }
        else
        {
            RepetitionTestError(Tester, "open failed");
        }
    }
}
#else
#error Platform not supported
#endif


typedef struct test_fuction
{
    char *Name;
    read_overhead_test_func *Func;
} test_function;
test_function TestFunctions[] =
{
    { "WriteToAllBytes", WriteToAllBytes },
    { "MOVAllBytes", MOVAllBytes },
    { "NOPAllBytes", NOPAllBytes },
    { "CMPAllBytes", CMPAllBytes },
    { "DECAllBytes", DECAllBytes },
    { "ReadViaMapViewOfFile", ReadViaMapViewOfFile },
    { "fread", ReadViaFRead },
    { "ReadFile", ReadViaFRead },
};

internal u64
PlatformEnableLargePages()
{
    u64 Result = 0;

    HANDLE TokenHandle;
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &TokenHandle))
    {
        TOKEN_PRIVILEGES TokenPrivileges = 
        {
            .PrivilegeCount = 1,
            .Privileges = 
            {
                { .Attributes = SE_PRIVILEGE_ENABLED, },
            },
        };
        if(LookupPrivilegeValue(0, SE_LOCK_MEMORY_NAME, &TokenPrivileges.Privileges[0].Luid))
        {
            AdjustTokenPrivileges(TokenHandle, FALSE, &TokenPrivileges, 0, 0, 0);
            if(GetLastError() == ERROR_SUCCESS)
            {
                Result = GetLargePageMinimum();
            }
        }

        CloseHandle(TokenHandle);
    }

    return Result;
}


s32
MainLoop(app_memory *AppMemory)
{
    GlobalArena.MinimumBlockSize = Gigabytes(2);
    app_state *AppState = AppMemory->AppState = PushStruct(&GlobalArena, app_state);
    memory_arena *Arena = AppState->Arena = &GlobalArena;

    HANDLE ProcessHandle = GetCurrentProcess();
    u64 CPUTimerFreq = EstimateCPUTimerFrequency();
    
    string_list *Args = PlatformGetCommandLineArgs(Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 1)
    {
        string FilePath = Args->Entry;
        char CPath[MAX_PATH];
        StringToCString(FilePath, sizeof(CPath), CPath);

#if KENGINE_WIN32
        struct __stat64 Stat;
        __stat64(CPath, &Stat);
#elif KENGINE_LINUX
        struct stat Stat;
        stat(CPath, &Stat);
#else 
        #error Unsupported OS 
#endif 

        read_parameters Params =
        {
            .Dest = AllocateBuffer(Stat.st_size),
            .FilePath = CPath,
            .MinLargePageSize = PlatformEnableLargePages(),
        };

        if(Params.Dest.Size > 0)
        {
            repetition_tester Testers[ArrayCount(TestFunctions)][AllocTyp_Count] = {0};

            for(;;)
            {
                for(u32 FuncIndex = 0; FuncIndex < ArrayCount(TestFunctions); ++FuncIndex)
                {
                    for(u32 AllocType = 0;
                        // AllocType < AllocTyp_Count;
                        AllocType < 1;
                        ++AllocType)
                    {
                        Params.AllocType = (allocation_type)AllocType;

                        repetition_tester *Tester = &Testers[FuncIndex][AllocType];
                        test_function TestFunc = TestFunctions[FuncIndex];

                        PlatformConsoleOut("\n--- %S%s%s ---\n",
                                           AllocationTypeToString(Params.AllocType),
                                           Params.AllocType ? " + " : "",
                                           TestFunc.Name);
                        RepetitionTestNewTestWave(Tester, Params.Dest.Size, CPUTimerFreq, 10);
                        TestFunc.Func(Tester, &Params);
                    }
                }
            }
        }
    }
    else
    {
        PlatformConsoleOut("Missing file path arg\n", 0);
    }
    
    return 0;
}
