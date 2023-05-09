#define KENGINE_WIN32
#define KENGINE_CONSOLE
#define KENGINE_IMPLEMENTATION
#include "kengine.h"

#include "sim8086.h"

typedef struct app_state
{
    memory_arena Arena;
} app_state;

s32
MainLoop(app_memory *AppMemory)
{
    s32 Result = 0;
    
    app_state *AppState = AppMemory->AppState = BootstrapPushStruct(app_state, Arena);
    
    string_list *Args = PlatformGetCommandLineArgs(&AppState->Arena);
    string FilePath = {0};
    
    for(string_list *Arg = Args;
        Arg;
        Arg = Arg->Next)
    {
        // TODO(kstandbridge): Check arg for options like silent, dump, etc
        FilePath = Arg->Entry;
    }
    
    if(!PlatformFileExists(FilePath))
    {
        Result = 1;
        LogError("File not found: \"%S\"", FilePath);
    }
    else
    {
        string File = PlatformReadEntireFile(&AppState->Arena, FilePath);
        simulator_context Context = GetSimulatorContext(&AppState->Arena, File.Data, File.Size);
        
        while(Context.InstructionStreamAt < Context.InstructionStreamSize)
        {        
            string Output = SimulateStep(&Context);
            PlatformConsoleOut("%S\n", Output);
        }
        
        PlatformConsoleOut("\n\nFinal registers:\n");
        string Output = GetRegisterDetails(&Context);
        PlatformConsoleOut("%S", Output);
    }
    
    return Result;
}

#include "sim8086.c"