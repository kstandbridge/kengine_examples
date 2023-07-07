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
    b32 DisplayClocks = false;
    b32 DumpMemory = false;
    
    for(string_list *Arg = Args;
        Arg;
        Arg = Arg->Next)
    {
        if(StringsAreEqual(Arg->Entry, String("-dump")))
        {
            DumpMemory = true;
        }
        else if(StringsAreEqual(Arg->Entry, String("-clocks")))
        {
            DisplayClocks = true;
        }
        else
        {
            FilePath = Arg->Entry;
        }
    }
    
    if(!PlatformFileExists(FilePath))
    {
        Result = 1;
        PlatformConsoleOut("File not found: \"%S\"\n", FilePath);
    }
    else
    {
        string File = PlatformReadEntireFile(&AppState->Arena, FilePath);
        simulator_context Context = GetSimulatorContext(&AppState->Arena, File.Data, File.Size);
        Context.DisplayClocks = DisplayClocks;
        
        while(Context.InstructionStreamAt < Context.InstructionStreamSize)
        {        
            string Output = SimulateStep(&Context);
            PlatformConsoleOut("%S\n", Output);
        }
        
        PlatformConsoleOut("\n\nFinal registers:\n");
        string Output = GetRegisterDetails(&Context);
        PlatformConsoleOut("%S\n", Output);
        
        if(DumpMemory)
        {
            PlatformWriteTextToFile(String_(Megabytes(1), Context.Memory), String("memory.data"));
        }
    }
    
    return Result;
}

#include "sim8086.c"