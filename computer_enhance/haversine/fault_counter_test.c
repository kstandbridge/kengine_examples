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
WriteToAllBytesBackwards(repetition_tester *Tester, read_parameters *Params)
{
    while(RepetitionTestIsTesting(Tester))
    {
        string DestBuffer = Params->Dest;
        HandleAllocation(Params, &DestBuffer);

        RepetitionTestBeginTime(Tester);
        for(u64 Index = 0; Index < DestBuffer.Size; ++Index)
        {
            DestBuffer.Data[(DestBuffer.Size - Index)] = (u8)Index;
        }
        RepetitionTestEndTime(Tester);

        RepetitionTestCountBytes(Tester, DestBuffer.Size);

        HandleDeallocation(Params, &DestBuffer);
    }
}

typedef struct test_fuction
{
    char *Name;
    read_overhead_test_func *Func;
} test_function;
test_function TestFunctions[] =
{
    { "WriteToAllBytes", WriteToAllBytes },
    { "WriteToAllBytesBackwards", WriteToAllBytesBackwards },
};

int
MainLoop(app_memory *AppMemory)
{
    GlobalArena.MinimumBlockSize = Gigabytes(2);
    AppMemory->AppState = PushStruct(&GlobalArena, app_state);

    u64 CPUTimerFreq = EstimateCPUTimerFrequency();

    read_parameters Params =
    {
        .Dest = AllocateBuffer(Gigabytes(1)),
        .FilePath = "/dev/null",
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
    
    return 0;
}
