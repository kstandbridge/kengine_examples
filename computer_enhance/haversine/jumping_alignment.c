#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

global memory_arena GlobalArena;
typedef struct app_state
{
    memory_arena *Arena;
} app_state;

typedef void asm_function(u64 Count, u8 *Data);

extern void NOPAligned64(u64 Count, u8 *Data);
extern void NOPAligned1(u64 Count, u8 *Data);
extern void NOPAligned15(u64 Count, u8 *Data);
extern void NOPAligned31(u64 Count, u8 *Data);
extern void NOPAligned63(u64 Count, u8 *Data);
#pragma comment(lib, "win32_jumping_alignment.lib")

typedef struct test_function
{
    char *Name;
    asm_function *Func;
} test_function;
global test_function TestFunctions[] =
{
    { "NOPAligned64", NOPAligned64 },
    { "NOPAligned1", NOPAligned1 },
    { "NOPAligned15", NOPAligned15 },
    { "NOPAligned31", NOPAligned31 },
    { "NOPAligned63", NOPAligned63 },
};

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

    repetition_tester Testers[ArrayCount(TestFunctions)] = {0};
    for(;;)
    {
        for(u32 FuncIndex = 0; FuncIndex < ArrayCount(TestFunctions); ++FuncIndex)
        {
            repetition_tester *Tester = &Testers[FuncIndex];
            test_function TestFunc = TestFunctions[FuncIndex];

            PlatformConsoleOut("\n--- %s ---\n", TestFunc.Name);
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

    return 0;
}