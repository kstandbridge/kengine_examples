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

extern void MOVAllBytesASM(u64 Count, u8 *Data, u64 Mask);
#pragma comment(lib, "win32_memory_bandwidth.lib")

typedef struct test_function
{
    char *Name;
    asm_function *Func;
    u64 Mask;
} test_function;
global test_function TestFunctions[] =
{
    { "1GB",   MOVAllBytesASM, 0b1111111111111111111111111111111111111111111111111111111111111111 },
    { "16MB",  MOVAllBytesASM, 0b111111111111111111111111 },
    { "4MB",   MOVAllBytesASM, 0b1111111111111111111111 },
    { "1MB",   MOVAllBytesASM, 0b11111111111111111111 },
    { "8192K", MOVAllBytesASM, 0b1111111111111 },
    { "4096K", MOVAllBytesASM, 0b111111111111 },

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
        .Data = PlatformAllocateMemory(BufferSize, 0)->Base,
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
                TestFunc.Func(Buffer.Size, Buffer.Data, TestFunc.Mask);
                RepetitionTestEndTime(Tester);
                RepetitionTestCountBytes(Tester, Buffer.Size);
            }
        }
    }

    return 0;
}
