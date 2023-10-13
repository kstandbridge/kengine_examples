#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

typedef struct app_state
{
    memory_arena *Arena;
} app_state;

typedef enum allocation_type
{
    AllocType_none,
    AllocType_malloc,
    AllocType_arena,

    AllocTyp_Count,
} allocation_type;

typedef struct read_parameters
{
    allocation_type AllocType;
    string Dest;
    char *FilePath;
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
        case AllocType_none:    { Result = String(""); } break;
        case AllocType_malloc:  { Result = String("malloc"); } break;
        case AllocType_arena:  { Result = String("arena"); } break;
        default:                { Result = String("UNKNOWN"); } break;
    }

    return Result;
}

global memory_arena GlobalArena;
global temporary_memory GlobalTempMemory;

internal void
HandleAllocation(read_parameters *Params, string *Buffer)
{
    switch (Params->AllocType)
    {
        case AllocType_none:
        {
            
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
HandleDeallocation(read_parameters *Params, string *Buffer)
{
    switch (Params->AllocType)
    {
        case AllocType_none:
        {
            
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

internal void
WriteToAllBytes(repetition_tester *Tester, read_parameters *Params)
{
    while(RepetitionTestIsTesting(Tester))
    {
        string DestBuffer = Params->Dest;
        HandleAllocation(Params, &DestBuffer);

        RepetitionTestBeginTime(Tester);
        for(u64 Index = 0; Index < DestBuffer.Size; ++Index)
        {
            DestBuffer.Data[Index] = (u8)Index;
        }
        RepetitionTestEndTime(Tester);

        RepetitionTestCountBytes(Tester, DestBuffer.Size);

        HandleDeallocation(Params, &DestBuffer);
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
            HandleAllocation(Params, &DestBuffer);

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

            HandleDeallocation(Params, &DestBuffer);
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
    { "fread", ReadViaFRead },
    { "ReadFile", ReadViaFRead },
};


s32
MainLoop(app_memory *AppMemory)
{
    GlobalArena.MinimumBlockSize = Gigabytes(2);
    app_state *AppState = AppMemory->AppState = PushStruct(&GlobalArena, app_state);
    memory_arena *Arena = AppState->Arena = &GlobalArena;

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
        };

        if(Params.Dest.Size > 0)
        {
            repetition_tester Testers[ArrayCount(TestFunctions)][AllocTyp_Count] = {0};

            for(;;)
            {
                for(u32 FuncIndex = 0; FuncIndex < ArrayCount(TestFunctions); ++FuncIndex)
                {
                    for(u32 AllocType = 0; AllocType < AllocTyp_Count; ++AllocType)
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
        PlatformConsoleOut("Missing file path arg\n");
    }
    
    return 0;
}
