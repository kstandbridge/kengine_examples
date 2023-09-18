#define _GNU_SOURCE

#define KENGINE_PROFILER
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
    BEGIN_TIMED_BANDWIDTH(SumHaversineDistance);

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

    END_TIMED_BANDWIDTH(SumHaversineDistance, PointCount*sizeof(point));

    return Result;
}

internal string
ReadEntireFile(memory_arena *Arena, string FilePath)
{
    BEGIN_TIMED_BANDWIDTH(ReadJson);

    string Result = PlatformReadEntireFile(Arena, FilePath);

    END_TIMED_BANDWIDTH(ReadJson, Result.Size);

    return Result;
}

s32
MainLoop(app_memory *AppMemory)
{
    BeginProfile();

    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    memory_arena *Arena = &AppState->Arena;
    
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

        PlatformConsoleOut("Input size: %lu\n", Json.Size);

        BEGIN_TIMED_BLOCK(ParseJsonPoints);
        point *Points = ParseJsonPoints(Arena, Json, Args->Entry);
        END_TIMED_BLOCK(ParseJsonPoints);
        
        u64 PointCount = 0;
        for(point *Point = Points;
            Point;
            Point = Point->Next)
        {
            ++PointCount;
        }

        f64 HaversineSum = SumHaversineDistance(Points, PointCount);

        PlatformConsoleOut("Pair count: %u\n", PointCount);
        PlatformConsoleOut("Haversine Sum: %.16lf", HaversineSum);

        if(Answers)
        {
            f64 ReferenceSum = Answers[PointCount];
            f64 Difference = HaversineSum - ReferenceSum;
            PlatformConsoleOut("\n\nValidation:\n");
            PlatformConsoleOut("Reference sum: %.16lf\n", ReferenceSum);
            PlatformConsoleOut("Difference: %lf\n", Difference);
        }

        PlatformConsoleOut("\n");

        EndAndPrintProfile();
    }
    else
    {
        PlatformConsoleOut("Usage: haversine_processor [input.json] [input.f64]\n");
    }

    return 0;
}
