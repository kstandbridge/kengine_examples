#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "haversine.h"
#include "haversine.c"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

s32
MainLoop(app_memory *AppMemory)
{
    u64 OSFrequency = PlatformGetOSTimerFrequency();
    u64 CPUStart = PlatformReadCPUTimer();
    u64 OSStart = PlatformReadOSTimer();

    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    memory_arena *Arena = &AppState->Arena;
    PlatformConsoleOut("Usage: haversine_processor [input.json] [input.f64]\n");
    u64 StartEnd = PlatformReadOSTimer();

    string_list *Args = PlatformGetCommandLineArgs(Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 2)
    {
        string JsonFile = Args->Entry;
        string AnswerFile = Args->Next->Entry;

        string Json = PlatformReadEntireFile(Arena, JsonFile);
        string AnswerData = PlatformReadEntireFile(Arena, AnswerFile);

        u64 ReadEnd = PlatformReadOSTimer();

        f64 *Answers = (f64 *)AnswerData.Data;

        PlatformConsoleOut("Input size: %lu\n", Json.Size);

        u32 PairCount = 0;
        f64 HaversineSum = 0.0f;
        point *Points = ParseJsonPoints(Arena, Json);
        u64 ParseEnd = PlatformReadOSTimer();

        for(point *Point = Points;
            Point;
            Point = Point->Next)
        {
            f64 Haversine = ReferenceHaversine(Point->X0, Point->Y0, Point->X1, Point->Y1, EARTH_RADIUS);
            f64 ExpectedHaversine = Answers[PairCount];

            if(((ExpectedHaversine - Haversine) >= FLT_EPSILON) && 
               ((Haversine - ExpectedHaversine) >= FLT_EPSILON))
            {
                PlatformConsoleOut("Error: expected %.16lf but found %.16lf\n", ExpectedHaversine, ExpectedHaversine);
            }

            HaversineSum += Haversine;
            ++PairCount;
        }
        
        HaversineSum /= PairCount;
        f64 ReferenceSum = Answers[PairCount];
        f64 Difference = HaversineSum - ReferenceSum;

        u64 SumEnd = PlatformReadOSTimer();

        PlatformConsoleOut("Pair count: %u\n", PairCount);
        PlatformConsoleOut("Haversine Sum: %.16lf\n", HaversineSum);
        PlatformConsoleOut("\nValidation:\n");
        PlatformConsoleOut("Reference sum: %.16lf\n", ReferenceSum);
        PlatformConsoleOut("Difference: %lf\n", Difference);

        u64 OutputEnd = PlatformReadOSTimer();

        u64 CPUEnd = PlatformReadCPUTimer();
        u64 OSEnd = PlatformReadOSTimer();

        u64 CPUElapsed = CPUEnd - CPUStart;
        u64 OSElapsed = OSEnd - OSStart;

        u64 CPUFrequency = 0;
        if(OSElapsed)
        {
            CPUFrequency = OSFrequency * CPUElapsed / OSElapsed;
        }

        PlatformConsoleOut("\nTotal time: %.4fms (CPU freq %lu)\n", 
                           (f64)OSElapsed/(f64)OSFrequency, 
                           CPUFrequency);
        PlatformConsoleOut("\tStartup:%lu\t(%.2f%%)\n", StartEnd - OSStart, ((f64)StartEnd - (f64)OSStart) / (f64)OSElapsed*100.0f);
        PlatformConsoleOut("\tRead:\t%lu\t(%.2f%%)\n", ReadEnd - StartEnd, (((f64)ReadEnd - (f64)StartEnd) / (f64)OSElapsed)*100.0f);
        PlatformConsoleOut("\tParse:\t%lu\t(%.2f%%)\n", ParseEnd - ReadEnd, (((f64)ParseEnd - (f64)ReadEnd) / (f64)OSElapsed)*100.0f);
        PlatformConsoleOut("\tSum:\t%lu\t(%.2f%%)\n", SumEnd - ParseEnd, (((f64)SumEnd - (f64)ParseEnd) / (f64)OSElapsed)*100.0f);
        PlatformConsoleOut("\tOutput:\t%lu\t(%.2f%%)\n", OutputEnd - SumEnd, (((f64)OutputEnd - (f64)SumEnd) / (f64)OSElapsed)*100.0f);

        PlatformConsoleOut("\n");
    }
    else
    {
        PlatformConsoleOut("Invalid arguments\n");
    }

    return 0;
}