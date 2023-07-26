#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "haversine.h"
#include "haversine.c"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

internal f64
SumHaversineDistance(point *Points, u64 PointCount)
{
    f64 Result = 0;

    f64 Coefficient = 1 / (f64)PointCount;
    for(point *Point = Points;
            Point;
            Point = Point->Next)
        {
            f64 Haversine = ReferenceHaversine(Point->X0, Point->Y0, Point->X1, Point->Y1, EARTH_RADIUS);
            
#if 0
            f64 ExpectedHaversine = Answers[PairCount];

            if(((ExpectedHaversine - Haversine) >= FLT_EPSILON) && 
               ((Haversine - ExpectedHaversine) >= FLT_EPSILON))
            {
                PlatformConsoleOut("Error: expected %.16lf but found %.16lf\n", ExpectedHaversine, ExpectedHaversine);
            }
#endif

            Result += Coefficient * Haversine;
        }
        

    return Result;
}

internal string
ReadEntireFile(memory_arena *Arena, string FilePath)
{
    string Result = PlatformReadEntireFile(Arena, FilePath);

    return Result;
}

internal void
PrintTimeElapsed(string Label, u64 TotalElapsed, u64 Begin, u64 End)
{
    u64 Elapsed = End - Begin;
    f64 Percent = 100.0f * ((f64)Elapsed / (f64)TotalElapsed);
    PlatformConsoleOut("\t%S\t%lu\t(%.2f%%)\n", Label, Elapsed, Percent);
}

internal u64
PlatformEstimateCPUTimerFrequency()
{
    u64 Result = 0;

    u64 MillisecondsToWait = 100;
    u64 OSFrequency = PlatformGetOSTimerFrequency();

    u64 CPUStart = PlatformReadCPUTimer();
    u64 OSStart = PlatformReadOSTimer();
    u64 OSEnd = 0;
    u64 OSElapsed = 0;
    u64 OSWaitTime = OSFrequency * MillisecondsToWait / 1000;

    while(OSElapsed < OSWaitTime)
    {
        OSEnd = LinuxReadOSTimer();
        OSElapsed = OSEnd - OSStart;
    }

    u64 CPUEnd = PlatformReadCPUTimer();
    u64 CPUElapsed = CPUEnd - CPUStart;

    if(OSElapsed)
    {
        Result = OSFrequency * CPUElapsed / OSElapsed;
    }

    return Result;
}

s32
MainLoop(app_memory *AppMemory)
{
    u64 OSStart = PlatformReadCPUTimer();

    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    memory_arena *Arena = &AppState->Arena;
    PlatformConsoleOut("Usage: haversine_processor [input.json] [input.f64]\n");
    u64 StartEnd = PlatformReadCPUTimer();

    string_list *Args = PlatformGetCommandLineArgs(Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if((ArgsCount == 1) || (ArgsCount == 2))
    {
        string Json = ReadEntireFile(Arena, Args->Entry);
        
        f64 *Answers = 0;
        
        if(ArgsCount == 2)
        {
            string AnswerData = ReadEntireFile(Arena, Args->Next->Entry);
            Answers = (f64 *)AnswerData.Data;
        }

        u64 ReadEnd = PlatformReadCPUTimer();

        PlatformConsoleOut("Input size: %lu\n", Json.Size);

        point *Points = ParseJsonPoints(Arena, Json);
        
        u64 ParseEnd = PlatformReadCPUTimer();

        u64 PointCount = 0;
        for(point *Point = Points;
            Point;
            Point = Point->Next)
        {
            ++PointCount;
        }

        f64 HaversineSum = SumHaversineDistance(Points, PointCount);
        

        u64 SumEnd = PlatformReadCPUTimer();

        PlatformConsoleOut("Pair count: %u\n", PointCount);
        PlatformConsoleOut("Haversine Sum: %.16lf\n", HaversineSum);

        if(Answers)
        {
            f64 ReferenceSum = Answers[PointCount];
            f64 Difference = HaversineSum - ReferenceSum;
            PlatformConsoleOut("\nValidation:\n");
            PlatformConsoleOut("Reference sum: %.16lf\n", ReferenceSum);
            PlatformConsoleOut("Difference: %lf\n", Difference);
        }

        u64 OutputEnd = PlatformReadCPUTimer();
        u64 OSEnd = PlatformReadCPUTimer();

        u64 OSElapsed = OSEnd - OSStart;

        u64 CPUFrequency = PlatformEstimateCPUTimerFrequency();
        if(CPUFrequency)
        {
            PlatformConsoleOut("\nTotal time: %.4fms (CPU freq %lu)\n", (f64)OSElapsed/(f64)CPUFrequency, CPUFrequency);
        }
        
        PrintTimeElapsed(String("Startup"), OSElapsed, OSStart, StartEnd);
        PrintTimeElapsed(String("Read"), OSElapsed, StartEnd, ReadEnd);
        PrintTimeElapsed(String("Parse"), OSElapsed, ReadEnd, ParseEnd);
        PrintTimeElapsed(String("Sum"), OSElapsed, ParseEnd, SumEnd);
        PrintTimeElapsed(String("Output"), OSElapsed, SumEnd, OutputEnd);
        PlatformConsoleOut("\n");
    }
    else
    {
        PlatformConsoleOut("Invalid arguments\n");
    }

    return 0;
}