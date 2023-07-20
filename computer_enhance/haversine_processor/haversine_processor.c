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
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    memory_arena *Arena = &AppState->Arena;
    PlatformConsoleOut("Usage: haversine_processor [input.json] [input.f64]\n");

    string_list *Args = PlatformGetCommandLineArgs(Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 2)
    {
        string JsonFile = Args->Entry;
        string AnswerFile = Args->Next->Entry;

        string Json = PlatformReadEntireFile(Arena, JsonFile);
        string AnswerData = PlatformReadEntireFile(Arena, AnswerFile);
        f64 *Answers = (f64 *)AnswerData.Data;

        PlatformConsoleOut("Input size: %lu\n", Json.Size);

        u32 PairCount = 0;
        f64 HaversineSum = 0.0f;
        point *Points = ParseJsonPoints(Arena, Json);
        for(point *Point = Points;
            Point;
            Point = Point->Next)
        {
            f64 Haversine = ReferenceHaversine(Point->X0, Point->Y0, Point->X1, Point->Y1, EARTH_RADIUS);
            f64 ExpectedHaversine = Answers[PairCount];

            if(((ExpectedHaversine - Haversine) > 0.01f) || 
               ((Haversine - ExpectedHaversine) > 0.01f))
            {
                PlatformConsoleOut("Error: expected %.16lf but found %.16lf\n", ExpectedHaversine, ExpectedHaversine);
            }

            HaversineSum += Haversine;
            ++PairCount;
        }
        
        HaversineSum /= PairCount;
        f64 ReferenceSum = Answers[PairCount];
        f64 Difference = HaversineSum - ReferenceSum;

        PlatformConsoleOut("Pair count: %u\n", PairCount);
        PlatformConsoleOut("Haversine Sum: %.16lf\n", HaversineSum);
        PlatformConsoleOut("\nValidation:\n");
        PlatformConsoleOut("Reference sum: %.16lf\n", ReferenceSum);
        PlatformConsoleOut("Difference: %lf\n", Difference);
    }
    else
    {
        PlatformConsoleOut("Invalid arguments\n");
    }

    return 0;
}