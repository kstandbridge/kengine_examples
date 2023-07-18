#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "haversine.h"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

s32
MainLoop(app_memory *AppMemory)
{
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);

    PlatformConsoleOut("Usage: haversine_processor [input.json] [input.f64]\n");

    string_list *Args = PlatformGetCommandLineArgs(&AppState->Arena);
    u32 ArgsCount = GetStringListCount(Args);
    if(ArgsCount == 2)
    {
        string JsonFile = Args->Entry;
        string AnswerFile = Args->Next->Entry;

        PlatformConsoleOut("Using %S and %S\n", JsonFile, AnswerFile);
    }
    else
    {
        PlatformConsoleOut("Invalid arguments\n");
    }

    return 0;
}