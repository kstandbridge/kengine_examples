
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

extern void DoubleLoopRead_32x8(u64 Count, u8 *Data, u64 InnerLoopCount);
#pragma comment(lib, "win32_npt_cache_test.lib")

s32
MainLoop(app_memory *AppMemory)
{
    GlobalArena.MinimumBlockSize = Gigabytes(2);
    app_state *AppState = AppMemory->AppState = PushStruct(&GlobalArena, app_state);
    memory_arena *Arena = AppState->Arena = &GlobalArena;

    HANDLE ProcessHandle = GetCurrentProcess();
    u64 CPUTimerFreq = EstimateCPUTimerFrequency();

    u64 RegionSizes[64] = {0};
    repetition_tester Testers[ArrayCount(RegionSizes)] = {0};
    u64 InnerLoopSize = 256;

    u64 SizeDelta = 512;
    u64 AccumulatedSize = 4*1024;
    for(u32 TestIndex = 0; TestIndex < ArrayCount(Testers); ++TestIndex)
    {
        RegionSizes[TestIndex] = AccumulatedSize;

        // NOTE(kstandbridge): Increase delta when we hit power of two to avoid too many tests
        if(((AccumulatedSize - 1) & AccumulatedSize) == 0)
        {
            SizeDelta *= 2;
        }
        AccumulatedSize += SizeDelta;
    }

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

    for(u32 TestIndex = 0; TestIndex < ArrayCount(Testers); ++TestIndex)
    {
        repetition_tester *Tester = Testers + TestIndex;

        u64 RegionSize = RegionSizes[TestIndex];

        u64 OuterLoopCount = Buffer.Size/RegionSize;
        u64 InnerLoopCount = RegionSize/InnerLoopSize;
        u64 TotalSize = OuterLoopCount*RegionSize;


        PlatformConsoleOut("\n--- Read32x8 %llu outer x %llu inner x %llu bytes = %llu bytes in %lluk chunks --- \n",
                           OuterLoopCount, InnerLoopCount, InnerLoopSize, TotalSize, RegionSize/1024);

        RepetitionTestNewTestWave(Tester, TotalSize, CPUTimerFreq, 3);
        
        while(RepetitionTestIsTesting(Tester))
        {
            RepetitionTestBeginTime(Tester);
            DoubleLoopRead_32x8(OuterLoopCount, Buffer.Data, InnerLoopCount);
            RepetitionTestEndTime(Tester);
            RepetitionTestCountBytes(Tester, TotalSize);
        }
    }

    PlatformConsoleOut("Region Size,gb/s\n");
    for(u32 TestIndex = 0; TestIndex < ArrayCount(Testers); ++TestIndex)
    {
        repetition_tester *Tester = Testers + TestIndex;

        repetition_value Value = Tester->Results.Min;
        f64 Seconds = SecondsFromCPUTime((f64)Value.E[RepValue_CPUTimer], Tester->CPUTimerFreq);
        f64 Gigabyte = Gigabytes(1);
        f64 Bandwidth = Value.E[RepValue_ByteCount] / (Gigabyte * Seconds);

        PlatformConsoleOut("%llu,%f\n", (1ull << TestIndex), Bandwidth);
    }

    return 0;
}
