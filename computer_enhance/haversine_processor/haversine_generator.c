#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

s32
MainLoop(app_memory *AppMemory)
{
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);   

    PlatformConsoleOut("Usage: haversine_generator [uniform/cluster] [random seed] [number of coordinate pairs to generate]\n");

    string_list *Args = PlatformGetCommandLineArgs(&AppState->Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 3)
    {
        string Method = Args->Entry;
        string RandomSeed = Args->Next->Entry;
        string PairCount = Args->Next->Next->Entry;
        PlatformConsoleOut("Method: %S\n", Method);
        PlatformConsoleOut("Random seed: %S\n", RandomSeed);
        PlatformConsoleOut("Pair count: %S\n", PairCount);
        PlatformConsoleOut("Expected sum: %u\n", 1234);
    }
    else
    {
        PlatformConsoleOut("Invalid arguments\n");
    }

    platform_file File = PlatformOpenFile(String("output.txt"), FileAccess_Write);
    PlatformWriteFile(&File, String("123"));
    PlatformWriteFile(&File, String("456"));
    PlatformWriteFile(&File, String("789"));
    PlatformCloseFile(&File);

    return 0;
}