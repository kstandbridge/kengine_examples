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

extern void Read_32x8(u64 Count, u8 *Data, u64 Mask);
#pragma comment(lib, "win32_memory_bandwidth.lib")

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

    // NOTE(kstandbridge): Ensure pages are mapped by writing garbage to them
    for(u64 ByteIndex = 0; ByteIndex < Buffer.Size; ++ByteIndex)
    {
        Buffer.Data[ByteIndex] = (u8)ByteIndex;
    }

    u32 MinSizeIndex = 10;
    repetition_tester Testers[30] = {0};
    for(u32 FuncIndex = MinSizeIndex; FuncIndex < ArrayCount(Testers); ++FuncIndex)
    {
        repetition_tester *Tester = Testers + FuncIndex;

        u64 RegionSize = (1ull << FuncIndex);
        u64 RegionMask = RegionSize - 1;

        PlatformConsoleOut("\n--- Read32x8 of %lluk ---\n", RegionSize/1024);
        RepetitionTestNewTestWave(Tester, Buffer.Size, CPUTimerFreq, 3);
        
        while(RepetitionTestIsTesting(Tester))
        {
            RepetitionTestBeginTime(Tester);
            Read_32x8(Buffer.Size, Buffer.Data, RegionMask);
            RepetitionTestEndTime(Tester);
            RepetitionTestCountBytes(Tester, Buffer.Size);
        }
    }

    PlatformConsoleOut("Region Size,gb/s\n");
    for(u32 FuncIndex = MinSizeIndex; FuncIndex < ArrayCount(Testers); ++FuncIndex)
    {
        repetition_tester *Tester = Testers + FuncIndex;

        repetition_value Value = Tester->Results.Min;
        f64 Seconds = SecondsFromCPUTime((f64)Value.E[RepValue_CPUTimer], Tester->CPUTimerFreq);
        f64 Gigabyte = Gigabytes(1);
        f64 Bandwidth = Value.E[RepValue_ByteCount] / (Gigabyte * Seconds);

        PlatformConsoleOut("%llu,%f\n", (1ull << FuncIndex), Bandwidth);
    }

    return 0;
}
