#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "kengine_profiler.c"

#include "haversine.h"
#include "haversine.c"

typedef struct app_state
{
    memory_arena Arena;
} app_state;


internal f64
SumHaversineDistance(point *Points, u64 PointCount)
{
    BEGIN_TIMED_FUNCTION();

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

    END_TIMED_FUNCTION();

    return Result;
}

internal string
ReadEntireFile(memory_arena *Arena, string FilePath)
{
    BEGIN_TIMED_FUNCTION();

    string Result = PlatformReadEntireFile(Arena, FilePath);

    END_TIMED_FUNCTION();

    return Result;
}

s32
MainLoop(app_memory *AppMemory)
{
    BeginProfile();

    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    memory_arena *Arena = &AppState->Arena;
    PlatformConsoleOut("Usage: haversine_processor [input.json] [input.f64]\n");
    
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

        point *Points = ParseJsonPoints(Arena, Json, Args->Entry);


        u64 PointCount = 0;
        for(point *Point = Points;
            Point;
            Point = Point->Next)
        {
            BEGIN_TIMED_BLOCK(CountPoint);
            ++PointCount;
            END_TIMED_BLOCK(CountPoint);
        }
        

        f64 HaversineSum = SumHaversineDistance(Points, PointCount);

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

        PlatformConsoleOut("\n");

        EndAndPrintProfile();
    }
    else
    {
        PlatformConsoleOut("Invalid arguments\n");
    }

    return 0;
}