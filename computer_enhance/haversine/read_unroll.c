#define _GNU_SOURCE

#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

global memory_arena GlobalArena;
typedef struct app_state
{
    memory_arena *Arena;
} app_state;

typedef void asm_function();

extern void Read_x1(u64 Count, u8 *Data);
extern void Read_x2(u64 Count, u8 *Data);
extern void Read_x3(u64 Count, u8 *Data);
extern void Read_x4(u64 Count, u8 *Data);
extern void Read_1x2(u64 Count, u8 *Data);
extern void Read_4x2(u64 Count, u8 *Data);
extern void Read_8x2(u64 Count, u8 *Data);
extern void Read_16x2(u64 Count, u8 *Data);
extern void Read_32x2(u64 Count, u8 *Data);
extern void Read_32x4(u64 Count, u8 *Data);
extern void Read_32x6(u64 Count, u8 *Data);
#pragma comment(lib, "win32_read_unroll.lib")

typedef struct test_function
{
    char *Name;
    asm_function *Func;
} test_function;
global test_function TestFunctions[] =
{
    // { "Read_x1", Read_x1 },
    // { "Read_x2", Read_x2 },
    // { "Read_x3", Read_x3 },
    // { "Read_x4", Read_x4 },

    // { "Read_1x2", Read_1x2 },

    // { "Read_4x2", Read_4x2 },
    // { "Read_8x2", Read_8x2 },
    // { "Read_16x2", Read_16x2 },
    { "Read_32x2", Read_32x2 },
    { "Read_32x4", Read_32x4 },
    { "Read_32x6", Read_32x6 },
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
            RepetitionTestNewTestWave(Tester, Buffer.Size, CPUTimerFreq, 3);
            
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
