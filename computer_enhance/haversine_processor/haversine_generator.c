#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "haversine.h"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

typedef enum method_type
{
    Method_Cluster,
    Method_Uniform
} method_type;

s32
MainLoop(app_memory *AppMemory)
{
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);   

    PlatformConsoleOut("Usage: haversine_generator [uniform/cluster] [random seed] [number of coordinate pairs to generate]\n");

    string_list *Args = PlatformGetCommandLineArgs(&AppState->Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 3)
    {
        u64 RandomSeed = 0;
        u32 PairCount = 0;
        f64 ExpectedSum = 0.0f;
        method_type Method;
        if(StringsAreEqual(String("cluster"), Args->Entry))
        {
            Method = Method_Cluster;
            PlatformConsoleOut("Method: cluster\n");
        }
        else if(StringsAreEqual(String("uniform"), Args->Entry))
        {
            Method = Method_Uniform;
            PlatformConsoleOut("Method: uniform\n");
        }
        else
        {
            PlatformConsoleOut("Invalid method \"%S\" using uniform\nMethod: uniform\n", Args->Entry);
            Method = Method_Uniform;
        }
       
        ParseFromString(Args->Next->Entry, "%lu", &RandomSeed);       
        ParseFromString(Args->Next->Next->Entry, "%u", &PairCount);
        PlatformConsoleOut("Random seed: %lu\n", RandomSeed);
        PlatformConsoleOut("Pair count: %u\n", PairCount);

        platform_file JsonFile = PlatformOpenFile(String("output.json"), FileAccess_Write);
        platform_file FloatFile = PlatformOpenFile(String("output.f64"), FileAccess_Write);
        PlatformWriteFile(&JsonFile, String("{\n\t\"pairs\":\n\t\t[\n"));

        b32 First = true;
        random_state Random = 
        {
            .Value = RandomSeed
        };

        v2 Clusters[64];
        for(u32 Index = 0;
            Index < ArrayCount(Clusters);
            ++Index)
        {
            Clusters[Index] = V2(RandomF32Between(&Random, 0.0f, 90.0f),
                                 RandomF32Between(&Random, 0.0f, 90.0f));
        }

        for(s32 Index = 0;
            Index < PairCount;
            ++Index)
        {
            if(First)
            {
                First = false;
            }
            else
            {
                PlatformWriteFile(&JsonFile, String(",\n"));
            }
            temporary_memory Temp = BeginTemporaryMemory(&AppState->Arena);

            f32 MinX, MinY, MaxX, MaxY;
            if(Method == Method_Uniform)
            {
                MinX = 0.0f;
                MinY = 0.0f;
                MaxX = 100.0f;
                MaxY = 100.0f;
            }
            else
            {
                Assert(Method == Method_Cluster);
                v2 Cluster = Clusters[RandomU32Between(&Random, 0, ArrayCount(Clusters))];
                MinX = Cluster.X;
                MaxX = MinX + 10.0f;
                MinY = Cluster.Y;
                MaxY = MinY + 10.0f;
            }

            f32 X0 = RandomF32Between(&Random, MinX, MaxX);
            f32 Y0 = RandomF32Between(&Random, MinY, MaxY);
            f32 X1 = RandomF32Between(&Random, MinX, MaxX);
            f32 Y1 = RandomF32Between(&Random, MinY, MaxY);

            f64 Haversine = ReferenceHaversine(X0, Y0, X1, Y1, EARTH_RADIUS);
            ExpectedSum += Haversine;

            PlatformWriteFile(&FloatFile, String_(sizeof(Haversine), (u8 *)&Haversine));

            PlatformWriteFile(&JsonFile, FormatString(Temp.Arena, "\t\t\t{ \"x0\": %lf, \"y0\": %lf, \"x1\": %lf, \"y1\": %lf }", 
                                                X0, Y0, X1, Y1));
            EndTemporaryMemory(Temp);
        }

        ExpectedSum /= PairCount;

        PlatformWriteFile(&FloatFile, String_(sizeof(ExpectedSum), (u8 *)&ExpectedSum));
        PlatformCloseFile(&FloatFile);

        PlatformWriteFile(&JsonFile, String("\n\t\t]\n}"));
        PlatformCloseFile(&JsonFile);

        PlatformConsoleOut("Expected sum: %f\n", ExpectedSum);
    }
    else
    {
        PlatformConsoleOut("Invalid arguments\n");
    }

    return 0;
}