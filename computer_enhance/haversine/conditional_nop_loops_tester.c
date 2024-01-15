
#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#pragma comment(lib, "bcrypt.lib")

global memory_arena GlobalArena;
typedef struct app_state
{
    memory_arena *Arena;
} app_state;

typedef void asm_function(u64 Count, u8 *Data);

extern void ConditionalNOP(u64 Count, u8 *Data);
#pragma comment(lib, "win32_conditional_nop_loops.lib")

typedef struct test_function
{
    char *Name;
    asm_function *Func;
} test_function;
global test_function TestFunctions[] =
{
    { "ConditionalNOP", ConditionalNOP },
};

typedef enum branch_pattern_type
{
    BranchPattern_NeverTaken,
    BranchPattern_AlwaysTaken,
    BranchPattern_Every2,
    BranchPattern_Every3,
    BranchPattern_Every4,
    BranchPattern_CRTRandom,
    BranchPattern_OSRandom,

    BranchPattern_Count,
} branch_pattern_type;

internal u64
GetMaxOSRandomCount()
{
    u64 Result = 0xffffffff;
    return Result;
}

internal b32
ReadOSRandomBytes(u64 Count, void *Dest)
{
    b32 Result = false;

    if(Count < GetMaxOSRandomCount())
    {
        Result = (BCryptGenRandom(0, (BYTE *)Dest, (u32)Count, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0);
    }

    return Result;
}

internal void
FillWithRandomBytes(string Dest)
{
    u64 MaxRandCount = GetMaxOSRandomCount();
    u64 AtOffset = 0;
    while(AtOffset < Dest.Size)
    {
        u64 ReadCount = Dest.Size - AtOffset;
        if(ReadCount > MaxRandCount)
        {
            ReadCount = MaxRandCount;
        }

        ReadOSRandomBytes(ReadCount, Dest.Data + AtOffset);
        AtOffset += ReadCount;
    }
}

internal char *
FillWithBranchPattern(branch_pattern_type Pattern, string Buffer)
{
    char *PatternName = "UNKNOWN";

    if(Pattern == BranchPattern_OSRandom)
    {
        PatternName = "OSRandom";
        FillWithRandomBytes(Buffer);
    }
    else
    {
        for(u64 Index = 0; Index < Buffer.Size; ++Index)
        {
            u8 Value = 0;

            switch(Pattern)
            {
                case BranchPattern_NeverTaken:
                {
                    PatternName = "Never Taken";
                    Value = 0;
                } break;

                case BranchPattern_AlwaysTaken:
                {
                    PatternName = "Always Taken";
                    Value = 1;
                } break;

                case BranchPattern_Every2:
                {
                    PatternName = "Every 2";
                    Value = ((Index % 2) == 0);
                } break;

                case BranchPattern_Every3:
                {
                    PatternName = "Every 3";
                    Value = ((Index % 3) == 0);
                } break;

                case BranchPattern_Every4:
                {
                    PatternName = "Every 4";
                    Value = ((Index % 4) == 0);
                } break;
                
                case BranchPattern_CRTRandom:
                {
                    PatternName = "CRTRandom";
                    // NOTE(kstandbridge): rand() isn't very random
                    Value = (u8)rand();
                } break;

                default:
                {
                    PlatformConsoleOut("Error: Unrecognized branch pattern.\n");
                }
            }

            Buffer.Data[Index] = Value;
        }
    }

    return PatternName;
}

s32
MainLoop(app_memory *AppMemory)
{
    GlobalArena.MinimumBlockSize = Gigabytes(2);
    app_state *AppState = AppMemory->AppState = PushStruct(&GlobalArena, app_state);
    memory_arena *Arena = AppState->Arena = &GlobalArena;

    HANDLE ProcessHandle = GetCurrentProcess();
    u64 CPUTimerFreq = EstimateCPUTimerFrequency();

    u64 BufferSize = Gigabytes(1);
    string Buffer =
    {
        .Data = (u8 *)malloc(BufferSize),
        .Size = BufferSize,
    };

    repetition_tester Testers[BranchPattern_Count][ArrayCount(TestFunctions)] = {0};
    for(;;)
    {
        for(u32 PatternIndex = 0; PatternIndex < BranchPattern_Count; ++PatternIndex)
        {
            char *PatternName = FillWithBranchPattern((branch_pattern_type)PatternIndex, Buffer);

            for(u32 FuncIndex = 0; FuncIndex < ArrayCount(TestFunctions); ++FuncIndex)
            {
                repetition_tester *Tester = &Testers[PatternIndex][FuncIndex];
                test_function TestFunc = TestFunctions[FuncIndex];

                PlatformConsoleOut("\n--- %s, %s ---\n", TestFunc.Name, PatternName);
                RepetitionTestNewTestWave(Tester, Buffer.Size, CPUTimerFreq, 10);
                
                while(RepetitionTestIsTesting(Tester))
                {
                    RepetitionTestBeginTime(Tester);
                    TestFunc.Func(Buffer.Size, Buffer.Data);
                    RepetitionTestEndTime(Tester);
                    RepetitionTestCountBytes(Tester, Buffer.Size);
                }

            }
        }
    }

    return 0;
}