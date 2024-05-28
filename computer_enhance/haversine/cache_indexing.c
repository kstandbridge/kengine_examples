
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

extern void ReadStrided_32x2(u64 Count, u8 *Data, u64 ReadsPerBlock, u64 Stride);
#pragma comment(lib, "win32_cache_indexing.lib")

s32
MainLoop(app_memory *AppMemory)
{
    GlobalArena.MinimumBlockSize = Gigabytes(2);
    app_state *AppState = AppMemory->AppState = PushStruct(&GlobalArena, app_state);
    memory_arena *Arena = AppState->Arena = &GlobalArena;

    HANDLE ProcessHandle = GetCurrentProcess();
    u64 CPUTimerFreq = EstimateCPUTimerFrequency();

    repetition_tester Testers[128] = {0};
    u64 CacheLineSize = 64;
    u64 RepCount = 64;
    u64 ReadCount = 256;
    u64 TotalBytes = RepCount*ReadCount*CacheLineSize;

    u64 BufferSize = ReadCount * (CacheLineSize*ArrayCount(Testers));
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

    for(u64 StrideIndex = 0; StrideIndex < ArrayCount(Testers); ++StrideIndex)
    {
        repetition_tester *Tester = Testers + StrideIndex;

        u64 Stride = CacheLineSize*StrideIndex;
        PlatformConsoleOut("\n--- ReadStrided_32x8 of %llu lines spaced %llu byttes apart (total span: %llu) ---\n",
                           ReadCount, Stride, ReadCount*Stride);
        RepetitionTestNewTestWave(Tester, TotalBytes, CPUTimerFreq, 3);
        
        while(RepetitionTestIsTesting(Tester))
        {
            RepetitionTestBeginTime(Tester);
            ReadStrided_32x2(RepCount, Buffer.Data, ReadCount, Stride);
            RepetitionTestEndTime(Tester);
            RepetitionTestCountBytes(Tester, TotalBytes);
        }
    }

    PlatformConsoleOut("Stride,gb/s\n");
    for(u64 StrideIndex = 0; StrideIndex < ArrayCount(Testers); ++StrideIndex)
    {
        repetition_tester *Tester = Testers + StrideIndex;

        repetition_value Value = Tester->Results.Min;
        f64 Seconds = SecondsFromCPUTime((f64)Value.E[RepValue_CPUTimer], Tester->CPUTimerFreq);
        f64 Gigabyte = Gigabytes(1);
        f64 Bandwidth = Value.E[RepValue_ByteCount] / (Gigabyte * Seconds);

        u64 Stride = CacheLineSize*StrideIndex;
        PlatformConsoleOut("%llu,%f\n", Stride, Bandwidth);
    }

    return 0;
}
